#include <QMessageBox>
#include <QTimer>
#include <QApplication>

#include "../include/game.h"

Game::Game(int selectedPlayer, const QString& protocol, QObject* parent)
    : QObject(parent),
      m_gameView(new GameView(this)),
      gameTimer(new QTimer(this)),
      mapLoader(new MapLoader()),
      m_gameNetworkManager(new GameNetworkManager(selectedPlayer, protocol, this)),
      protocol(protocol),
      selectedPlayer(selectedPlayer) {
    qDebug() << "Game started with selected player:" << selectedPlayer << "and protocol:" << protocol;
    m_gameView->initialize();

    QPixmap bgImage(":/assets/Background.png");
    QSize bgSize = bgImage.size();
    hud = new HUD(m_gameView->scene(), bgSize.width());

    connectNetworkSignals();
    m_gameNetworkManager->setup();

    connectGameTimer();
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
    connect(m_gameNetworkManager, &GameNetworkManager::playerDied, this, &Game::handlePlayerDied, Qt::UniqueConnection);
    connect(m_gameNetworkManager, &GameNetworkManager::playerMoved, this, &Game::handlePlayerMoved, Qt::UniqueConnection);
    connect(m_gameNetworkManager, &GameNetworkManager::playerPlacedBomb, this, &Game::handlePlayerPlacedBomb, Qt::UniqueConnection);
}

void Game::connectPlayerSignals(QPointer<Player> player) {
    connect(player, &Player::playerDied, m_gameNetworkManager, &GameNetworkManager::onPlayerDied, Qt::UniqueConnection);
    connect(player, &Player::playerDied, this, &Game::gameOver, Qt::UniqueConnection);
    connect(player, &Player::playerMoved, m_gameNetworkManager, &GameNetworkManager::onPlayerMoved, Qt::UniqueConnection);
    connect(player, &Player::playerPlacedBomb, m_gameNetworkManager, &GameNetworkManager::onPlayerPlacedBomb, Qt::UniqueConnection);
}

void Game::gameOver(int diedPlayerId) {
        int winnerId = (diedPlayerId == 1) ? 2 : 1;

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
