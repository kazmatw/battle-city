#include "bullet.h"

namespace Tanks {

// Bullet 類的構造函數
Bullet::Bullet(Affinity affinity, Level level) : _affinity(affinity), _level(level)
{
    _geometry.setWidth(2); // 設置子彈的寬度
    _geometry.setHeight(2); // 設置子彈的高度
}

// 子彈爆炸的函數
void Bullet::explode(Bullet::ExplosionType et)
{
    _etype = et; // 設置爆炸類型
    emit detonated(); // 發出爆炸信號
}

// 當子彈離開遊戲範圍時的行動
DynamicBlock::OutBoardAction Bullet::outBoardAction() const { return DynamicBlock::Disappear; }

} // namespace Tanks
