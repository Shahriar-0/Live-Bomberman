#ifndef UDPMANAGER_H
#define UDPMANAGER_H

#include "networkmanager.h"

class UDPManager : public NetworkManager {
public:
    explicit UDPManager(QObject* parent = nullptr);
};

#endif // UDPMANAGER_H
