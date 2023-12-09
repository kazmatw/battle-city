#include "board.h"
#include "abstractmaploader.h"
#include "staticblock.h"
#include "tank.h"

#include <QTimer>

namespace Tanks {

// 地圖縮放因子，用於更細分的塊管理
#define MAP_SCALE_FACTOR 2

// Board 類的構造函數
Board::Board(QObject *parent) : QObject(parent) { }

// 加載地圖的函數
bool Board::loadMap(AbstractMapLoader *loader)
{
    if (!loader->open()) {
        return false;
    }

           // 初始化棋盤尺寸和地圖
    _size = loader->dimensions() * MAP_SCALE_FACTOR;
    _size = _size.boundedTo(QSize(1024, 1024));
    QRect boardRect(QPoint(0, 0), _size);
    _map.resize(_size.width() * _size.height());
    _map.fill(0);

           // 加載地圖物件
    while (loader->hasNext()) {
        MapObject block = loader->next();
        QRect     cropped(block.geometry.topLeft() * MAP_SCALE_FACTOR, block.geometry.size() * MAP_SCALE_FACTOR);
        cropped &= boardRect;
        if (cropped.isEmpty()) {
            continue;
        }
        renderBlock(block.type, cropped);
    }

           // 設置旗幟位置並渲染旗幟框架
    _flagPosition = loader->flagPosition() * MAP_SCALE_FACTOR;
    renderBlock(Nothing, QRect(_flagPosition, QSize(4, 4)));
    renderFlagFrame(Brick);

           // 設置敵方和友方坦克的起始位置
    _initialEnemyTanks = loader->enemyTanks();
    foreach (const QPoint &p, loader->enemyStartPositions()) {
        QPoint sp = p * MAP_SCALE_FACTOR;
        _enemyStartPositions.append(sp);
        renderBlock(Nothing, QRect(sp, QSize(4, 4)));
    }
    foreach (const QPoint &p, loader->friendlyStartPositions()) {
        QPoint sp = p * MAP_SCALE_FACTOR;
        _friendlyStartPositions.append(sp);
        renderBlock(Nothing, QRect(sp, QSize(4, 4)));
    }

    return true;
}

// 渲染地圖塊的函數
void Board::renderBlock(MapObjectType type, const QRect &area)
{
    QRect cr = QRect(QPoint(0, 0), _size) & area;
    if (cr.isEmpty())
        return;

    int start = posToMapIndex(cr.topLeft());
    for (int r = 0; r < cr.height(); r++) {
        for (int c = 0; c < cr.width(); c++) {
            _map[start + c] = type;
        }
        start += _size.width();
    }
}

// 渲染旗幟框架的函數
void Board::renderFlagFrame(MapObjectType type)
{
    QPoint tl = _flagPosition - QPoint(2, 2);
    renderBlock(type, QRect(tl, QSize(2, 8)));
    renderBlock(type, QRect(tl, QSize(8, 2)));
    renderBlock(type, QRect(tl + QPoint(0, 6), QSize(8, 2)));
    renderBlock(type, QRect(tl + QPoint(6, 0), QSize(2, 8)));
}

// 獲取地圖縮放因子的函數
int Board::blockDivider() const { return MAP_SCALE_FACTOR; }

// 根據地圖塊類型獲取屬性的函數
Board::BlockProps Board::blockTypeProperties(MapObjectType type) const
{
    switch (type) {
    case Concrete:
        return TankObstackle | BulletObstackle | Breakable | Sturdy;
    case Brick:
        return TankObstackle | BulletObstackle | Breakable;
    case Nothing:
    case Bush:
    case LastMapObjectType: // make compiler happy
        return {};
    case Ice:
        return BadManoeuvre;
    case Water:
        return TankObstackle;
    }
    return {};
}

// 計算區域屬性的函數
Board::BlockProps Board::rectProps(const QRect &rect)
{
    Board::BlockProps props;
    if (!QRect(QPoint(0, 0), _size).contains(rect)) {
        return TankObstackle;
    }
    for (int i = 0; i < rect.width(); i++) {
        for (int j = 0; j < rect.height(); j++) {
            props |= blockProperties(QPoint(rect.x() + i, rect.y() + j));
        }
    }
    return props;
}

} // namespace Tanks
