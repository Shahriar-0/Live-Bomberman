#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#include "networkmanager.h"

class TCPManager : public NetworkManager {
    Q_OBJECT
public:
    explicit TCPManager(QObject* parent = nullptr);
    ~TCPManager();

    bool initialize(Role role, const QString& address, quint16 port) override;
    void sendData(const QJsonObject& data) override;
    void stop() override;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    bool setupServer();
    bool setupClient();

    QTcpServer* m_server;
    QTcpSocket* m_socket;
};

#endif // TCPMANAGER_H
