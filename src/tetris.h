/**
 * Programarea Calculatoarelor 2011-2012
 * Tema #3 - Joc de Tetris
 *
 * Schelet pentru Tetris
 *
 * Autor: Adrian Scoica <adrian.sc@rosedu.org>
 *
 */

#ifndef __TETRIS_H__
#define __TETRIS_H__

#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_KEY_PRESSED (-1)

/**
 * Functia intoarce codul caracterului apasat sau NO_KEY_PRESSED
 * daca niciun caracter nu a fost apasat.
 */
int get_key_pressed() {
  int ch = getch();
  if (ch != ERR) {
    return ch;
  }
  return NO_KEY_PRESSED;   
}

typedef struct{
	int row, col;
	}	punct;

typedef struct{
	int score;
	char name[40];
} performer;
#endif

