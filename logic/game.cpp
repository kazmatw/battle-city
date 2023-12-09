#include "game.h"
#include "ai.h"
#include "aiplayer.h"
#include "board.h"
#include "flag.h"
#include "humanplayer.h"
#include "randommaploader.h"
#include "tank.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

#include <list>

namespace Tanks {

// GamePrivate 類，用於管理遊戲的內部狀態
class GamePrivate {
public:
    GamePrivate(Game *game) : game(game), board(), mapLoader(nullptr), playersCount(1) { }

    Game              *game; // 指向遊戲物件的指針
    Board             *board; // 棋盤物件
    AbstractMapLoader *mapLoader; // 地圖加載器
    QTimer            *clock; // 遊戲時鐘
    quint8             playersCount; // 玩家數量
    AI                *ai; // AI 物件

    QList<QSharedPointer<HumanPlayer>> humans; // 人類玩家列表
    std::list<QSharedPointer<Bullet>>  bullets; // 子彈列表

    QSharedPointer<Flag> flag; // 旗幟物件
};

// Game 類的構造函數
Game::Game(QObject *parent) : QObject(parent), _d(new GamePrivate(this))
{
    // 初始化 Game 的各個部分
    _d->board     = new Board(this);
    _d->ai        = new AI(this);
    _d->mapLoader = new RandomMapLoader();
    _d->flag      = QSharedPointer<Flag>(new Flag);

           // 設置遊戲時鐘
    _d->clock = new QTimer(this);
    _d->clock->setInterval(50);
    connect(_d->clock, &QTimer::timeout, this, &Game::clockTick);
    connect(_d->ai, &AI::newPlayer, this, &Game::connectPlayerSignals);
}

// Game 類的析構函數
Game::~Game()
{
    reset();
    delete _d->mapLoader;
    delete _d;
}

// 重置遊戲狀態的函數
void Game::reset()
{
    _d->clock->stop();
    _d->humans.clear();
    _d->bullets.clear();
    _d->ai->reset();
    //_d->board->reset();
}

// 獲取 Board 物件的函數
Board *Game::board() const { return _d->board; }

// 設置玩家數量的函數
void Game::setPlayersCount(int n)
{
    _d->playersCount = qMax(1, qMin(n, 20));
}

// 獲取玩家數量的函數
int Game::playersCount() { return _d->playersCount; }

// 獲取 AI 生命值的函數
int Game::aiLifes() { return _d->ai->lifesCount(); }

// 獲取特定玩家生命值的函數
int Game::playerLifes(int playerId)
{
    if (!_d->humans.count()) {
        return 0;
    }
    auto player(_d->humans.value(playerId % _d->humans.count()));
    if (player) {
        return player->lifesCount();
    }
    return 0;
}

// 獲取旗幟物件的函數
QSharedPointer<Flag> &Game::flag() const { return _d->flag; }

// 連接玩家訊號的函數
void Game::connectPlayerSignals(AbstractPlayer *player)
{
    connect(player, &AbstractPlayer::newTankAvailable, this, &Game::newTankAvailable);
    connect(player, &AbstractPlayer::lifeLost, this, &Game::statsChanged);
}

// 處理玩家移動請求的函數
void Game::playerMoveRequested(int playerNum, int direction)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->move((Direction)direction);
    }
}

// 處理玩家射擊請求的函數
void Game::playerFireRequested(int playerNum)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->fire();
    }
}

// 處理玩家停止移動請求的函數
void Game::playerStopMoveRequested(int playerNum, int direction)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stop((Direction)direction);
    }
}

// 處理玩家停止射擊請求的函數
void Game::playerStopFireRequested(int playerNum)
{
    if (!_d->humans.count()) {
        return;
    }
    auto player = _d->humans.value(playerNum % _d->humans.count());
    if (player) {
        player->stopFire();
    }
}

// 啟動遊戲的函數
void Game::start(int playersCount)
{
    reset();
    _d->playersCount = playersCount;
    if (!_d->board->loadMap(_d->mapLoader)) {
        qDebug("Failed to load map");
        return;
    }
    QTimer::singleShot(0, this, &Game::mapReady);
}

// 地圖準備就緒的處理函數
void Game::mapReady()
{
    _d->flag->restore();
    _d->flag->setInitialPosition(_d->board->flagPosition());
    emit mapLoaded();

    for (int i = 0; i < _d->playersCount; i++) {
        auto human = new HumanPlayer(this, i);
        connectPlayerSignals(human);
        _d->humans.append(QSharedPointer<HumanPlayer>(human));
        human->start();
    }

    _d->ai->start();
    _d->clock->start();

    emit statsChanged();
}

