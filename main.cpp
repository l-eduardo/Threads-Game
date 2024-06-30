#include "bullet.h"
#include "kbreader.h"
#include "platform.h"
#include "ship.h"
#include "threadManager.h"
#include <mutex>
#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <shared_mutex>

std::vector<Bullet> bullets;
std::vector<Ship> ships;
std::mutex rechargeMtx;
std::mutex shipsMtx;
std::mutex bulletsMtx;
std::shared_mutex gameEndMtx;
std::shared_mutex playerPointsMtx;
std::shared_mutex shipPointsMtx;
std::unordered_map<char, std::thread> threads;

int MAX_HEIGHT, MAX_WIDTH;
unsigned long ONE_SEC_IN_MICROSEC = 1000000;
unsigned int FRAME_PER_SECOND = 60;
unsigned int FRAME_TIME = ONE_SEC_IN_MICROSEC / FRAME_PER_SECOND;
unsigned long POSITION_ACTUALIZATION_TIME = 100000;

unsigned int EXIT_FLAG = 0;
unsigned int GAME_END = 0;

char DIFFICULTY = 'e';

unsigned int SHIP_QUANTITY;
unsigned int SHIP_SPEED;
unsigned int SHIP_LIFE = 3;
unsigned int SHIP_GENERATE_INTERVAL = 3;
unsigned int PLAYER_BULLETS = 10;

unsigned int SHIP_POINTS = 0;
unsigned int PLAYER_POINTS = 0;
unsigned int POINTS_TARGET;

void drawPositionedItems(Platform* plat);
void positionControl();
void generateShips();
void readKeyboard(KeyboardReader kbReader, Platform* plat, ThreadManager* threadManager);
void drawPlataform(Platform* plat);
void collisionControl();
void openMenu();
void gameFinished();
int isTimeToFinish();

int main() {
  openMenu();
  initscr(); // Inicializa a tela ncurses
  noecho();  // Não ecoa as teclas digitadas
  curs_set(FALSE);       // Esconde o cursor
  cbreak();
  curs_set(0);

  getmaxyx(stdscr, MAX_HEIGHT, MAX_WIDTH);

  Platform plataform({ MAX_WIDTH / 2, MAX_HEIGHT - 1 }, PLAYER_BULLETS, rechargeMtx);
  KeyboardReader kbreader = KeyboardReader();
  ThreadManager threadManager;

  std::thread generateShipsThread(generateShips);
  std::thread positionControlThread(positionControl);
  std::thread drawPositionedItemsThread(drawPositionedItems, &plataform);
  std::thread readKeyboardThread(readKeyboard, kbreader, &plataform, &threadManager);
  std::thread gameIsFinishedThread(gameFinished);


  generateShipsThread.join();
  positionControlThread.join();
  drawPositionedItemsThread.join();
  readKeyboardThread.join();
  gameIsFinishedThread.join();

  return 0;
}

void readKeyboard(KeyboardReader kbReader, Platform* plat, ThreadManager* threadManager) {
  while (isTimeToFinish() == 0) {
    char c = kbReader.readKey();
    switch (c) {
    case 'a':
      threadManager->startAction("a", [&plat]() { plat->setCannonPosition(CannonPosition::LEFT); });
      break;
    case 'q':
      threadManager->startAction("q", [&plat]() { plat->setCannonPosition(CannonPosition::MIDDLE_LEFT); });
      break;
    case 'w':
      threadManager->startAction("w", [&plat]() { plat->setCannonPosition(CannonPosition::UP); });
      break;
    case 'e':
      threadManager->startAction("e", [&plat]() { plat->setCannonPosition(CannonPosition::MIDDLE_RIGHT); });
      break;
    case 'd':
      threadManager->startAction("d", [&plat]() { plat->setCannonPosition(CannonPosition::RIGHT); });
      break;
    case 'r':
      threadManager->startAction("r", [&plat]() { plat->recharge(); });
      break;
    case ' ':
      threadManager->startAction(" ", [&plat]() {
        auto shoot = plat->shoot();
        if (shoot != nullptr) {
          bulletsMtx.lock();
          bullets.emplace_back(shoot->getSpeed(), shoot->getPosition(), plat->getCannonPosition());
          bulletsMtx.unlock();
        }
        });
      break;
    case 'f':
      EXIT_FLAG = 1;
      break;
    }
  }
}

