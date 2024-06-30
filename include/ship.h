#ifndef SHIP_H
#define SHIP_H

#include "position.h"

class Ship {
private:
    int actualLife;
    Position position;
    int maxLife;
    int speed;
    bool alive;
    int width = 3;
    int height = 3;

public:
    Ship(Position startPos, int maxLife, int speed);
    int getLife();
    void move(Position newPos);
    void moveToTop();
    void moveDown();
    void setLife(int life);
    void damage(int damage);
    void kill();
    bool isAlive();
    int getWidth();
    int getHeight();
    Position getPosition();
};

#endif // SHIP_H