// 新坦克可用的處理函數
void Game::newTankAvailable()
{
    AbstractPlayer *player = qobject_cast<AbstractPlayer *>(sender());
    auto            tank   = player->tank();
    emit            newTank(tank.data());

    connect(tank.data(), &Tank::fired, this, &Game::onTankFired);
}

// 坦克射擊的處理函數
void Game::onTankFired()
{
    Tank *tank   = qobject_cast<Tank *>(sender());
    auto  bullet = tank->takeBullet();
    _d->bullets.push_front(bullet);
}

// 時間流逝的處理函數
void Game::clockTick()
{
    foreach (auto p, _d->humans) {
        p->clockTick();
    }
    _d->ai->clockTick();

    for (int bMove = 0; bMove < 2; bMove++) {
        moveBullets();
    }
}

// 子彈移動的函數
void Game::moveBullets()
{
    auto it = _d->bullets.begin();
    while (it != _d->bullets.end()) {
        auto bullet = *it;

        bool                  clashFound = false;
        Bullet::ExplosionType explType   = Bullet::Explosion;
        if (bullet->affinity() == Alien) {
            foreach (auto p, _d->humans) {
                if (p->tank() && p->tank()->hasClash(*bullet)) {
                    p->tank()->catchBullet();
                    clashFound = true;
                    break;
                }
            }
        } else {
            QSharedPointer<AIPlayer> p = _d->ai->findClash(bullet.dynamicCast<Block>());
            if (p) {
                p->tank()->catchBullet();
                clashFound = true;
            }
        }
        if (!clashFound && _d->flag->hasClash(*bullet)) {
            clashFound = true;
            _d->flag->burn();
            explType = Bullet::BigExplosion;
            emit flagLost();
            foreach (auto &human, _d->humans) {
                if (human->tank()) { // still alive
                    human->killAll();
                }
            }
        }
        if (!clashFound) {
            // meet other bullets
            auto it2 = it;
            ++it2;
            Affinity invAff = bullet->affinity() == Alien ? Friendly : Alien;
            while (it2 != _d->bullets.end()) {
                auto &b2 = **it2;
                if (b2.affinity() == invAff && b2.hasClash(*bullet)) {
                    explType = Bullet::BrickDestroyed;
                    b2.explode(explType); // let's imagine tank shoot with bricks / FIXME
                    _d->bullets.erase(it2);
                    clashFound = true;
                    break;
                }
                ++it2;
            }
        }
        if (clashFound) {
            it = _d->bullets.erase(it);
            bullet->explode(explType);
        } else {
            QRect fmr = bullet->forwardMoveRect();
            // qDebug() << "Bullet forward" << fmr;
            auto props = _d->board->rectProps(fmr);
            if (props & Board::BulletObstackle) {

                       // resize damage area to four blocks
                if (fmr.width() > fmr.height()) {
                    fmr.setWidth(4);
                    fmr.translate(-1, 0);
                } else {
                    fmr.setHeight(4);
                    fmr.translate(0, -1);
                }
                bool brickDamage = false;
                for (int i = 0; i < fmr.width(); i++) {
                    for (int j = 0; j < fmr.height(); j++) {
                        QPoint p(fmr.left() + i, fmr.top() + j);
                        props = _d->board->blockProperties(p);
                        if (props & Board::Breakable) {
                            if (bullet->level() == Bullet::ArmorPiercing || !(props & Board::Sturdy)) {
                                QRect r(p, QSize(1, 1));
                                _d->board->renderBlock(Nothing, r);
                                emit blockRemoved(r);
                                brickDamage = true;
                            }
                        }
                    }
                }
                it = _d->bullets.erase(it);
                // qDebug("Remove!!!");
                bullet->explode(brickDamage ? Bullet::BrickDestroyed : Bullet::NoDamage);
            } else {
                if (bullet->canMove()) {
                    bullet->move();
                    if (!QRect(QPoint(0, 0), _d->board->size()).contains(bullet->geometry())) {
                        it = _d->bullets.erase(it);
                        bullet->explode(Bullet::NoDamage);
                        continue;
                    }
                    // emit bulletMoved(bullet.data());
                }

                bullet->clockTick();
                ++it;
            }
        }
    }
}

} // namespace Tanks
