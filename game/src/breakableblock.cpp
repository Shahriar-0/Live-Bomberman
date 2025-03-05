#include "../include/breakableblock.h"

BreakableBlock::BreakableBlock(QGraphicsItem* parent)
    : Block(parent) {
    setupBlock();
}

void BreakableBlock::setupBlock() {
    setPixmap(QPixmap(":/assets/breakable_block.png"));
    QList<QPixmap> breakFrames = Animation::loadFrames(":/assets/breakable_block%1.png", FrameCount);
    Animation* breakAnimation = new Animation("break", breakFrames, FrameDurationMs, this);
    addAnimation(breakAnimation);
}

void BreakableBlock::destroyBlock() {
    playAnimation("break");
    const int totalDurationMs = FrameCount * FrameDurationMs;
    scheduleBlockRemoval(totalDurationMs);
}

void BreakableBlock::scheduleBlockRemoval(int delayMs) {
    QTimer::singleShot(delayMs, this, [this]() {
        if (scene()) {
            scene()->removeItem(this);
        }
        delete this;
    });
}
