#include "dynamicblock.h"

namespace Tanks {

// DynamicBlock 類的構造函數
DynamicBlock::DynamicBlock(quint8 speed, Direction direction) : _clockPhase(0), _direction(direction)
{
    setSpeed(speed); // 設置速度
}

// 時間流逝的處理函數，控制動態塊的移動能力
void DynamicBlock::clockTick()
{
    if (_clockPhase) {
        _clockPhase--; // 時間減少，用於控制移動速度
    }
}

// 檢查動態塊是否可以移動的函數
bool DynamicBlock::canMove() const { return _clockPhase == 0; }

// 動態塊的移動函數
void DynamicBlock::move()
{
    int distance = 1; // 移動距離
    int dx = 0, dy = 0;
    switch (_direction) {
    case North:
        dy = -distance; // 向北移動
        break;
    case South:
        dy = distance; // 向南移動
        break;
    case West:
        dx = -distance; // 向西移動
        break;
    case East:
        dx = distance; // 向東移動
        break;
    }
    _geometry.translate(dx, dy); // 更新位置
    _clockPhase = _speed; // 重設時鐘階段，根據速度

    emit moved(); // 發送移動信號
}

// 獲取動態塊前方移動區域的函數
QRect DynamicBlock::forwardMoveRect(int distance) const
{
    switch (_direction) {
    case North:
        return QRect(_geometry.left(), _geometry.top() - distance, _geometry.width(), distance);
    case South:
        return QRect(_geometry.left(), _geometry.y() + _geometry.height(), _geometry.width(), distance);
    case West:
        return QRect(_geometry.left() - distance, _geometry.top(), distance, _geometry.height());
    case East:
        return QRect(_geometry.x() + _geometry.width(), _geometry.top(), distance, _geometry.height());
    }
    return QRect();
}

} // namespace Tanks