void drawPositionedItems(Platform* plat) {
  while (isTimeToFinish() == 0) {
    clear();

    shipsMtx.lock();
    for (auto& ship : ships) {
      int shipWidth = ship.getWidth();
      int shipHeight = ship.getHeight();
      for (int y = ship.getPosition().y; y < ship.getPosition().y + shipHeight; y++) {
        for (int x = ship.getPosition().x; x < ship.getPosition().x + shipWidth; x++) {
          mvprintw(y, x, "0");
        }
      }
      mvprintw(ship.getPosition().y - 1, ship.getPosition().x, "ooo");
      mvprintw(ship.getPosition().y - 2, ship.getPosition().x, ". .");
      mvprintw(ship.getPosition().y - 3, ship.getPosition().x, " . ");


    }
    shipsMtx.unlock();

    mvprintw(MAX_HEIGHT-2, 0, "Active Bullets: %i", static_cast<int>(bullets.size()));
    mvprintw(MAX_HEIGHT-1, 0, "Platform Bullets: %i", plat->getBullets());
    
    std::shared_lock<std::shared_mutex> shipPointsLock(shipPointsMtx);
    mvprintw(MAX_HEIGHT-2, MAX_WIDTH-45, "Enemy Points: %i/%i", SHIP_POINTS, POINTS_TARGET);
    shipPointsLock.unlock();

    std::shared_lock<std::shared_mutex> playerPointsLock(playerPointsMtx);
    mvprintw(MAX_HEIGHT-2, MAX_WIDTH-20, "Player Points: %i/%i", PLAYER_POINTS, POINTS_TARGET);
    playerPointsLock.unlock();


    bulletsMtx.lock();
    for (auto& bullet : bullets) {
      mvprintw(22, 20, "y: %i", bullet.getPosition()->y);
      mvprintw(23, 20, "x: %i", bullet.getPosition()->x);
      mvprintw(20, 30, "a");

      mvprintw(bullet.getPosition()->y, bullet.getPosition()->x, "O");
    }
    bulletsMtx.unlock();

    if (rechargeMtx.try_lock()) {
      rechargeMtx.unlock();
    }
    else {
      mvprintw(24, 0, "RECARREGANDO");
    }

    drawPlataform(plat);

    refresh();
    usleep(FRAME_TIME);
  }
}

