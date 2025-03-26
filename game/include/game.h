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
    void playerDied(int playerId);
    void playerMoved(int playerId, Qt::Key key, bool isPressed);
    void playerPlacedBomb(int playerId);
    void errorOccurred(const QString& message);

private:
    void connectGameTimer();
    void loadMap();
    void setFocusOnPlayer();
    void setupNetwork();
    void connectPlayerSignals(QPointer<Player> player);
    void gameOver(int diedPlayerId);

    GameView* m_gameView;
    QTimer* gameTimer;
    MapLoader* mapLoader;
    HUD* hud;
    NetworkManager* m_networkManager;

    int selectedPlayer;
    QString protocol;
    QList<QPointer<Player>> players;
    static constexpr int FrameRate = 30;

    enum MESSAGE_TYPE {
        PlayerMoved,
        PlayerDied,
        PlayerPlacedBomb,
        ConnectionStatus,
        TypeError
    };

    enum MESSAGE_FIELD {
        PlayerId,
        Key,
        IsPressed,
        Type,
        FieldError
    };

    QString messageTypeToString(MESSAGE_TYPE type);
    QString messageFieldToString(MESSAGE_FIELD field);
    MESSAGE_TYPE stringToMessageType(const QString& type);
    MESSAGE_FIELD stringToMessageField(const QString& field);
    QPointer<Player> getPlayerById(int playerId);
};

#endif // GAME_H
