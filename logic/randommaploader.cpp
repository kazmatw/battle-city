#include "randommaploader.h"
#include "tank.h"

#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QRandomGenerator>

namespace Tanks {

// 隨機地圖加載器的構造函數，初始化棋盤的寬度和高度
RandomMapLoader::RandomMapLoader() : boardWidth(50), boardHeight(50) { }

// 打開地圖加載器，初始化各種地形和物體的隊列
bool RandomMapLoader::open()
{
    int i;

    shapesQueue.clear();
    objectQueue.clear();

           // 初始化磚塊
    for (i = 0; i < 20; i++) {
        shapesQueue.enqueue({ Brick, 4, 20 });
    }

           // 初始化混凝土
    for (i = 0; i < 10; i++) {
        shapesQueue.enqueue({ Concrete, 3, 8 });
    }

           // 初始化水域
    for (i = 0; i < 10; i++) {
        shapesQueue.enqueue({ Water, 3, 8 });
    }

           // 初始化冰面
    for (i = 0; i < 10; i++) {
        shapesQueue.enqueue({ Ice, 3, 8 });
    }

           // 初始化灌木叢
    for (i = 0; i < 20; i++) {
        shapesQueue.enqueue({ Bush, 3, 8 });
    }
    return true;
}

// 生成隨機形狀的函數
void RandomMapLoader::generateShape(const PendingShape &shape)
{
    // 生成隨機大小和位置
    int rndWidth  = qMax(shape.minSize, QRandomGenerator::global()->bounded(shape.maxSize + 1));
    int rndHeight = qMax(shape.minSize, QRandomGenerator::global()->bounded(shape.maxSize + 1));
    int rndLeft   = QRandomGenerator::global()->bounded(boardWidth) - rndWidth / 2;
    int rndTop    = QRandomGenerator::global()->bounded(boardHeight) - rndHeight / 2;

    int shapeVariant = QRandomGenerator::global()->bounded(6); // 偏重於橢圓形
    switch (shapeVariant) {
    case 0:
        // 垂直條
        objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, 2), shape.type });
        return;
    case 1:
        // 水平條
        objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, 2, rndHeight), shape.type });
        return;
    case 2:
        // 其他形狀
        if (shape.type != Brick) {
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, rndHeight), shape.type });
        } else {
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, 1, rndHeight), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop, rndWidth, 1), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft + rndWidth - 1, rndTop, 1, rndHeight), shape.type });
            objectQueue.enqueue(MapObject { QRect(rndLeft, rndTop + rndHeight - 1, rndWidth, 1), shape.type });
        }
        return;
    }

           // 處理複雜形狀
    QSize  shapeSize(rndWidth, rndHeight);
    QImage img(shapeSize, QImage::Format_ARGB32);
    img.fill(QColor(Qt::black));
    QPainter painter(&img);
    if (shape.type != Brick) {
        painter.setBrush(QColor(Qt::white));
    } else {
        QPen pen(Qt::white);
        int  penWidth = 2;
        pen.setWidth(penWidth);
        painter.setPen(pen);
        shapeSize -= QSize(penWidth, penWidth);
    }
    QRect drawRect(QPoint(0, 0), shapeSize);

           // 目前只支持橢圓形
    painter.drawEllipse(drawRect);

    for (int y = 0; y < rndHeight; y++) {
        for (int x = 0; x < rndWidth; x++) {
            if (img.pixelColor(x, y) != Qt::black) {
                objectQueue.enqueue(MapObject { QRect(rndLeft + x, rndTop + y, 1, 1), shape.type });
            }
        }
    }
}

// 獲取棋盤尺寸
QSize RandomMapLoader::dimensions() const { return QSize(boardWidth, boardHeight); }

// 檢查是否還有更多形狀或物體
bool RandomMapLoader::hasNext() const { return !shapesQueue.isEmpty() || !objectQueue.isEmpty(); }

// 獲取下一個地圖物體
MapObject RandomMapLoader::next()
{
    if (objectQueue.isEmpty()) {
        Q_ASSERT(!shapesQueue.isEmpty());
        generateShape(shapesQueue.dequeue());
    }
    return objectQueue.dequeue();
}

// 生成敵方坦克
QList<quint8> RandomMapLoader::enemyTanks() const
{
    QList<quint8> ret;
    ret.reserve(10);
    for (int i = 0; i < 20; i++) {
        int val = QRandomGenerator::global()->bounded(12);
        if (val > 10) { // 11
            ret.append(Tank::ArmoredTank);
        } else if (val > 7) { // 8,9
            ret.append(Tank::FastBulletTank);
        } else if (val > 4) { // 5,6,7
            ret.append(Tank::SpeedyTank);
        } else { // 0,1,2,3,4
            ret.append(Tank::RegularTank);
        }
    }
    return ret;
}

// 生成敵方坦克的起始位置
QList<QPoint> RandomMapLoader::enemyStartPositions() const
{
    return QList<QPoint>() << QPoint(0, 0) << QPoint(boardWidth - 2, 0) << QPoint(boardWidth / 2, 0);
}

// 生成友軍坦克的起始位置
QList<QPoint> RandomMapLoader::friendlyStartPositions() const
{
    return QList<QPoint>() << QPoint(boardWidth / 2 - 5, boardHeight - 2)
                           << QPoint(boardWidth / 2 + 1, boardHeight - 2);
}

// 生成旗幟的位置
QPoint RandomMapLoader::flagPosition() const { return QPoint(boardWidth / 2 - 2, boardHeight - 2); }

} // namespace Tanks
