#ifndef TANKS_ABSTRACTPLAYER_H
#define TANKS_ABSTRACTPLAYER_H

#include "bullet.h"
#include "tank.h"

#include <QObject>

namespace Tanks {

class AbstractPlayer : public QObject {
    Q_OBJECT
public:
    explicit AbstractPlayer(QObject *parent = 0);
    virtual int          lifesCount() const = 0;
    QSharedPointer<Tank> tank() const { return _tank; }
    virtual void         clockTick();

signals:
    void newTankAvailable();
    void lifeLost();

public slots:

protected:
    QSharedPointer<Tank> _tank;
};

} // namespace Tanks

#endif // TANKS_ABSTRACTPLAYER_H
