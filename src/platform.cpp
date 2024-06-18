#include "platform.h"
#include "bullet.h"
#include "position.h"
#include <unistd.h>
#include <stdio.h>
#include <memory>

Platform::Platform(Position startPos, int maxBullets, std::mutex &rechargeMtx)
{

    this->position = startPos;
    this->maxBullets = maxBullets;
    this->actualBullets = maxBullets;
    this->rechargeMtx = &rechargeMtx;
}

void Platform::recharge()
{
    this->rechargeMtx->lock();
    actualBullets = maxBullets;
    sleep(1);
    this->rechargeMtx->unlock();
}

std::unique_ptr<Bullet> Platform::shoot()
{
    if(this->rechargeMtx->try_lock()){
        actualBullets--;
        auto b = std::make_unique<Bullet>(1, &position, cannonPosition);
        usleep(100001);
        this->rechargeMtx->unlock();
        return b;
    }
    
    return nullptr;    
}

Position Platform::getPosition()
{
    return this->position;
}

CannonPosition Platform::getCannonPosition()
{
    return this->cannonPosition;
}

void Platform::setCannonPosition(CannonPosition cannonPosition)
{
    this->cannonPosition = cannonPosition;
    return;
}

int Platform::getBullets()
{
    return actualBullets;
}

