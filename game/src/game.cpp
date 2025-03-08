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

    if (protocol == "TCP") {
        m_networkManager = new TCPManager(this);
        m_networkManager->initialize(role, address, port);
        qDebug() << "TCPManager initialized";
        connect(m_networkManager, &TCPManager::dataReceived, this, &Game::onDataReceived);
        connect(m_networkManager, &TCPManager::connectionStatusChanged, this, &Game::onConnectionStatusChanged);
    }
    else if (protocol == "UDP") { // TODO
        m_networkManager = new UDPManager(this);
        m_networkManager->initialize(role, address, port);
        qDebug() << "UDPManager initialized";
        connect(m_networkManager, &UDPManager::dataReceived, this, &Game::onDataReceived);
        connect(m_networkManager, &UDPManager::connectionStatusChanged, this, &Game::onConnectionStatusChanged);
    }
}

void Game::onDataReceived(const QJsonObject& data) {
    qDebug() << "Data received:" << data;
}

void Game::onConnectionStatusChanged(bool connected) {
    if (connected) {
        qDebug() << "Connected to peer.";
        if (m_networkManager->role() == NetworkManager::Client) {
            QJsonObject message;
            message["type"] = "test";
            message["content"] = "Hello from Player 2!";
            m_networkManager->sendData(message);
        }
    } else {
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
        if (player && player->getPlayerId() == selectedPlayer) {
            focusPlayer = player;
            break;
        }
    }
    if (!focusPlayer && !players.isEmpty())
        focusPlayer = players.first();
    if (focusPlayer) {
        focusPlayer->setFlag(QGraphicsItem::ItemIsFocusable, true);
        focusPlayer->setFocus();
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