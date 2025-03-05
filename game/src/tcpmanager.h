#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include "networkmanager.h"

class TCPManager : public NetworkManager {
public:
    explicit TCPManager(QObject* parent = nullptr);
};

#endif // TCPMANAGER_H
