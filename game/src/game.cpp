#include <QMessageBox>
#include <QTimer>
#include <QApplication>

#include "../include/game.h"

#include "../include/tcpmanager.h"
#include "../include/udpmanager.h"

Game::Game(int selectedPlayer, const QString& protocol, QObject* parent)
    : QObject(parent),
      m_gameView(new GameView(this)),
      gameTimer(new QTimer(this)),
      mapLoader(new MapLoader()),
      selectedPlayer(selectedPlayer),
      protocol(protocol) {
    qDebug() << "Game started with selected player:" << selectedPlayer << "and protocol:" << protocol;
    m_gameView->initialize();

    QPixmap bgImage(":/assets/Background.png");
    QSize bgSize = bgImage.size();
    hud = new HUD(m_gameView->scene(), bgSize.width());

    setupNetwork();
    connectGameTimer();
}

void Game::setupNetwork() {
    quint16 port = 12345;
    QString address = "127.0.0.1";
    NetworkManager::Role role = NetworkManager::Server; // Default to Server

    if (protocol == "TCP") {
        m_networkManager = new TCPManager(this);
    } else if (protocol == "UDP") {
        m_networkManager = new UDPManager(this);
    }

    // Try to initialize as Server
    if (!m_networkManager->initialize(role, address, port)) {
        // If server initialization fails, switch to Client
        qDebug() << "Server initialization failed. Joining as a client.";
        role = NetworkManager::Client;
        if (!m_networkManager->initialize(role, address, port)) {
            qDebug() << "Client initialization failed as well!";
            return; // Critical failure
        }
    }

    connect(m_networkManager, &NetworkManager::connectionStatusChanged, this, &Game::onConnectionStatusChanged);
    connect(m_networkManager, &NetworkManager::dataReceived, this, &Game::onDataReceived);
    connect(m_networkManager, &NetworkManager::errorOccurred, this, &Game::errorOccurred);

    if (protocol == "UDP" && role == NetworkManager::Client) {
        emit m_networkManager->connectionStatusChanged(true);
    }
}



void Game::onDataReceived(const QJsonObject& data) {
    qDebug() << "Data received:" << data;

    QString type = data[messageFieldToString(MESSAGE_FIELD::Type)].toString();
    int playerId = data[messageFieldToString(MESSAGE_FIELD::PlayerId)].toInt();
    int key = data[messageFieldToString(MESSAGE_FIELD::Key)].toInt();
    bool isPressed = data[messageFieldToString(MESSAGE_FIELD::IsPressed)].toBool();

    QPointer<Player> player = getPlayerById(playerId);

    MESSAGE_TYPE messageType = stringToMessageType(type);
    switch (messageType) {
    case MESSAGE_TYPE::PlayerDied:
        qDebug() << "Player " << playerId << " died.";
        player->die();
        gameOver(playerId);
        break;
    case MESSAGE_TYPE::PlayerMoved:
        qDebug() << "Player " << playerId << " moved.";
        player->updateDirectionState(key, isPressed);
        break;
    case MESSAGE_TYPE::PlayerPlacedBomb:
        qDebug() << "Player " << playerId << " placed a bomb.";
        player->placeBomb();
        break;
    default:
        break;
    }
}

void Game::errorOccurred(const QString& message) {
    qDebug() << "Error occurred:" << message;
}

void Game::onConnectionStatusChanged(bool connected) {
    qDebug() << "Connection status changed:" << connected;
    if (connected) {
        qDebug() << "Connected to peer.";
        if (m_networkManager->role() == NetworkManager::Client) {
            QJsonObject message;
            message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::ConnectionStatus);
            message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = 2; //should change this
            message[messageFieldToString(MESSAGE_FIELD::Key)] = "Hello from Player 2!"; //change this
            m_networkManager->sendData(message);
        }
    }
    else {
        qDebug() << "Disconnected from peer.";
    }
}

void Game::connectGameTimer() {
    connect(gameTimer, &QTimer::timeout, this, &Game::update);
    gameTimer->setInterval(1000 / FrameRate);
}

void Game::start() {
    loadMap();
    setFocusOnPlayer();
    gameTimer->start();
}

void Game::loadMap() {
    if (!mapLoader->loadMap(
            ":/assets/map.txt",
            m_gameView->scene(),
            m_gameView->view()->size(),
            31,
            13,
            this)) {
        qDebug() << "Map loading failed!";
    }
}

