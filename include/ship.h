#ifndef SHIP_H
#define SHIP_H

#include "position.h"

class Ship {
private:
    int actualLife;
    Position position;
    int maxLife;
    int speed;

public:
    Ship(Position startPos, int maxLife, int speed);
    int getLife();
    void move(Position newPos);
    void moveToTop();
    void moveDown();
    void setLife(int life);
    Position getPosition();
};

#endif // SHIP_H