#include "../include/gameNetworkManager.h"
#include "../include/tcpmanager.h"
#include "../include/udpmanager.h"
#include <QDebug>

GameNetworkManager::GameNetworkManager(int selectedPlayer, const QString& protocol, QObject* parent)
    : QObject(parent), m_networkManager(nullptr), protocol(protocol), selectedPlayer(selectedPlayer) {}

void GameNetworkManager::setup() {
    quint16 port = 12345;
    QString address = "127.0.0.1";
    NetworkManager::Role role = NetworkManager::Server;

    if (protocol == "TCP") {
        m_networkManager = new TCPManager(this);
    } else if (protocol == "UDP") {
        m_networkManager = new UDPManager(this);
    }

    if (!m_networkManager->initialize(role, address, port)) {
        qDebug() << "Server initialization failed. Joining as a client.";
        role = NetworkManager::Client;
        if (!m_networkManager->initialize(role, address, port)) {
            qDebug() << "Client initialization failed as well!";
            return;
        }
    }

    connectNetworkSignals();

    if (protocol == "UDP" && role == NetworkManager::Client) {
        emit m_networkManager->connectionStatusChanged(true);
    }
}

void GameNetworkManager::connectNetworkSignals() {
    connect(m_networkManager, &NetworkManager::connectionStatusChanged, this, &GameNetworkManager::onConnectionStatusChanged);
    connect(m_networkManager, &NetworkManager::dataReceived, this, &GameNetworkManager::onDataReceived);
    connect(m_networkManager, &NetworkManager::errorOccurred, this, &GameNetworkManager::onErrorOccurred);
}

void GameNetworkManager::onPlayerDied(int playerId) {
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerDied);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;

        m_networkManager->sendData(message);
    }
}

void GameNetworkManager::onPlayerMoved(int playerId, Qt::Key key, bool isPressed) {
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerMoved);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;
        message[messageFieldToString(MESSAGE_FIELD::Key)] = key;
        message[messageFieldToString(MESSAGE_FIELD::IsPressed)] = isPressed;

        m_networkManager->sendData(message);
    }
}

void GameNetworkManager::onPlayerPlacedBomb(int playerId) {
    if (playerId == selectedPlayer) {
        QJsonObject message;
        message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerPlacedBomb);
        message[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;
        message[messageFieldToString(MESSAGE_FIELD::SequenceNumber)] = bombSequenceNumber++;

        for (int i = 0; i < 3; i++) {
            m_networkManager->sendData(message);
        }
    }
}

void GameNetworkManager::sendUpdatedPlayerState(int playerId, qreal x, qreal y, int health) {
    QJsonObject stateUpdate;
    stateUpdate[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::PlayerStateUpdate);
    stateUpdate[messageFieldToString(MESSAGE_FIELD::SequenceNumber)] = updateSequenceNumber++;
    stateUpdate[messageFieldToString(MESSAGE_FIELD::PlayerId)] = playerId;
    stateUpdate[messageFieldToString(MESSAGE_FIELD::X)] = x;
    stateUpdate[messageFieldToString(MESSAGE_FIELD::Y)] = y;
    stateUpdate[messageFieldToString(MESSAGE_FIELD::Health)] = health;

    emit stateUpdateReceived(updateSequenceNumber);
    m_networkManager->sendData(stateUpdate);
}

void GameNetworkManager::onDataReceived(const QJsonObject& data) {
    QString type = data[messageFieldToString(MESSAGE_FIELD::Type)].toString();
    MESSAGE_TYPE messageType = stringToMessageType(type);

    int seqNum = data[messageFieldToString(MESSAGE_FIELD::SequenceNumber)].toInt();

    if (messageType == MESSAGE_TYPE::PlayerPlacedBomb) {
        if (receivedBombSequenceNumbers.contains(seqNum)) {
            qDebug() << "Duplicate bomb placement message received. Ignoring.";
            return;
        }
        receivedBombSequenceNumbers.insert(seqNum);
    }

    switch (messageType) {
    case MESSAGE_TYPE::PlayerDied:
        emit playerDied(data[messageFieldToString(MESSAGE_FIELD::PlayerId)].toInt());
        break;
        
    case MESSAGE_TYPE::PlayerMoved:
        emit playerMoved(data[messageFieldToString(MESSAGE_FIELD::PlayerId)].toInt(),
                         data[messageFieldToString(MESSAGE_FIELD::Key)].toInt(),
                         data[messageFieldToString(MESSAGE_FIELD::IsPressed)].toBool());
        break;

    case MESSAGE_TYPE::PlayerPlacedBomb:
        emit playerPlacedBomb(data[messageFieldToString(MESSAGE_FIELD::PlayerId)].toInt());
        break;

    case MESSAGE_TYPE::PlayerStateUpdate:
        if (seqNum > lastReceivedSequenceNumber) {
            lastReceivedSequenceNumber = seqNum;
            emit playerStateUpdated(
                data[messageFieldToString(MESSAGE_FIELD::PlayerId)].toInt(),
                data[messageFieldToString(MESSAGE_FIELD::X)].toDouble(),
                data[messageFieldToString(MESSAGE_FIELD::Y)].toDouble(),
                data[messageFieldToString(MESSAGE_FIELD::Health)].toInt()
            );
            emit stateUpdateReceived(seqNum);
        } else {
            qDebug() << "Ignored outdated update. Current seq:" << lastReceivedSequenceNumber;
        }
        break;

    default:
        break;
    }
}

