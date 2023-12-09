#include "abstractplayer.h"

namespace Tanks {

AbstractPlayer::AbstractPlayer(QObject *parent) : QObject(parent) { }

void AbstractPlayer::clockTick()
{
    if (_tank) {
        _tank->clockTick();
    }
}

} // namespace Tanks
