#include "kbreader.h"
#include "ship.h"
#include "platform.h"
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

int main() {
    int max_width = 64;
    int max_height = 32;
    Position posShip = {5, 0};
    Ship ship1 = {posShip, 3, 1};
    Position posPlat = {max_height, max_width/2};
    Platform plat = {posPlat, 7};

    initscr();             // Inicializa a tela ncurses
    keypad(stdscr, TRUE);  // Habilita a leitura de teclas de função como as setas
    noecho();              // Não ecoa as teclas digitadas
    curs_set(FALSE);       // Esconde o cursor

    while (true)
    {
        clear();            // Limpa a tela
        mvprintw(ship1.getPosition().y, ship1.getPosition().x, "O0O0O0O0O0O0"); // Move o caractere para a nova posição
        mvprintw(plat.getPosition().y, plat.getPosition().x, "PLATAFORMA"); // Move o caractere para a nova posição
        ship1.moveDown();     // Move o personagem para cima
        refresh();          // Atualiza a tela
        sleep(1);         // Espera 120ms
    }
    return 0;
}