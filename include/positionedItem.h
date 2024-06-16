#ifndef POSITIONED_ITEM_H
#define POSITIONED_ITEM_H

#include <mutex>
#include "position.h"

class PositionedItem {
private:
    Position position;
    int speed;

protected:
    PositionedItem(Position startPos, int maxLife, int speed) : position(startPos), speed(speed) {};
    void virtual move(Position newPos);
    void virtual move(int x, int y);
    void virtual moveToLeft();
    void virtual moveToRight();
    void virtual moveToTop();
    void virtual moveToBottom();

public:
    Position getPosition();
};

#endif // POSITIONED_ITEM_H