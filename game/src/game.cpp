#include "../include/game.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

Game::Game(int selectedPlayer, const QString& protocol, QObject* parent)
    : QObject(parent),
      m_gameView(new GameView(this)),
      gameTimer(new QTimer(this)),
      mapLoader(new MapLoader()),
      m_networkThread(new QThread(this)),
      m_gameNetworkManager(new GameNetworkManager(selectedPlayer, protocol, nullptr)),
      protocol(protocol),
      selectedPlayer(selectedPlayer),
      stateUpdateTimer(new QTimer(this))
{
    qDebug() << "Game started with selected player:" << selectedPlayer << "and protocol:" << protocol;
    m_gameView->initialize();

    QPixmap bgImage(":/assets/Background.png");
    QSize bgSize = bgImage.size();
    hud = new HUD(m_gameView->scene(), bgSize.width());

    m_gameNetworkManager->moveToThread(m_networkThread);
    connect(m_networkThread, &QThread::finished, m_gameNetworkManager, &GameNetworkManager::deleteLater);
    m_networkThread->start();
    connectNetworkSignals();
    QMetaObject::invokeMethod(m_gameNetworkManager, "setup", Qt::QueuedConnection);

    connectGameTimer();
    connect(stateUpdateTimer, &QTimer::timeout, this, &Game::emitPlayerState);
    stateUpdateTimer->start(250);
}

Game::~Game() {
    m_networkThread->quit();
    m_networkThread->wait();
}

void Game::start() {
    loadMap();
    setFocusOnPlayer();
    gameTimer->start();
}

void Game::addPlayer(Player* player) {
    players.append(player);
}

void Game::connectGameTimer() {
    connect(gameTimer, &QTimer::timeout, this, &Game::update);
    gameTimer->setInterval(1000 / FrameRate);
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

void Game::connectNetworkSignals() {
    connect(m_gameNetworkManager, &GameNetworkManager::playerDied, this, &Game::handlePlayerDied);
    connect(m_gameNetworkManager, &GameNetworkManager::playerMoved, this, &Game::handlePlayerMoved);
    connect(m_gameNetworkManager, &GameNetworkManager::playerPlacedBomb, this, &Game::handlePlayerPlacedBomb);
    connect(m_gameNetworkManager, &GameNetworkManager::playerStateUpdated, this, &Game::updatePlayerState);
    connect(m_gameNetworkManager, &GameNetworkManager::stateUpdateReceived, this, &Game::handleStateUpdateReceived);

    connect(this, &Game::playerStateReady, m_gameNetworkManager, &GameNetworkManager::sendUpdatedPlayerState);
}

void Game::connectPlayerSignals(QPointer<Player> player) {
    connect(player, &Player::playerDied, m_gameNetworkManager, &GameNetworkManager::onPlayerDied, Qt::UniqueConnection);
    connect(player, &Player::playerDied, this, &Game::gameOver, Qt::UniqueConnection);
    connect(player, &Player::playerMoved, m_gameNetworkManager, &GameNetworkManager::onPlayerMoved, Qt::UniqueConnection);
    connect(player, &Player::playerPlacedBomb, m_gameNetworkManager, &GameNetworkManager::onPlayerPlacedBomb, Qt::UniqueConnection);
}

void Game::gameOver(int diedPlayerId) {
    int winnerId = (diedPlayerId == 1) ? 2 : 1; // or 3 - diedPlayerId just because i respect clean coding

    qDebug() << "Player " << winnerId << " is the winner!";
    QMessageBox::information(nullptr, "Game Over",
                             QString("Player %1 is the winner!").arg(winnerId));
    QTimer::singleShot(500, qApp, &QCoreApplication::quit);
}

QPointer<Player> Game::getPlayerById(int playerId) {
    for (const QPointer<Player>& player : players) {
        if (player && player->getPlayerId() == playerId) {
            return player;
        }
    }
    return nullptr;
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

void Game::handlePlayerDied(int playerId) {
    qDebug() << "Player " << playerId << " died.";
    QPointer<Player> player = getPlayerById(playerId);
    if (player) {
        player->die();
    }
}

void Game::handlePlayerMoved(int playerId, int key, bool isPressed) {
    qDebug() << "Player " << playerId << " moved.";
    QPointer<Player> player = getPlayerById(playerId);
    if (player) {
        player->updateDirectionState(key, isPressed);
    }
}

void Game::handlePlayerPlacedBomb(int playerId) {
    qDebug() << "Player " << playerId << " placed a bomb.";
    QPointer<Player> player = getPlayerById(playerId);
    if (player) {
        player->placeBomb();
    }
}

void Game::updatePlayerState(int playerId, qreal x, qreal y, int health) {
    QPointer<Player> player = getPlayerById(playerId);
    if (player) {
        player->setPos(x, y);
        player->setHealth(health);
    }
}

void Game::handleStateUpdateReceived(int sequenceNumber) {
    if (sequenceNumber > lastReceivedSequenceNumber) {
        lastReceivedSequenceNumber = sequenceNumber;
        qDebug() << "Updated to sequence number:" << sequenceNumber;
    }
    else {
        qDebug() << "Ignored outdated update. Current seq:" << lastReceivedSequenceNumber;
    }
}

void Game::emitPlayerState() {
    QPointer<Player> player = getPlayerById(selectedPlayer);
    if (player) {
        qreal x = player->pos().x();
        qreal y = player->pos().y();
        int health = player->getHealth();
        emit playerStateReady(selectedPlayer, x, y, health);
    }
}
