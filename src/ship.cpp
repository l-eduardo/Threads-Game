#include "ship.h"
#include <mutex>


Ship::Ship(Position startPos, int maxLife, int speed)
{
    this->maxLife = maxLife;
    this->actualLife = maxLife;
    this->position = startPos;
    this->speed = speed;
}

void Ship::move(Position newPos)
{
    this->position = newPos;
    return;
}

void Ship::moveToTop()
{
    this->position.y -= speed;
}

void Ship::moveDown()
{
    this->position.y += speed;
}

int Ship::getLife()
{
    return this->actualLife;
}

void Ship::setLife(int newLife)
{
    actualLife = newLife;
}

Position Ship::getPosition()
{
    return this->position;
}
