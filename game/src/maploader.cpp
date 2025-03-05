#include "../include/maploader.h"

bool MapLoader::loadMap(const QString& filePath, QGraphicsScene* scene,
                        const QSize& viewSize, int numCols, int numRows, Game* game) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Could not open map file:" << filePath;
        return false;
    }

    const qreal cellSize = qMin(
        viewSize.width() / static_cast<qreal>(numCols),
        viewSize.height() / static_cast<qreal>(numRows));

    QTextStream in(&file);
    int row = 0;
    while (!in.atEnd()) {
        parseMapLine(in.readLine(), row, cellSize, scene, game);
        ++row;
    }
    file.close();
    return true;
}

void MapLoader::parseMapLine(const QString& line, int row, qreal cellSize, QGraphicsScene* scene, Game* game) {
    int col = 0;
    for (const QChar& symbol : line) {
        const qreal x = col * cellSize * MapConstants::CELL_SCALE;
        const qreal y = row * cellSize * MapConstants::CELL_SCALE;
        switch (symbol.unicode()) {
        case MapConstants::BLOCK_CHAR.unicode():
            scene->addItem(createBlock(x, y));
            break;
        case MapConstants::BREAKABLE_CHAR.unicode():
            scene->addItem(createBreakableBlock(x, y));
            break;
        case MapConstants::PLAYER1_CHAR.unicode():
        case MapConstants::PLAYER2_CHAR.unicode(): {
            Player* player = createPlayer(symbol.digitValue());
            player->setPos(x, y);
            scene->addItem(player);
            if (game)
                game->addPlayer(player);
            break;
        }
        case MapConstants::IGNORE_CHAR.unicode():
            break;
        default:
            qDebug() << "Warning: Unknown symbol:" << symbol << "at row:" << row << "col:" << col;
        }
        ++col;
    }
}

Block* MapLoader::createBlock(qreal x, qreal y) {
    Block* block = new Block();
    block->setPos(x, y);
    return block;
}

BreakableBlock* MapLoader::createBreakableBlock(qreal x, qreal y) {
    BreakableBlock* breakableBlock = new BreakableBlock();
    breakableBlock->setPos(x, y);
    return breakableBlock;
}

Player* MapLoader::createPlayer(int playerId) {
    static const QPixmap idleSprite(":/assets/player_down2.png");
    return new Player(idleSprite, playerId);
}
