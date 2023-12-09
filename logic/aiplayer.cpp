#include "aiplayer.h"
#include "ai.h"
#include "board.h"
#include "flag.h"
#include "game.h"

#include <QDebug>
#include <QRandomGenerator>

namespace Tanks {

// AIPlayer 類的構造函數
AIPlayer::AIPlayer(AI *ai) : _ai(ai) { }

// 獲取 AI 玩家生命值的函數
int AIPlayer::lifesCount() const { return _ai->pendingTanks(); }

// 啟動 AI 玩家的函數
void AIPlayer::start()
{
    // 創建一個新的 AI 控制的坦克
    _tank = QSharedPointer<Tank>(new Tank(Alien, _ai->takeTank()));
    _tank->setInitialPosition(_ai->initialPosition());
    emit newTankAvailable();
    connect(_tank.data(), &Tank::tankDestroyed, this, &AIPlayer::onTankDestroyed);
}

// 時間流逝的處理函數，控制 AI 玩家的行為
void AIPlayer::clockTick()
{
    AbstractPlayer::clockTick();

    if (!_tank) {
        return; // 如果沒有坦克，則不執行任何操作
    }

    bool forceShoot = false;

           // 決定坦克的移動和射擊行為
    if (_tank->canMove()) {
        Board::BlockProps props          = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
        bool              canMoveForward = !(props & Board::TankObstackle);

        int r = QRandomGenerator::global()->bounded(16);
        int d = QRandomGenerator::global()->bounded(16);

        bool moving     = r < 15;
        bool needNewDir = !canMoveForward || d > 13;

        if (needNewDir) {
            if (_ai->game()->flag()->isBroken()) {
                _tank->setDirection((Direction)(QRandomGenerator::global()->bounded(4)));
            } else {
                QPoint    tc = _tank->geometry().center();
                QPoint    fc = _ai->game()->flag()->geometry().center();
                Direction dirs[4]; // first directions are more probable (towards the flag)
                if (tc.x() < fc.x()) {
                    dirs[0] = East;
                    dirs[2] = West;
                } else {
                    dirs[0] = West;
                    dirs[2] = East;
                }
                if (tc.y() < fc.y()) {
                    dirs[1] = South;
                    dirs[3] = North;
                } else {
                    dirs[1] = North;
                    dirs[3] = South;
                }

                int toFlagInd
                    = QRandomGenerator::global()->generate() < (std::numeric_limits<quint32>::max() * 0.9) ? 0 : 2;
                Direction newDir = dirs[toFlagInd + (d & 1)];

                _tank->setDirection(newDir);
            }
            props          = _ai->game()->board()->rectProps(_tank->forwardMoveRect());
            canMoveForward = !(props & Board::TankObstackle);
        }

        if (canMoveForward && moving) {
            _tank->move();
        } else if (props & Board::Breakable && !(props & Board::Sturdy)) {
            forceShoot = true;
        }

        if (!moving) {
            _tank->setClockPhase(20);
        }
    }

           // 決定是否射擊
    if (_tank->canShoot()) {
        if (forceShoot || QRandomGenerator::global()->generate() < std::numeric_limits<quint32>::max() / 100) {
            _tank->fire();
        }
    }
}

// 坦克被摧毀時的處理函數
void AIPlayer::onTankDestroyed()
{
    _tank.clear();
    emit lifeLost();
}

} // namespace Tanks
