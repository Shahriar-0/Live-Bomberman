#include "../include/udpmanager.h"

UDPManager::UDPManager(QObject* parent) : NetworkManager(parent),
                                          m_socket(nullptr) {
    // moveToThread(&m_networkThread);
    // m_networkThread.start();
}

UDPManager::~UDPManager() {
    stop();
    // m_networkThread.quit();
    // m_networkThread.wait();
}

void UDPManager::initialize(Role role, const QString& address, quint16 port) {
    m_role = role;
    m_address = address;
    m_port = port;

    QMetaObject::invokeMethod(this, [this]() {
        m_socket = new QUdpSocket(this);
        connect(m_socket, &QUdpSocket::readyRead, this, &UDPManager::onReadyRead);
        connect(m_socket, &QUdpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) { emit errorOccurred(m_socket->errorString()); });

        if (m_role == Server) {
            if (!m_socket->bind(QHostAddress::Any, m_port))
                emit errorOccurred(m_socket->errorString());

            qDebug() << "UDP Server bound to port" << m_port;
        }
        else {
            // Client binds to any available port to receive responses
            if (!m_socket->bind())
                emit errorOccurred(m_socket->errorString());

            qDebug() << "UDP Client bound to port" << m_socket->localPort();
        }
    });

    qDebug() << "UDPManager initialized";
}

void UDPManager::onReadyRead() {
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        if (m_role == Server) {
            m_peerAddress = sender;
            m_peerPort = senderPort;
        }

        QJsonDocument doc = QJsonDocument::fromJson(datagram);
        if (!doc.isNull() && doc.isObject())
            emit dataReceived(doc.object());
    }
}

void UDPManager::sendData(const QJsonObject& data) {
    QMetaObject::invokeMethod(this, [this, data]() {
        QJsonDocument doc(data);
        QByteArray datagram = doc.toJson();

        QHostAddress targetAddress;
        quint16 targetPort;

        if (m_role == Server) {
            // Send to last known client address
            targetAddress = m_peerAddress;
            targetPort = m_peerPort;
        }
        else {
            // Client sends to server's address
            targetAddress = QHostAddress(m_address);
            targetPort = m_port;
        }

        if (!targetAddress.isNull() && targetPort != 0) {
            m_socket->writeDatagram(datagram, targetAddress, targetPort);
            qDebug() << "UDP Data sent to" << targetAddress << ":" << datagram;
        }
    });
}

void UDPManager::stop() {
    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}