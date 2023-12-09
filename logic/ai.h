#ifndef TANKS_AI_H
#define TANKS_AI_H

#include "aiplayer.h"

#include <QObject>

#include <list>

namespace Tanks {

class Game;

// AI 類，繼承自 QObject
class AI : public QObject {
    Q_OBJECT // 使用 Qt 的訊號與槽機制
public:
    // 構造函數，explicit 防止隱式轉換
    explicit AI(Game *game = 0);

           // 析構函數
    ~AI();

           // 重置 AI 狀態的函數
    void reset();

           // 獲取指向 Game 類實例的指針
    inline Game *game() const { return _game; }

           // 獲取待發送坦克的數量
    inline int pendingTanks() const { return _tanks.count(); }

           // 獲取 AI 生命值的數量
    inline int lifesCount() const { return _tanks.count() + _activePlayers.size(); }

           // 取出一個坦克類型
    inline quint8 takeTank() { return _tanks.takeFirst(); }

           // 啟動 AI 的函數
    void start();

           // 尋找與指定塊發生碰撞的 AI 玩家
    QSharedPointer<AIPlayer> findClash(const QSharedPointer<Block> &block);

           // 獲取初始位置的函數
    QPoint initialPosition() const;

           // 時間流逝的處理函數，控制 AI 的行為
    void clockTick();

signals:
    // 新玩家創建的訊號
    void newPlayer(Tanks::AIPlayer *);

public slots:

private slots:
    // 停用玩家的槽函數
    void deactivatePlayer();

private:
    Game                               *_game; // 指向 Game 類實例的指針
    QList<quint8>                       _tanks; // 存儲坦克類型的列表
    std::list<QSharedPointer<AIPlayer>> _activePlayers; // 存儲活躍的 AI 玩家
    std::list<QSharedPointer<AIPlayer>> _inactivePlayers; // 存儲非活躍的 AI 玩家
    int                                 _activateClock; // 控制 AI 玩家激活的計時器
};

} // namespace Tanks

#endif // TANKS_AI_H
