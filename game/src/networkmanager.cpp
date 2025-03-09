#include "../include/networkmanager.h"

NetworkManager::NetworkManager(QObject* parent)
    : QObject{parent}, m_port(0) {}