void GameNetworkManager::onConnectionStatusChanged(bool connected) {
    qDebug() << "Connection status changed:" << connected;
    if (connected) {
        qDebug() << "Connected to peer.";
        if (m_networkManager->role() == NetworkManager::Client) {
            QJsonObject message;
            message[messageFieldToString(MESSAGE_FIELD::Type)] = messageTypeToString(MESSAGE_TYPE::ConnectionStatus);
            m_networkManager->sendData(message);
        }
    } else {
        qDebug() << "Disconnected from peer.";
    }
}

void GameNetworkManager::onErrorOccurred(const QString& message) {
    qDebug() << "Error occurred:" << message;
}

QString GameNetworkManager::messageTypeToString(MESSAGE_TYPE type) {
    switch (type) {
    case MESSAGE_TYPE::PlayerDied:
        return "playerDied";
    case MESSAGE_TYPE::PlayerMoved:
        return "playerMoved";
    case MESSAGE_TYPE::PlayerPlacedBomb:
        return "playerPlacedBomb";
    case MESSAGE_TYPE::ConnectionStatus:
        return "connectionStatus";
    case MESSAGE_TYPE::PlayerStateUpdate:
        return "playerStateUpdate";
    default:
        return "";
    }
}

GameNetworkManager::MESSAGE_TYPE GameNetworkManager::stringToMessageType(const QString& type) {
    if (type == "playerDied")
        return MESSAGE_TYPE::PlayerDied;
    if (type == "playerMoved")
        return MESSAGE_TYPE::PlayerMoved;
    if (type == "playerPlacedBomb")
        return MESSAGE_TYPE::PlayerPlacedBomb;
    if (type == "connectionStatus")
        return MESSAGE_TYPE::ConnectionStatus;
    if (type == "playerStateUpdate")
        return MESSAGE_TYPE::PlayerStateUpdate;
    return MESSAGE_TYPE::TypeError;
}

QString GameNetworkManager::messageFieldToString(MESSAGE_FIELD field) {
    switch (field) {
    case MESSAGE_FIELD::Type:
        return "type";
    case MESSAGE_FIELD::PlayerId:
        return "playerId";
    case MESSAGE_FIELD::Key:
        return "key";
    case MESSAGE_FIELD::IsPressed:
        return "isPressed";
    case MESSAGE_FIELD::SequenceNumber:
        return "sequenceNumber";
    case MESSAGE_FIELD::X:
        return "x";
    case MESSAGE_FIELD::Y:
        return "y";
    case MESSAGE_FIELD::Health:
        return "health";
    default:
        return "";
    }
}

GameNetworkManager::MESSAGE_FIELD GameNetworkManager::stringToMessageField(const QString& field) {
    if (field == "type")
        return MESSAGE_FIELD::Type;
    if (field == "playerId")
        return MESSAGE_FIELD::PlayerId;
    if (field == "key")
        return MESSAGE_FIELD::Key;
    if (field == "isPressed")
        return MESSAGE_FIELD::IsPressed;
    if (field == "sequenceNumber")
        return MESSAGE_FIELD::SequenceNumber;
    if (field == "x")
        return MESSAGE_FIELD::X;
    if (field == "y")
        return MESSAGE_FIELD::Y;
    if (field == "health")
        return MESSAGE_FIELD::Health;
    return MESSAGE_FIELD::FieldError;
}
