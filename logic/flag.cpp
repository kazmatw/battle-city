#include "flag.h"

namespace Tanks {

// Flag 類的構造函數，設置初始狀態和幾何大小
Flag::Flag() : _broken(false) { _geometry.setSize(QSize(4, 4)); }

// 恢復旗幟至初始狀態的函數
void Flag::restore()
{
    _broken = false; // 將旗幟狀態設為未破壞
    emit changed(); // 發送狀態變化的信號
}

// 燃燒（破壞）旗幟的函數
void Flag::burn()
{
    _broken = true; // 將旗幟狀態設為已破壞
    emit changed(); // 發送狀態變化的信號
}

} // namespace Tanks