void Game::setFocusOnPlayer() {
    Player* focusPlayer = nullptr;
    for (const QPointer<Player>& player : players) {
        connectPlayerSignals(player);
        if (player && player->getPlayerId() == selectedPlayer) {
            focusPlayer = player;
        }
    }
    if (!focusPlayer && !players.isEmpty())
        focusPlayer = players.first();
    if (focusPlayer) {
        focusPlayer->setFlag(QGraphicsItem::ItemIsFocusable, true);
        focusPlayer->setFocus();
    }
}

void Game::connectPlayerSignals(QPointer<Player> player) {
    connect(player, &Player::playerDied, this, &Game::playerDied, Qt::UniqueConnection);
    connect(player, &Player::playerMoved, this, &Game::playerMoved, Qt::UniqueConnection);
    connect(player, &Player::playerPlacedBomb, this, &Game::playerPlacedBomb, Qt::UniqueConnection);
    // connect(player, &Player::playerDied, this, &Game::gameOver, Qt::UniqueConnection);
}

void Game::gameOver(int diedPlayerId) {
    int winnerId = (diedPlayerId == 1) ? 2 : 1;

    qDebug() << "Player " << winnerId << " is the winner!";
    QMessageBox::information(nullptr, "Game Over",
                             QString("Player %1 is the winner!").arg(winnerId));
    QTimer::singleShot(500, qApp, &QCoreApplication::quit);
}


void Game::playerDied(int playerId) {
    qDebug() << "Player " << playerId << " died.";
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerDied);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;

        m_networkManager->sendData(message);
        gameOver(playerId);
    }
}

void Game::playerMoved(int playerId, Qt::Key key, bool isPressed) {
    qDebug() << "Player " << playerId << " moved.";
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerMoved);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;
        message[messageFieldToString(MESSAGE_FIELD::Key)] = key;
        message[messageFieldToString(MESSAGE_FIELD::IsPressed)] = isPressed;

        m_networkManager->sendData(message);
    }
}

void Game::playerPlacedBomb(int playerId) {
    qDebug() << "Player " << playerId << " placed a bomb.";
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerPlacedBomb);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;

        m_networkManager->sendData(message);
    }
}

void Game::update() {
    if (!(m_gameView->scene()->focusItem())) {
        setFocusOnPlayer();
    }

    players.removeIf([](const QPointer<Player>& player) { return player.isNull(); });

    for (const auto& player : players) {
        if (player)
            player->updateMovement();
    }

    hud->updateHealth(players);
}

void Game::addPlayer(Player* player) {
    players.append(player);
}

QString Game::messageTypeToString(MESSAGE_TYPE type) {
    switch (type) {
    case MESSAGE_TYPE::PlayerDied:
        return "playerDied";
    case MESSAGE_TYPE::PlayerMoved:
        return "playerMoved";
    case MESSAGE_TYPE::PlayerPlacedBomb:
        return "playerPlacedBomb";
    case MESSAGE_TYPE::ConnectionStatus:
        return "connectionStatus";
    default:
        return "";
    }
}

Game::MESSAGE_TYPE Game::stringToMessageType(const QString& type) {
    if (type == "playerDied")
        return MESSAGE_TYPE::PlayerDied;
    if (type == "playerMoved")
        return MESSAGE_TYPE::PlayerMoved;
    if (type == "playerPlacedBomb")
        return MESSAGE_TYPE::PlayerPlacedBomb;
    if (type == "connectionStatus")
        return MESSAGE_TYPE::ConnectionStatus;
    return MESSAGE_TYPE::TypeError;
}

QString Game::messageFieldToString(MESSAGE_FIELD field) {
    switch (field) {
    case MESSAGE_FIELD::Type:
        return "type";
    case MESSAGE_FIELD::PlayerId:
        return "playerId";
    case MESSAGE_FIELD::Key:
        return "key";
    case MESSAGE_FIELD::IsPressed:
        return "isPressed";
    default:
        return "";
    }
}

Game::MESSAGE_FIELD Game::stringToMessageField(const QString& field) {
    if (field == "type")
        return MESSAGE_FIELD::Type;
    if (field == "playerId")
        return MESSAGE_FIELD::PlayerId;
    if (field == "key")
        return MESSAGE_FIELD::Key;
    if (field == "isPressed")
        return MESSAGE_FIELD::IsPressed;
    return MESSAGE_FIELD::FieldError;
}

QPointer<Player> Game::getPlayerById(int playerId) {
    for (const QPointer<Player>& player : players) {
        if (player && player->getPlayerId() == playerId) {
            return player;
        }
    }
    return nullptr;
}
