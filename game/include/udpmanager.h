#ifndef UDPMANAGER_H
#define UDPMANAGER_H

#include <QHostAddress>
#include <QThread>
#include <QUdpSocket>

#include "networkmanager.h"

class UDPManager : public NetworkManager {
    Q_OBJECT
public:
    explicit UDPManager(QObject* parent = nullptr);
    ~UDPManager();

    bool initialize(Role role, const QString& address, quint16 port) override;
    void sendData(const QJsonObject& data) override;
    void stop() override;

private slots:
    void onReadyRead();

private:
    QUdpSocket* m_socket;
    QHostAddress m_peerAddress;
    quint16 m_peerPort = 0;
};

#endif // UDPMANAGER_H
