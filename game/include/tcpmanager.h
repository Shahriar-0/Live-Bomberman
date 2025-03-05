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

    void initialize(Role role, const QString& address, quint16 port) override;
    void sendData(const QJsonObject& data) override;
    void stop() override;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

private:
    void setupServer();
    void setupClient();

    QTcpServer* m_server;
    QTcpSocket* m_socket;
    QThread m_networkThread;
    QString m_address;
    quint16 m_port;
    Role m_role;
};

#endif // TCPMANAGER_H
