#include "humanplayer.h"
#include "board.h"
#include "game.h"

#include <QDebug>

namespace Tanks {

// 人類玩家類的構造函數
HumanPlayer::HumanPlayer(Game *game, int playerIndex) :
    _game(game), _playerIndex(playerIndex), _lifes(3), _shooting(false)
{
}

// 獲取玩家生命值的函數
int HumanPlayer::lifesCount() const { return _lifes; }

// 開始遊戲時的初始化函數
void HumanPlayer::start()
{
    _tank         = QSharedPointer<Tank>(new Tank(Friendly));
    _oldDirection = _tank->direction();
    moveToStart(); // 移動至起始位置
    emit newTankAvailable(); // 發出新坦克可用的訊號
    // 連接坦克被摧毀的信號
    connect(_tank.data(), &Tank::tankDestroyed, this, &HumanPlayer::onTankDestroyed);
}

// 將坦克移動至起始位置的函數
void HumanPlayer::moveToStart()
{
    const auto &posList = _game->board()->friendlyStartPositions();
    _tank->setInitialPosition(posList[_playerIndex % posList.count()]);
}

// 控制移動的函數
void HumanPlayer::move(Direction dir)
{
    if (_tank) {
        Direction curDir = _tank->direction();
        _movingDir.erase(std::remove(_movingDir.begin(), _movingDir.end(), dir), _movingDir.end());
        _movingDir.push_front(dir);
        if (curDir != dir) {
            _oldDirection = curDir;
            _tank->setDirection(dir);
        }
    }
}

// 射擊函數
void HumanPlayer::fire() { _shooting = true; }

// 停止移動的函數
void HumanPlayer::stop(Direction dir)
{
    _movingDir.erase(std::remove(_movingDir.begin(), _movingDir.end(), dir), _movingDir.end());
    if (!_movingDir.empty()) {
        auto dir = _movingDir.front();
        _movingDir.pop_front();
        move(dir);
    }
}

// 停止射擊的函數
void HumanPlayer::stopFire() { _shooting = false; }

// 時間流逝的更新函數
void HumanPlayer::clockTick()
{
    if (!_tank) {
        return; // 無坦克時不做任何事
    }
    AbstractPlayer::clockTick();

    QRect             fmr;
    Board::BlockProps props;

    bool shouldMove  = !_movingDir.empty() && _tank->canMove();
    bool shouldShoot = (_shooting && _tank->canShoot());

    if (shouldMove || shouldShoot) {
        fmr   = _tank->forwardMoveRect(); // 坦克前方的空間
        props = _game->board()->rectProps(fmr);
    }

    if (shouldMove && !(props & Board::TankObstackle)) {
        _tank->move();
        _oldDirection = _tank->direction();
    }

    if (shouldShoot) {
        _tank->fire();
    }
}

// 坦克被摧毀時的處理函數
void HumanPlayer::onTankDestroyed()
{
    if (!_lifes) {
        qDebug("Something went wrong");
        return;
    }
    _lifes--;
    if (_lifes) {
        start(); // 重啟遊戲
    } else {
        _tank.clear(); // 清除坦克
    }
    emit lifeLost(); // 發出生命損失的訊號
}

// 摧毀所有的函數
void HumanPlayer::killAll()
{
    if (_tank) {
        _lifes = 1;
        _tank->selfDestroy();
    }
}

} // namespace Tanks
