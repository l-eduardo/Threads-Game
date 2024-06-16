#include "kbreader.h"
#include "ship.h"
#include <iostream>
#include <ncurses.h>
#include <unistd.h>

int main() {
    Position pos = {5, 0};
    Ship ship1 = {pos, 3, 1};

    initscr();             // Inicializa a tela ncurses
    keypad(stdscr, TRUE);  // Habilita a leitura de teclas de função como as setas
    noecho();              // Não ecoa as teclas digitadas
    curs_set(FALSE);       // Esconde o cursor

    while (true)
    {
        clear();            // Limpa a tela
        mvprintw(ship1.getPosition().y, ship1.getPosition().x, "O0O0O0O0O0O0"); // Move o caractere para a nova posição
        ship1.moveDown();     // Move o personagem para cima
        refresh();          // Atualiza a tela
        sleep(1);         // Espera 120ms
    }
    return 0;
}