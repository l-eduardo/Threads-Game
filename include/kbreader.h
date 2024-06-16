// include/kbreader.h
#ifndef KBREADER_H
#define KBREADER_H

#include <termios.h>

class KeyboardReader {
public:
    KeyboardReader();
    ~KeyboardReader();
    char readKey();

private:
    struct termios originalSettings_;
    struct termios modifiedSettings_;
};

#endif // KBREADER_H
