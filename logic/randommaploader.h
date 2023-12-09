#ifndef TANKS_RANDOMMAPLOADER_H
#define TANKS_RANDOMMAPLOADER_H

#include "abstractmaploader.h"

#include <QQueue>

namespace Tanks {

// RandomMapLoader 類，用於隨機生成遊戲地圖
class RandomMapLoader : public AbstractMapLoader {
    // PendingShape 結構，代表等待生成的形狀
    struct PendingShape {
        MapObjectType type; // 形狀的類型
        int           minSize; // 最小尺寸
        int           maxSize; // 最大尺寸
    };

public:
    RandomMapLoader();

           // 實現抽象基類的方法
    bool          open();
    QSize         dimensions() const;
    bool          hasNext() const;
    MapObject     next();
    QList<quint8> enemyTanks() const;
    QList<QPoint> enemyStartPositions() const;
    QList<QPoint> friendlyStartPositions() const;
    QPoint        flagPosition() const;

private:
    // 生成形狀的私有函數
    void generateShape(const PendingShape &shape);

private:
    int                  boardWidth; // 棋盤的寬度
    int                  boardHeight; // 棋盤的高度
    QQueue<PendingShape> shapesQueue; // 待生成形狀的隊列
    QQueue<MapObject>    objectQueue; // 地圖物體的隊列
};

} // namespace Tanks

#endif // TANKS_RANDOMMAPLOADER_H
