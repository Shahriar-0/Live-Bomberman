#ifndef GAMENETWORKMANAGER_H
#define GAMENETWORKMANAGER_H

#include <QDebug>
#include <QObject>
#include <QPointer>
#include <QJsonObject>
#include <QSet>

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
    void playerStateUpdated(int playerId, qreal x, qreal y, int health);
    void stateUpdateReceived(int sequenceNumber);

public slots:
    void onPlayerDied(int playerId);
    void onPlayerMoved(int playerId, Qt::Key key, bool isPressed);
    void onPlayerPlacedBomb(int playerId);
    void sendUpdatedPlayerState(int playerId, qreal x, qreal y, int health);

private slots:
    void onDataReceived(const QJsonObject& data);
    void onConnectionStatusChanged(bool connected);
    void onErrorOccurred(const QString& message);

private:
    void connectNetworkSignals();

    enum MESSAGE_TYPE {
        PlayerMoved,
        PlayerDied,
        PlayerPlacedBomb,
        ConnectionStatus,
        PlayerStateUpdate,
        TypeError
    };

    enum MESSAGE_FIELD {
        PlayerId,
        Key,
        IsPressed,
        Type,
        SequenceNumber,
        X,
        Y,
        Health,
        FieldError
    };

    QString messageTypeToString(MESSAGE_TYPE type);
    QString messageFieldToString(MESSAGE_FIELD field);
    MESSAGE_TYPE stringToMessageType(const QString& type);
    MESSAGE_FIELD stringToMessageField(const QString& field);

    NetworkManager* m_networkManager;
    QString protocol;
    int selectedPlayer;

    int updateSequenceNumber = 0;
    int bombSequenceNumber = 0;
    
    QSet<int> receivedBombSequenceNumbers;
};

#endif // GAMENETWORKMANAGER_H
