#ifndef UDPMANAGER_H
#define UDPMANAGER_H

#include <QThread>
#include <QUdpSocket>

#include "networkmanager.h"

class UDPManager : public NetworkManager {
    Q_OBJECT
public:
    explicit UDPManager(QObject* parent = nullptr);
    ~UDPManager();

    void initialize(Role role, const QString& address, quint16 port) override;
    void sendData(const QJsonObject& data) override;
    void stop() override;

private slots:
    void onReadyRead();

private:
    QUdpSocket* m_socket;
    QThread m_networkThread;
    QString m_address;
    quint16 m_port;
    Role m_role;
};

#endif // UDPMANAGER_H
