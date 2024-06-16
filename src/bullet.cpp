#include "bullet.h"

Bullet::Bullet(int speed, Position startPos, CannonPosition direction)
{
    this->speed = speed;
    this->position = startPos;
    this->direction = direction;
    this->alive = true;
}

void Bullet::move()
{
    switch (direction)
    {
        case CannonPosition::LEFT:
            position.x -= speed;
            break;
        case CannonPosition::MIDDLE_LEFT:
            position.x -= speed;  
            position.y -= speed;
            break;
        case CannonPosition::UP:
            position.y -= speed;
            break;
        case CannonPosition::MIDDLE_RIGHT:
            position.x += speed;
            position.y -= speed;
            break;
        case CannonPosition::RIGHT:
            position.x += speed;
            break;
        default:
            break;
    }
}

bool Bullet::isAlive()
{
    return this->alive;
}

void Bullet::kill()
{
    this->alive = false;
}
