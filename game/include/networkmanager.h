#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    enum Protocol { TCP,
                    UDP };
    Q_ENUM(Protocol)

    enum Role { Server,
                Client };
    Q_ENUM(Role)

    virtual ~NetworkManager() = default;

    virtual void initialize(Role role, const QString& address, quint16 port) = 0;
    virtual void sendData(const QJsonObject& data) = 0;
    virtual void stop() = 0;
    virtual Role role() = 0;

signals:
    void dataReceived(const QJsonObject& data);
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString& message);

protected:
    explicit NetworkManager(QObject* parent = nullptr);
};

#endif // NETWORKMANAGER_H