void drawPlataform(Platform* plat) {
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

void collisionControl()
{
  shipsMtx.lock();
  for (auto& ship : ships)
  {
    bulletsMtx.lock();
    for (auto& bullet : bullets)
    {
      if ((ship.getPosition().x <= bullet.getPosition()->x && ship.getPosition().x + ship.getWidth() >= bullet.getPosition()->x) &&
         (ship.getPosition().y <= bullet.getPosition()->y && ship.getPosition().y + ship.getHeight() >= bullet.getPosition()->y))
      {
        ship.damage(1);
        bullet.kill();

        if (!ship.isAlive())
        {
          std::unique_lock<std::shared_mutex> playerPointsLock(playerPointsMtx);
          PLAYER_POINTS++;
          playerPointsLock.unlock();
        }
      }
    }
    bulletsMtx.unlock();
  }
  shipsMtx.unlock();
}

void openMenu() {
  std::cout << "=== Set Dificulty ===" << std::endl;
  std::cout << "1 - Easy" << std::endl;
  std::cout << "2 - Medium" << std::endl;
  std::cout << "3 - Hard" << std::endl;
  
  switch (getchar()) {
    case '1':
      DIFFICULTY = 'e';
      SHIP_QUANTITY = 20;
      SHIP_SPEED = 1;
      SHIP_GENERATE_INTERVAL = 4;
      SHIP_LIFE = 1;
      break;
    case '2':
      DIFFICULTY = 'm';
      SHIP_QUANTITY = 30;
      SHIP_SPEED = 1;
      POSITION_ACTUALIZATION_TIME = 85000;
      SHIP_GENERATE_INTERVAL = 2;
      SHIP_LIFE = 2;
      PLAYER_BULLETS = 15;
      break;
    case '3':
      DIFFICULTY = 'h';
      SHIP_QUANTITY = 40;
      SHIP_SPEED = 1;
      SHIP_LIFE = 3;
      POSITION_ACTUALIZATION_TIME = 70000;
      SHIP_GENERATE_INTERVAL = 1;
      PLAYER_BULLETS = 25;
      break;
    default:
      exit(0);
  }

  POINTS_TARGET = SHIP_QUANTITY / 2;
}

void positionControl() {
  int i = 0;
  while (isTimeToFinish() == 0) {
    i = 0;
    shipsMtx.lock();
    for (auto& ship : ships) {
      ship.moveDown();
      if (ship.getPosition().y == MAX_HEIGHT || !ship.isAlive()) {
        ships.erase(ships.begin() + i);
        std::unique_lock<std::shared_mutex> shipPointsLock(shipPointsMtx);
        SHIP_POINTS++;
        shipPointsLock.unlock();
      }
      i++;
    }
    shipsMtx.unlock();

    bulletsMtx.lock();
    i = 0;
    for (auto& bullet : bullets) {
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

    collisionControl();

    usleep(POSITION_ACTUALIZATION_TIME);
  }
}

void generateShips() {
  while (isTimeToFinish() == 0) {
    Position shipPosition = { rand() % (MAX_WIDTH-3), 0 };
    shipsMtx.lock();
    ships.emplace_back(shipPosition, SHIP_LIFE, SHIP_SPEED);
    shipsMtx.unlock();
    sleep(SHIP_GENERATE_INTERVAL);
  }
}

void gameFinished() {
  while (true)
  {
    std::shared_lock<std::shared_mutex> playerLock(playerPointsMtx);
    if (PLAYER_POINTS >= POINTS_TARGET)
    {
      std::unique_lock<std::shared_mutex> gameEndLock(gameEndMtx);
      GAME_END = 1;
      gameEndLock.unlock();
      clear();
      sleep(2);
      refresh();
      mvprintw(MAX_HEIGHT / 2, MAX_WIDTH / 2, "YOU WIN!");
      mvprintw(MAX_HEIGHT / 2 + 69, MAX_WIDTH / 2 - 2, "  |");
      mvprintw(MAX_HEIGHT / 2 + 1, MAX_WIDTH / 2 - 2, "  / \\");
      mvprintw(MAX_HEIGHT / 2 + 2, MAX_WIDTH / 2 - 2, " / _ \\");
      mvprintw(MAX_HEIGHT / 2 + 3, MAX_WIDTH / 2 - 2, "| / \\ |");
      mvprintw(MAX_HEIGHT / 2 + 4, MAX_WIDTH / 2 - 2, "| \\_/ |");
      mvprintw(MAX_HEIGHT / 2 + 5, MAX_WIDTH / 2 - 2, " \\   /");
      mvprintw(MAX_HEIGHT / 2 + 6, MAX_WIDTH / 2 - 2, "  \\_/");
      mvprintw(MAX_HEIGHT / 2 + 69, MAX_WIDTH / 2 - 2, "   |");
      refresh();
      sleep(2);
      endwin();
      exit(0);
    }
    playerLock.unlock();    

    std::shared_lock<std::shared_mutex> shipLock(shipPointsMtx);
    if (SHIP_POINTS >= POINTS_TARGET)
    {
      std::unique_lock<std::shared_mutex> gameEndLock(gameEndMtx);
      GAME_END = 1;
      gameEndLock.unlock();
      clear();
      sleep(2);
      refresh();
      mvprintw(MAX_HEIGHT / 2, MAX_WIDTH / 2, "YOU LOSE!");
      mvprintw((MAX_HEIGHT / 2) + 1,  MAX_WIDTH / 2, "    .-~~\\");
      mvprintw((MAX_HEIGHT / 2) + 1 + 1,  MAX_WIDTH / 2, "   /     \\ \\");
      mvprintw((MAX_HEIGHT / 2) + 1 + 2,  MAX_WIDTH / 2, "  /       \\ \\");
      mvprintw((MAX_HEIGHT / 2) + 1 + 3,  MAX_WIDTH / 2, " |   _   _ | |");
      mvprintw((MAX_HEIGHT / 2) + 1 + 4,  MAX_WIDTH / 2, " |  | |_| |  |");
      mvprintw((MAX_HEIGHT / 2) + 1 + 5,  MAX_WIDTH / 2, " |  \\     /  |");
      mvprintw((MAX_HEIGHT / 2) + 1 + 6,  MAX_WIDTH / 2, "  \\  ~~~~  /");
      mvprintw((MAX_HEIGHT / 2) + 1 + 7,  MAX_WIDTH / 2, "   \\_____/");
      refresh();
      sleep(2);
      endwin();
      exit(0);
    }
    shipLock.unlock();
    
    if (EXIT_FLAG){
      clear();
      refresh();
      endwin();
      exit(0);
    }
  }
  
  
  clear();
  mvprintw(MAX_HEIGHT / 2, MAX_WIDTH / 2, "GAME OVER");
  refresh();
  sleep(2);
  endwin();
  exit(0);
}

int isTimeToFinish()
{
  std::shared_lock<std::shared_mutex> gameEndLock(gameEndMtx);
  if(GAME_END == 1)
    return 1;
  return 0;
}
