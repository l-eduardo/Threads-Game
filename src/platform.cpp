#include "platform.h"
#include "bullet.h"
#include "position.h"
#include <unistd.h>
#include <stdio.h>
#include <memory>

Platform::Platform(Position startPos, int maxBullets)
{
    this->position = startPos;
    this->maxBullets = maxBullets;
    this->actualBullets = maxBullets;
}

void Platform::recharge()
{
    rechargeMtx.lock();
    actualBullets = maxBullets;
    sleep(1);
    rechargeMtx.unlock();
}

std::unique_ptr<Bullet> Platform::shoot()
{
    if(rechargeMtx.try_lock()){
        actualBullets--;
        return std::make_unique<Bullet>(1, position, cannonPosition);
    }
    
    return nullptr;    
}

Position Platform::getPosition()
{
    return this->position;
}

int Platform::getBullets()
{
    return actualBullets;
}

