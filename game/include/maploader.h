#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QChar>
#include <QDebug>
#include <QFile>
#include <QGraphicsScene>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QTextStream>
#include <cmath>

#include "block.h"
#include "breakableblock.h"
#include "game.h"
#include "player.h"

class Block;
class BreakableBlock;
class Player;
class Game;

// clang-format off
namespace MapConstants {
static constexpr QChar BLOCK_CHAR      = QChar('B');
static constexpr QChar BREAKABLE_CHAR  = QChar('H');
static constexpr QChar PLAYER1_CHAR    = QChar('1');
static constexpr QChar PLAYER2_CHAR    = QChar('2');
static constexpr QChar IGNORE_CHAR     = QChar('X');
static constexpr qreal CELL_SCALE      = 0.5;
} // namespace MapConstants
// clang-format on

class MapLoader {
public:
    explicit MapLoader() = default;
    bool loadMap(const QString& filePath, QGraphicsScene* scene, const QSize& viewSize, int numCols, int numRows, Game* game);

private:
    void parseMapLine(const QString& line, int row, qreal cellSize, QGraphicsScene* scene, Game* game);
    Block* createBlock(qreal x, qreal y);
    BreakableBlock* createBreakableBlock(qreal x, qreal y);
    Player* createPlayer(int playerId);
};

#endif // MAPLOADER_H
