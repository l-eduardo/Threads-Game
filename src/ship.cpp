#include "ship.h"
#include <stdio.h>


Ship::Ship(Position startPos, int maxLife, int speed)
{
  this->maxLife = maxLife;
  this->actualLife = maxLife;
  this->position = startPos;
  this->speed = speed;
  this->alive = true;
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

void Ship::damage(int damage)
{
  if (actualLife > 1)
    actualLife -= damage;
  else
    kill();
}

void Ship::kill()
{
  // printf("Ship killed\n");
  alive = false;
  return;
}

bool Ship::isAlive()
{
  return alive;
}

int Ship::getWidth()
{
    return this->width;
}

int Ship::getHeight()
{
    return this->height;
}

Position Ship::getPosition()
{
  return this->position;
}
