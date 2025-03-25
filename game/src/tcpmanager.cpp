#include "../include/tcpmanager.h"

TCPManager::TCPManager(QObject* parent) : NetworkManager(parent),
                                          m_server(nullptr),
                                          m_socket(nullptr) {
    // moveToThread(&m_networkThread);
    // m_networkThread.start();
}

TCPManager::~TCPManager() {
    stop();
    // m_networkThread.quit();
    // m_networkThread.wait();
}

void TCPManager::initialize(Role role, const QString& address, quint16 port) {
    m_role = role;
    m_address = address;
    m_port = port;

    QMetaObject::invokeMethod(this, [this]() {
        if (m_role == Server) setupServer();
        else setupClient();
    });

    qDebug() << "TCPManager initialized";
}

void TCPManager::setupServer() {
    m_server = new QTcpServer(this);
    qDebug() << "Server created";
    connect(m_server, &QTcpServer::newConnection, this, &TCPManager::onNewConnection);

    if (!m_server->listen(QHostAddress::Any, m_port)) {
        emit errorOccurred(m_server->errorString());
    }
    qDebug() << "Server started";
}

void TCPManager::setupClient() {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        emit connectionStatusChanged(true);
    });
    connect(m_socket, &QTcpSocket::readyRead, this, &TCPManager::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &TCPManager::onDisconnected);

    m_socket->connectToHost(m_address, m_port);
}

void TCPManager::onNewConnection() {
    if (m_role == Server && !m_socket) {
        m_socket = m_server->nextPendingConnection();
        connect(m_socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
            emit errorOccurred(m_socket->errorString());
        });
        connect(m_socket, &QTcpSocket::readyRead, this, &TCPManager::onReadyRead);
        connect(m_socket, &QTcpSocket::disconnected, this, &TCPManager::onDisconnected);
        emit connectionStatusChanged(true);
        qDebug() << "Client connected";
    }
}

void TCPManager::onReadyRead() {
    while (m_socket->bytesAvailable() > 0) {
        QByteArray data = m_socket->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (!doc.isNull() && doc.isObject()) {
            emit dataReceived(doc.object());
            // qDebug() << "Data received (in TCP): " << data;
        }
    }
}

void TCPManager::onDisconnected() {
    emit connectionStatusChanged(false);
    if (m_role == Server) {
        m_socket->deleteLater();
        m_socket = nullptr;
        qDebug() << "Server disconnected";
    }
}

void TCPManager::sendData(const QJsonObject& data) {
    QMetaObject::invokeMethod(this, [this, data]() {
        if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
            QJsonDocument doc(data);
            m_socket->write(doc.toJson());
            m_socket->flush();
            qDebug() << "TCP Data sent: " << doc.toJson();
        }
    });
}

void TCPManager::stop() {
    if (m_server) m_server->close();
    if (m_socket) m_socket->disconnectFromHost();
}
