#ifndef BULLET_H
#define BULLET_H

#include "position.h"
#include "platform.h"

class Bullet {
    private:
        int speed;
        CannonPosition direction;
        bool alive;
        Position position;

    public:
        Bullet(int speed, Position* startPos, CannonPosition direction);
        bool isAlive();
        Position* getPosition();
        int getSpeed();
        void move();
        void kill();
};

#endif // BULLET_H
