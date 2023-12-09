#ifndef TANKS_ABSTRACTMAPLOADER_H
#define TANKS_ABSTRACTMAPLOADER_H

#include "basics.h"

#include <QRect>
#include <QtGlobal>

namespace Tanks {

// MapObject 類，用於表示地圖上的物體
class MapObject {
public:
    QRect         geometry; // 物體的幾何位置和大小
    MapObjectType type; // 物體的類型
};

// AbstractMapLoader 抽象類，用於定義地圖加載器的接口
class AbstractMapLoader {
public:
    AbstractMapLoader();
    virtual ~AbstractMapLoader();

           // 打開地圖加載器的虛擬函數
    virtual bool          open()                         = 0;

           // 獲取地圖尺寸的虛擬函數
    virtual QSize         dimensions() const             = 0;

           // 檢查是否還有更多地圖物體的虛擬函數
    virtual bool          hasNext() const                = 0;

           // 獲取下一個地圖物體的虛擬函數
    virtual MapObject     next()                         = 0;

           // 獲取敵方坦克列表的虛擬函數
    virtual QList<quint8> enemyTanks() const             = 0;

           // 獲取敵方起始位置的虛擬函數
    virtual QList<QPoint> enemyStartPositions() const    = 0;

           // 獲取友方起始位置的虛擬函數
    virtual QList<QPoint> friendlyStartPositions() const = 0;

           // 獲取旗幟位置的虛擬函數
    virtual QPoint        flagPosition() const           = 0;
};

} // namespace Tanks

#endif // TANKS_ABSTRACTMAPLOADER_H
