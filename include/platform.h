#ifndef PLATFORM_H
#define PLATFORM_H

#include "position.h"
#include "bullet.h"
#include <mutex>
#include <memory>

enum class CannonPosition { LEFT, MIDDLE_LEFT, UP, MIDDLE_RIGHT, RIGHT};

class Platform {
public:
    Platform(Position startPos, int maxBullets);
    void recharge();
    std::unique_ptr<Bullet> shoot();
    Position getPosition();
    int getBullets();


private:
    Position position;
    int maxBullets;
    CannonPosition cannonPosition;
    int actualBullets;
    std::mutex rechargeMtx;
};

#endif // PLATFORM_H