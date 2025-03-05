#ifndef BREAKABLEBLOCK_H
#define BREAKABLEBLOCK_H

#include <QDebug>
#include <QGraphicsScene>
#include <QPixmap>
#include <QTimer>

#include "animation.h"
#include "block.h"

class BreakableBlock : public Block {
    Q_OBJECT

public:
    explicit BreakableBlock(QGraphicsItem* parent = nullptr);

public slots:
    void destroyBlock();

private:
    void setupBlock();
    void scheduleBlockRemoval(int delayMs);

    static constexpr int FrameCount = 3;
    static constexpr int FrameDurationMs = 200;
};

#endif // BREAKABLEBLOCK_H
