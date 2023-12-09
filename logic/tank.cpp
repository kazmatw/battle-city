#include "tank.h"

namespace Tanks {

// 坦克類的構造函數，設置坦克的初始屬性
Tank::Tank(Affinity affinity, quint8 variant) : _affinity(affinity), _variant(variant), _shootTicks(0)
{
    setTankDefaults();
    _geometry.setWidth(4); // 在棋盤座標中設置寬度為 4
    _geometry.setHeight(4); // 在棋盤座標中設置高度為 4
}

// 設置坦克的預設屬性
void Tank::setTankDefaults()
{
    _armorLevel = 1; // 初始裝甲等級
    if (_affinity == Friendly) {
        _armorLevel = 1;
        if (_variant == BurstFireTank) { // 快速射擊坦克
            _bulletCount = 2; // 彈藥數量
        }
    } else {
        if (_variant == SpeedyTank) { // 高速坦克
            _speed = 2; // 設置速度
        }
        if (_variant == ArmoredTank) { // 裝甲坦克
            _armorLevel = 4; // 裝甲等級
        }
    }
}

// 坦克射擊功能
void Tank::fire()
{
    // 創建新的子彈實例
    auto b = new Bullet(_affinity, isArmorPiercing() ? Bullet::ArmorPiercing : Bullet::Regular);
    b->setSpeed(_affinity == Alien && _variant == FastBulletTank ? 3 : 2); // 設置子彈速度

    QRect fmr = QRect(0, 0, 2, 2);
    fmr.moveCenter(_geometry.center());
    int dx = 0, dy = 0;
    switch (_direction) {
    case North:
        dy = -1;
        break;
    case South:
        dy = 1;
        break;
    case West:
        dx = -1;
        break;
    case East:
        dx = 1;
        break;
    }
    fmr.translate(dx, dy);

    b->setInitialPosition(fmr.topLeft());
    b->setDirection(_direction);
    resetShootClock();

    _bullet = QSharedPointer<Bullet>(b);

    emit fired(); // 發射信號
}

// 重置射擊時鐘（用於控制射擊頻率）
void Tank::resetShootClock()
{
    _shootTicks = 10; // 預設值
    if (_affinity == Friendly && _variant == BurstFireTank) {
        _shootTicks = 5; // 快速射擊坦克的特殊處理
    }
}

// 處理坦克離開棋盤的行為
DynamicBlock::OutBoardAction Tank::outBoardAction() const { return DynamicBlock::StopMove; }

// 時鐘滴答函數，用於更新坦克狀態
void Tank::clockTick()
{
    if (_shootTicks) {
        _shootTicks--;
    }
    DynamicBlock::clockTick();
}

// 處理坦克被子彈擊中的情況
void Tank::catchBullet()
{
    if (!_armorLevel) {
        qDebug("Something went wrong"); // 錯誤處理
        return;
    }
    _armorLevel--;
    if (_armorLevel) {
        emit armourChanged(); // 裝甲等級改變的信號
    } else {
        emit tankDestroyed(); // 坦克被摧毀的信號
    }
}

// 自我摧毀功能
void Tank::selfDestroy()
{
    _armorLevel = 1;
    catchBullet();
}

} // namespace Tanks
