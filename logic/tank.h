#ifndef TANKS_TANK_H
#define TANKS_TANK_H

#include "bullet.h"
#include "dynamicblock.h"

namespace Tanks {

// Tank 類，繼承自 DynamicBlock
class Tank : public DynamicBlock {
    Q_OBJECT // 使用 Qt 的訊號與槽機制
public:
    // 枚舉類型，定義友方坦克的變體
    enum FriendlyVariant {
        SmallTank,
        SpeedFireTank,
        BurstFireTank,
        ArmorPiercingTank,
    };

           // 枚舉類型，定義敵方坦克的變體
    enum EnemyVariant {
        RegularTank,
        SpeedyTank,
        FastBulletTank,
        ArmoredTank,

        LastEnemyVariant
    };

           // Tank 類的構造函數
    Tank(Affinity affinity, quint8 variant = 0);

           // 獲取坦克的親和性和變體
    inline Affinity affinity() const { return _affinity; }
    inline quint8   variant() const { return _variant; }

           // 設置坦克的預設值
    void setTankDefaults();

           // 檢查坦克是否能夠射擊
    inline bool canShoot() const { return _shootTicks == 0; }

           // 判斷是否為穿甲彈
    bool isArmorPiercing() const { return (_affinity == Friendly) && (_variant == ArmorPiercingTank); }

           // 坦克射擊的函數
    void fire();

           // 重置射擊計時器
    void resetShootClock();

           // 處理坦克離開遊戲範圍的行動
    OutBoardAction outBoardAction() const;

           // 時間流逝的處理函數
    void clockTick();

           // 坦克被子彈擊中的處理函數
    void catchBullet();

           // 自我摧毀的函數
    void selfDestroy();

           // 獲取射擊後的子彈
    QSharedPointer<Bullet> takeBullet()
    {
        QSharedPointer<Bullet> ret;
        _bullet.swap(ret);
        return ret;
    }

           // 獲取當前的子彈
    QSharedPointer<Bullet> bullet() const { return _bullet; }

signals:
    // 坦克被摧毀的訊號
    void tankDestroyed();

           // 裝甲等級改變的訊號
    void armourChanged();

           // 射擊的訊號
    void fired();

private:
    Affinity               _affinity; // 坦克的親和性
    quint8                 _variant; // 坦克的變體
    quint8                 _armorLevel; // 裝甲等級
    quint8                 _bulletCount; // 子彈數量
    int                    _shootTicks; // 射擊計時器
    QSharedPointer<Bullet> _bullet; // 存儲坦克射擊的子彈
};

} // namespace Tanks

#endif // TANKS_TANK_H
