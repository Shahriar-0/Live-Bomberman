#include "../include/udpmanager.h"

UDPManager::UDPManager(QObject* parent) : NetworkManager(parent),
                                          m_socket(nullptr),
                                          m_port(0) {
    connect(m_socket, &QUdpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
        emit errorOccurred(m_socket->errorString());
    });
    moveToThread(&m_networkThread);
    m_networkThread.start();
}

UDPManager::~UDPManager() {
    stop();
    m_networkThread.quit();
    m_networkThread.wait();
}

void UDPManager::initialize(Role role, const QString& address, quint16 port) {
    m_role = role;
    m_address = address;
    m_port = port;

    QMetaObject::invokeMethod(this, [this]() {
        m_socket = new QUdpSocket(this);
        connect(m_socket, &QUdpSocket::readyRead, this, &UDPManager::onReadyRead);

        if (m_role == Server) {
            if (m_socket->bind(QHostAddress::Any, m_port)) {
                emit connectionStatusChanged(true);
            }
            else {
                emit errorOccurred(m_socket->errorString());
            }
        }
        else {
            emit connectionStatusChanged(true);
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
        qDebug() << "Data received (in UDP): " << datagram;

        QJsonDocument doc = QJsonDocument::fromJson(datagram);
        if (!doc.isNull() && doc.isObject()) {
            emit dataReceived(doc.object());
        }
    }
}

void UDPManager::sendData(const QJsonObject& data) {
    QMetaObject::invokeMethod(this, [this, data]() {
        QJsonDocument doc(data);
        QByteArray datagram = doc.toJson();

        QHostAddress address = m_role == Server ? QHostAddress::Broadcast : QHostAddress(m_address);
        m_socket->writeDatagram(datagram, address, m_port);
        qDebug() << "Data sent: " << datagram;
    });
}

void UDPManager::stop() {
    if (m_socket) m_socket->close();
}
