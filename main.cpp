#include "bullet.h"
#include "kbreader.h"
#include "platform.h"
#include "ship.h"
#include <iostream>
#include <mutex>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::vector<Bullet> bullets;
std::vector<Ship> ships;
std::mutex rechargeMtx;
std::mutex shipsMtx;
std::mutex bulletsMtx;
std::mutex engineMtx;
std::unordered_map<char, std::thread> threads;

int EXIT_FLAG = 0;
int MAX_HEIGHT, MAX_WIDTH;

void drawPositionedItems(Platform *plat);
void positionControl();
void generateShips();
void readKeyboard(KeyboardReader kbReader, Platform *plat);
void drawPlataform(Platform *plat);
void gameEngine();

int main() {
  initscr(); // Inicializa a tela ncurses
  noecho();  // Não ecoa as teclas digitadas
             // curs_set(FALSE);       // Esconde o cursor
  cbreak();
  curs_set(0);

  getmaxyx(stdscr, MAX_HEIGHT, MAX_WIDTH);

  Platform plataform({MAX_WIDTH / 2, MAX_HEIGHT - 1}, 10, rechargeMtx);
  KeyboardReader kbreader = KeyboardReader();

  std::thread generateShipsThread(generateShips);
  std::thread positionControlThread(positionControl);
  std::thread drawPositionedItemsThread(drawPositionedItems, &plataform);
  std::thread readKeyboardThread(readKeyboard, kbreader, &plataform);

  generateShipsThread.join();
  positionControlThread.join();
  drawPositionedItemsThread.join();
  readKeyboardThread.join();

  return 0;
}

void readKeyboard(KeyboardReader kbReader, Platform *plat) {
  while (true) {
    char c = kbReader.readKey();
    switch (c) {
    case 'a':
      plat->setCannonPosition(CannonPosition::LEFT);
      break;
    case 'q':
      plat->setCannonPosition(CannonPosition::MIDDLE_LEFT);
      break;
    case 'w':
      plat->setCannonPosition(CannonPosition::UP);
      break;
    case 'e':
      plat->setCannonPosition(CannonPosition::MIDDLE_RIGHT);
      break;
    case 'd':
      plat->setCannonPosition(CannonPosition::RIGHT);
      break;
    case 'r':
      plat->recharge();
      break;
    case ' ':
      if (plat->getBullets() > 0) {
        auto shoot = plat->shoot();
        if (shoot == nullptr) {
          break;
        }
        bullets.emplace_back(shoot->getSpeed(), shoot->getPosition(),
                             plat->getCannonPosition());
      }
      break;
    case 'f':
      EXIT_FLAG = 1;
      break;
    }
  }
  return;
}

void drawPositionedItems(Platform *plat) {
  while (true) {
    clear();

    shipsMtx.lock();
    for (auto &ship : ships) {
      mvprintw(ship.getPosition().y, ship.getPosition().x, "N");
    }
    shipsMtx.unlock();
    mvprintw(18, 0, "plat x: %i", plat->getPosition().x);
    mvprintw(19, 0, "plat y: %i", plat->getPosition().y);
    mvprintw(20, 0, "bullets: %i", static_cast<int>(bullets.size()));
    mvprintw(21, 0, "plat bullets: %i", plat->getBullets());

    bulletsMtx.lock();
    for (auto &bullet : bullets) {
      mvprintw(22, 20, "y: %i", bullet.getPosition()->y);
      mvprintw(23, 20, "x: %i", bullet.getPosition()->x);
      mvprintw(20, 30, "a");

      mvprintw(bullet.getPosition()->y, bullet.getPosition()->x, "O");
    }
    bulletsMtx.unlock();

    drawPlataform(plat);

    refresh();
    usleep(16667);
  }
}

void drawPlataform(Platform *plat) {
  switch (plat->getCannonPosition()) {
  case CannonPosition::LEFT:
    mvprintw(plat->getPosition().y, plat->getPosition().x, "ESQUERDA");
    break;
  case CannonPosition::MIDDLE_LEFT:
    mvprintw(plat->getPosition().y, plat->getPosition().x, "MEIO ESQUERDA");
    break;
  case CannonPosition::UP:
    mvprintw(plat->getPosition().y, plat->getPosition().x, "CIMA");
    break;
  case CannonPosition::MIDDLE_RIGHT:
    mvprintw(plat->getPosition().y, plat->getPosition().x, "MEIO DIREITA");
    break;
  case CannonPosition::RIGHT:
    mvprintw(plat->getPosition().y, plat->getPosition().x, "DIREITA");
    break;

  default:
    break;
  }
}

void positionControl() {
  int i = 0;
  while (true) {
    i = 0;
    shipsMtx.lock();
    for (auto &ship : ships) {
      ship.moveDown();
      if (ship.getPosition().y == MAX_HEIGHT) {
        delete &ships.at(i);
        ships.erase(ships.begin() + i);
      }
      i++;
    }
    shipsMtx.unlock();

    bulletsMtx.lock();
    i = 0;
    for (auto &bullet : bullets) {
      if (!bullet.isAlive()) {
        bullets.erase(bullets.begin() + i);
        continue;
      };

      bullet.move();

      if (bullet.getPosition()->y <= 0 ||
          bullet.getPosition()->y > MAX_HEIGHT ||
          bullet.getPosition()->x <= 0 ||
          bullet.getPosition()->x >= MAX_WIDTH) {
        bullet.kill();
      }

      i++;
    }
    bulletsMtx.unlock();

    usleep(100000);
  }
}

void generateShips() {
  while (true) {
    Position shipPosition = {rand() % MAX_WIDTH, 0};
    shipsMtx.lock();
    ships.emplace_back(shipPosition, 3, 1);
    shipsMtx.unlock();
    sleep(3);
  }
}

void gameEngine() { 
  std::unique_lock<std::mutex> lock(queueMutex);
  

  

  lock.unlock()
  
  return;

}
