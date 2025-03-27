#ifndef GAMENETWORKMANAGER_H
#define GAMENETWORKMANAGER_H

#include <QDebug>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QPointer>
#include <QTimer>

#include "networkmanager.h"


class GameNetworkManager : public QObject {
    Q_OBJECT
public:
    explicit GameNetworkManager(int selectedPlayer, const QString& protocol, QObject* parent = nullptr);
    void setup();

signals:
    void playerDied(int playerId);
    void playerMoved(int playerId, int key, bool isPressed);
    void playerPlacedBomb(int playerId);

public slots:
    void onPlayerDied(int playerId);
    void onPlayerMoved(int playerId, Qt::Key key, bool isPressed);
    void onPlayerPlacedBomb(int playerId);

private slots:
    void onDataReceived(const QJsonObject& data);
    void onConnectionStatusChanged(bool connected);
    void onErrorOccurred(const QString& message);

private:
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

    NetworkManager* m_networkManager;
    QString protocol;
    int selectedPlayer;
};

#endif // GAMENETWORKMANAGER_H
