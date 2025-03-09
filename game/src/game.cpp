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
    NetworkManager::Role role = (selectedPlayer == 1) ? NetworkManager::Server : NetworkManager::Client;
    QString address = (role == NetworkManager::Server) ? QString() : "127.0.0.1";
    quint16 port = 12345;

    if (protocol == "TCP")
        m_networkManager = new TCPManager(this);
    else if (protocol == "UDP")
        m_networkManager = new UDPManager(this);

    m_networkManager->initialize(role, address, port);
    connect(m_networkManager, &NetworkManager::connectionStatusChanged, this, &Game::onConnectionStatusChanged);
    connect(m_networkManager, &NetworkManager::dataReceived, this, &Game::onDataReceived);
    connect(m_networkManager, &NetworkManager::errorOccurred, this, &Game::errorOccurred);
}

void Game::onDataReceived(const QJsonObject& data) {
    qDebug() << "Data received:" << data;
    // TODO: handle received data (died, moved, placed bomb, etc.)
}

void Game::errorOccurred(const QString& message) {
    qDebug() << "Error occurred:" << message;
}

void Game::onConnectionStatusChanged(bool connected) {
    if (connected) {
        qDebug() << "Connected to peer.";
        if (m_networkManager->role() == NetworkManager::Client) {
            QJsonObject message;
            message["type"] = "test";
            message["content"] = "Hello from Player 2!";
            m_networkManager->sendData(message);
            // m_networkManager->sendData(message);
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
    connect(player, &Player::playerDied, this, &Game::playerDied);
    connect(player, &Player::playerMoved, this, &Game::playerMoved);
    connect(player, &Player::playerPlacedBomb, this, &Game::playerPlacedBomb);
}

void Game::playerDied(int playerId) {
    qDebug() << "Player " << playerId << " died.";
    QJsonObject message;
    message["type"] = "playerDied";
    message["content"] = playerId;
    m_networkManager->sendData(message);
    // modify json structure if needed
}

void Game::playerMoved(int playerId, Qt::Key key) {
    qDebug() << "Player " << playerId << " moved.";
    QJsonObject message;
    message["type"] = "playerMoved";
    message["content"] = playerId;
    message["key"] = key;
    m_networkManager->sendData(message);
    // modify json structure if needed
}

void Game::playerPlacedBomb(int playerId) {
    qDebug() << "Player " << playerId << " placed a bomb.";
    QJsonObject message;
    message["type"] = "playerPlacedBomb";
    message["content"] = playerId;
    m_networkManager->sendData(message);
    // modify json structure if needed
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