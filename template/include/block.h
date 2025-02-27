#ifndef BLOCK_H
#define BLOCK_H

#include "gameobject.h"

#include <QPixmap>

class Block : public GameObject
{
public:
    explicit Block(QGraphicsItem *parent = nullptr);
};

#endif // BLOCK_H
