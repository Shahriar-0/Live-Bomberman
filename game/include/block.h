#ifndef BLOCK_H
#define BLOCK_H

#include <QPixmap>

#include "gameobject.h"

class Block : public GameObject {
public:
    explicit Block(QGraphicsItem* parent = nullptr);
};

#endif // BLOCK_H
