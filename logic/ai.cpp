#include "ai.h"
#include "aiplayer.h"
#include "board.h"
#include "game.h"

#include <QRandomGenerator>

namespace Tanks {

// AI 類的構造函數
AI::AI(Game *game) : QObject(game), _game(game), _activateClock(0) { }

// AI 類的析構函數
AI::~AI() { reset(); }

// 重置 AI 狀態的函數
void AI::reset()
{
    _activePlayers.clear(); // 清除活躍的 AI 玩家列表
    _inactivePlayers.clear(); // 清除不活躍的 AI 玩家列表
    _tanks.clear(); // 清除坦克列表
    _activateClock = 0; // 重置激活時鐘
}

// 啟動 AI 的函數
void AI::start()
{
    _tanks = _game->board()->initialEnemyTanks();
    for (int i = 0; i < 8; i++) { // 同時在地圖上最多顯示 4 個坦克
        auto robot = QSharedPointer<AIPlayer>(new AIPlayer(this));
        _inactivePlayers.push_back(robot);

        connect(robot.data(), &AIPlayer::lifeLost, this, &AI::deactivatePlayer);
        emit newPlayer(robot.data());
    }
}

// 查找和指定塊發生碰撞的 AI 玩家的函數
QSharedPointer<AIPlayer> AI::findClash(const QSharedPointer<Block> &block)
{
    foreach (auto p, _activePlayers) {
        if (p->tank() && p->tank()->hasClash(*block)) {
            return p;
        }
    }
    return QSharedPointer<AIPlayer>();
}

// 獲取 AI 玩家的初始位置的函數
QPoint AI::initialPosition() const
{
    auto &pos = _game->board()->enemyStartPositions();
    return pos.value(QRandomGenerator::global()->bounded(pos.count()));
}

// 時間流逝的處理函數，控制 AI 玩家的行為
void AI::clockTick()
{
    if (_activateClock) {
        _activateClock--;
    }
    if (!_activateClock && !_inactivePlayers.empty() && _tanks.count()) {
        auto player = _inactivePlayers.front();
        _inactivePlayers.pop_front();
        _activePlayers.push_back(player);
        player->start();
        _activateClock = 100;
    }

    for (auto &p : _activePlayers) {
        p->clockTick();
    }
}

// 停用 AI 玩家的函數
void AI::deactivatePlayer()
{
    auto it = _activePlayers.begin();
    while (it != _activePlayers.end()) {
        if ((*it).data() == sender()) {
            _inactivePlayers.push_back(*it);
            _activePlayers.erase(it);
            break;
        }
        ++it;
    }
}

} // namespace Tanks
