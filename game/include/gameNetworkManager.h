#ifndef GAMENETWORKMANAGER_H
#define GAMENETWORKMANAGER_H

#include <QDebug>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QJsonObject>

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
    void playerStateUpdated(int playerId, qreal x, qreal y, int health);  // New signal
    void stateUpdateReceived(int sequenceNumber);                        // Signal for state update received

public slots:
    void onPlayerDied(int playerId);
    void onPlayerMoved(int playerId, Qt::Key key, bool isPressed);
    void onPlayerPlacedBomb(int playerId);

private slots:
    void onDataReceived(const QJsonObject& data);
    void onConnectionStatusChanged(bool connected);
    void onErrorOccurred(const QString& message);
    void sendPlayerStateUpdate();  // Send player state updates

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

    QTimer* updateTimer;                // Timer for periodic updates
    int updateSequenceNumber = 0;       // Outgoing sequence number
};

#endif // GAMENETWORKMANAGER_H
