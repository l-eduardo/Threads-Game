// src/kbreader.cpp
#include "kbreader.h"
#include <unistd.h> // Necessário para usar tcgetattr e tcsetattr
#include <iostream>
#include <ncurses.h>

KeyboardReader::KeyboardReader() {
//     // Save the current terminal settings
//     tcgetattr(STDIN_FILENO, &originalSettings_);

//     // Create a copy of the original settings
//     modifiedSettings_ = originalSettings_;

//     // Disable canonical mode and echo
//     modifiedSettings_.c_lflag &= ~(ICANON | ECHO);

//     // Apply the modified settings
//     tcsetattr(STDIN_FILENO, TCSANOW, &modifiedSettings_);
}

KeyboardReader::~KeyboardReader() {
    // Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings_);
}

char KeyboardReader::readKey() {
    return wgetch(stdscr);
}
