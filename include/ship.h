#ifndef SHIP_H
#define SHIP_H

#include <mutex>
#include "position.h"
#include "positionedItem.h"

class Ship : public PositionedItem {
private:
    std::mutex moveMtx;
    int actualLife;
    Position position;
    int maxLife;
    int speed;

public:
    Ship(Position startPos, int maxLife, int speed);
    int getLife();
    void setLife(int life);
    Position getPosition();
};

#endif // SHIP_H