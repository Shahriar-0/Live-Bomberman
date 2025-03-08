#ifndef GAME_H
#define GAME_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QPointer>
#include <QTimer>

#include "gameview.h"
#include "hud.h"
#include "maploader.h"
#include "networkmanager.h"
#include "player.h"

class QTimer;
class MapLoader;
class GameView;
class HUD;

class Game : public QObject {
    Q_OBJECT
public:
    explicit Game(int selectedPlayer, const QString& protocol, QObject* parent = nullptr);
    void start();
    void addPlayer(Player* player);

public slots:
    void update();
    void onDataReceived(const QJsonObject& data);
    void onConnectionStatusChanged(bool connected);

private:
    void connectGameTimer();
    void loadMap();
    void setFocusOnPlayer();
    void setupNetwork();

    GameView* m_gameView;
    QTimer* gameTimer;
    MapLoader* mapLoader;
    HUD* hud;
    NetworkManager* m_networkManager;

    int selectedPlayer;
    QString protocol;
    QList<QPointer<Player>> players;
    static constexpr int FrameRate = 30;
};

#endif // GAME_H
