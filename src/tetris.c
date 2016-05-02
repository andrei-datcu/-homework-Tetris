/**
 * Programarea Calculatoarelor 2011-2012
 * Tema #3 - Joc de Tetris
 *
 * Autor: Datcu Andrei
 *
 */

#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tetris.h"
#include <time.h>
#include <sys/time.h>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#define NRLE 20
#define NRCE 20
#define NRLT 20
#define NRCT 10
#define PREVL 1
#define PREVC 25
#define TRESH 1000
#define INTMIN 100
#define BLOCKSIZE 237

int muta_piesa(punct dest, const char (*piesa)[4][4], char (*tabla)[NRLT][NRCT])
	{
	char i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if ( (((*piesa)[i][j]) && ((i + dest.row >= NRLT) || (j + dest.col < 0) 
				|| (j + dest.col >= NRCT))) || ( (i + dest.row >= 0) && ((*tabla)[i + dest.row][j + dest.col] * (*piesa)[i][j])) )
					return 0; //nu pot sa deplasez piesa la dest
	return 1;
	}

void deseneaza_piesa(punct dest, const char (*piesa)[4][4], char color)
	{
	char i, j, l, c, done;
	int print1, print2;
	print1 = color? ' ' : ACS_CKBOARD;
	print2 = print1;
		for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if ( ((*piesa)[i][j]) && (i+ dest.row >= 0)  )		
			{
				l = i + dest.row;
				c = j + dest.col;
				attron(COLOR_PAIR(color));
				move(l+1, 2 * c + 1);
				addch(print1);
				addch(print2);//desenez 2 caractere alaturate pentru un patrat din piesa
				attroff(COLOR_PAIR(color));
			}
		refresh();
	}

void sterge_piesa (punct dest, const char (*piesa)[4][4])
	{
	char i, j, l, c, done;
	int print1, print2;
		for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if ( ((*piesa)[i][j]) && (i + dest.row >= 0) )		
			{
				l = i + dest.row;
				c = j + dest.col;
				print1 = print2 = ' ';
				move(l+1, 2 * c + 1);
				addch(print1);
				addch(print2);
			}
		refresh();
	}

int scor(char *nrl, char (*tabla)[NRLT][NRCT], int level, Mix_Chunk *collapse, int collapse_ch, Mix_Chunk *tetris, int tetris_ch)
{
	int pct = 0;
	char i, j, nrl2 = 0, ok, fl = 0;
	for (i = 0; i < NRLT; i++)
	for (j = 0; j < NRCT; j++)
		if ((*tabla)[i][j])
		{
		nrl2++;
		break;
		}
	pct = 4 - (nrl2 - *nrl); //daca nu au scazut numarul de linii  acord 4 puncte
	*nrl = nrl2;
	if (nrl2 >= NRLT)
		return 0;
	if (nrl2 == 0)
		return TRESH;
	for (i = 0; i < NRLT; i++)
	{
		ok = 1;
		for (j = 0; j < NRCT; j++)
			if ((*tabla)[i][j] == 0)
			{
				ok = 0;
				break;
			}
		if (ok)//linie completa?
		{
			fl++;
			memset( &((*tabla)[0]), 0, sizeof((*tabla)[0]));//linia de sus devine goala
			for (; i >= 0; i--)
			{
				if (i)
					memcpy( &((*tabla)[i]), &((*tabla)[i - 1]), sizeof((*tabla)[i]));//mult liniile cu 1 pozitie in jos
				move(i + 1, 1);
				for (j = 0; j < NRCT; j++) //si desenez linia
				{
					attron(COLOR_PAIR((*tabla)[i][j]));
					addch(' ');
					addch(' ');
					attroff(COLOR_PAIR((*tabla)[i][j]));
				}
			}
			pct += 10 * level; //fiecarie linie completa valoareaza 10 * nivel
		}
	}
	if (fl == 4) //tetris?
	{
		pct += 10 * level * 4; //dublez punctajul pe linii
		Mix_PlayChannel(tetris_ch, tetris, 0);
	}
	else
		if (fl)//cel putin o linie disparuta
			Mix_PlayChannel(collapse_ch, collapse, 0);
	return pct;
}

void print_preview(const char (*piesa)[4][4], char color)
{
	char i, j;
	move(PREVL, PREVC);
	for (i = 0; i < 4; i++)
	{
		move (i + PREVL, PREVC);
		for (j = 0; j < 4; j++)
		{
			attron(COLOR_PAIR(color * (*piesa)[i][j]));
			addch(' ');
			addch(' ');
			attroff(COLOR_PAIR(color * (*piesa)[i][j]));
		}
	}
	refresh();
}

void highscores(WINDOW *wnd, int score) //-1 vreau doar sa vad
{
	Mix_Chunk *hs = Mix_LoadWAV("highscores.wav");
	Mix_PlayChannel(6, hs, -1);
	FILE *fin = fopen("highscores.dat", "r");
	performer a[10], b[10];
	int n = 0, i, j;
	if (fin)
	fscanf(fin, "%d", &n);
	for (i = 0; i < n; i++)
		fscanf(fin, "%d%s", &(a[i].score), a[i].name);
	if (fin)
		fclose(fin);
	clear();
	if (score > 0)
		mvprintw(0, 10, "FELICITARI! Ati obtinut %d puncte", score);
	init_pair(9, COLOR_RED, COLOR_BLACK);
	init_pair(10, COLOR_GREEN, COLOR_BLACK);
	attron(COLOR_PAIR(10));
	mvaddstr(2, 15, "S C O R U R I     R E C O R D");
	attroff(COLOR_PAIR(10));
	attron(COLOR_PAIR(9));
	mvaddstr(4, 4, "Poz.");
	mvaddstr(4, 9, "Nume");
	mvaddstr(4, 48, "Scor");
	attroff(COLOR_PAIR(9));
	if (score <= 0)
		{
		nodelay(wnd, FALSE);
		for (i = 0; i < n; i++)
			mvprintw(5 + i, 5, "%2d. %-41s %d", i + 1, a[i].name, a[i].score);
		printw("\n\n Apasati o tasta pentru a revini la meniul principal");
		getch();
		refresh();
		Mix_HaltChannel(6);
		Mix_FreeChunk(hs);
		return;
		}
	for (i = 0; (i < n) && (a[i].score > score); i++)
		memcpy(&(b[i]), &(a[i]), sizeof(performer));
	b[i].name[0] = 0;
	b[i].score = score;
	j = i;
	for (; i < n; i++)
		memcpy(&(b[i + 1]), &(a[i]), sizeof(performer));
	if (++n > 10)
		n = 10;
	nodelay(wnd, FALSE);
	for (i = 0; i < n; i++)
		mvprintw(5 + i, 5, "%2d. %-41s %d", i + 1, b[i].name, b[i].score);
	if (j <= 9)
	{
		int cs =  9;
		move(5 + j, cs );
		curs_set(1);
		int c = 0;
		i = 0;
		for (c = getch(); (c != 10) || (i == 0); c = getch())
		{
			switch (c)
			{
			case KEY_BACKSPACE: 	if (i)
						{
							b[j].name[i--] = 0;
							move(5 + j, cs + i);
							addch(' ');
							move(5 + j, cs + i);
						}
						break;
			default:	if ( (c >= 32) && (c <= 'z') && (i < 40) )
					{
						b[j].name[i++] = c;
						addch(c);
					}
			}
		}
	b[j].name[i] = 0;
	curs_set(0);
	}
	FILE *fout = fopen ("highscores.dat", "w");
	fprintf(fout, "%d\n", n);
	for (i = 0; i < n; i++)
		fprintf(fout, "%d %s\n", b[i].score, b[i].name);
	fclose(fout);
	mvprintw(5 + n + 2, 5, "Apasati o tasta pentru a revini la meniul principal");
	refresh();
	getch();
	Mix_HaltChannel(6);
	Mix_FreeChunk(hs);

}
 

int print_saves(WINDOW *wnd, char loading)
{
	FILE * fin = fopen("savegame.dat", "rb");
	int c, i;
	char taken = 5;
	time_t data[10] = {0};
	clear();//213
	refresh();
	mvaddstr(25, 5,"Pentru a reveni la meniul anterior apasati q");
	mvaddstr(3, 5, "Alegeti un slot");
	for (i = 0; i < 10; i++)
	{
		fseek(fin, i * BLOCKSIZE, SEEK_SET);
		//taken = 0;
		fread(&taken, 1, 1, fin); //este save game pe slotul i?
		if (taken)
		{
			fseek(fin, 212, SEEK_CUR);
			fread(&data[i], sizeof(time_t), 1, fin);//citesc data salvarii
		}
		mvprintw(5 + i, 5, "%2d. %s", i + 1, taken? ctime(&data[i]) : "Slot liber");
	}
	fclose(fin);
	refresh();
	nodelay(wnd, FALSE);
	init_pair(12, COLOR_WHITE, COLOR_BLUE);
	attron(COLOR_PAIR(12));
	i = 0;
	mvprintw(5 + i, 5, "%2d. %s", i + 1, data[i]? ctime(&data[i]) : "Slot liber");
	attroff(COLOR_PAIR(12));
	Mix_Chunk *mv = Mix_LoadWAV("menumove.wav");
	for (c = getch(); (c != 10) || ( (loading) && (data[i] == 0)); c = getch())
		switch(tolower(c))
		{
			case KEY_UP: Mix_PlayChannel(1, mv, 0);
				 mvprintw(5 + i, 5, "%2d. %s", i + 1, data[i]? ctime(&data[i]) : "Slot liber");
				 i = (i + 9) % 10;
				 attron(COLOR_PAIR(12));
				 mvprintw(5 + i, 5, "%2d. %s", i + 1, data[i]? ctime(&data[i]) : "Slot liber");
				 attroff(COLOR_PAIR(12));
				 break;

			case KEY_DOWN: Mix_PlayChannel(1, mv, 0); 
				 mvprintw(5 + i, 5, "%2d. %s", i + 1, data[i]? ctime(&data[i]) : "Slot liber");
				 i = (i + 1) % 10;
				 attron(COLOR_PAIR(12));
				 mvprintw(5 + i, 5, "%2d. %s", i + 1, data[i]? ctime(&data[i]) : "Slot liber");
				 attroff(COLOR_PAIR(12));
				 break;
			case 'q': return -1;
		}
	Mix_HaltChannel(1);
	Mix_FreeChunk(mv);
	return i;
}

	

void save_game(WINDOW *wnd, punct poz, char p1, char p2, char c1, char c2, char (*tabla)[NRLT][NRCT], time_t data, time_t timp, int points, int level)
{		
	int slot = print_saves(wnd, 0);
	FILE * fout = fopen("savegame.dat", "r+b");
	if (slot < 0)
		return; //nu am ales un slot valid
	fseek(fout, slot * BLOCKSIZE, SEEK_SET);
	char i = 1;
	fwrite(&i, 1, 1, fout);
	fwrite(tabla, sizeof(*tabla), 1, fout);
	fwrite(&poz, sizeof(punct), 1, fout);
	fwrite(&p1, 1, 1, fout);
	fwrite(&c1, 1, 1, fout);
	fwrite(&p2, 1, 1, fout);
	fwrite(&c2, 1, 1, fout);
	fwrite(&data, sizeof(time_t), 1, fout);
	fwrite(&timp, sizeof(time_t), 1, fout);
	fwrite(&points, sizeof(int), 1,fout);
	fwrite(&level, sizeof(int), 1, fout);
	fclose(fout);
}

char load_game(char slot, punct *poz, char *p1, char *p2, char* c1, char *c2, char (*tabla)[NRLT][NRCT], time_t *data, time_t *timp, int *points, int *level)
{
	FILE * fin = fopen("savegame.dat", "rb"); 
	fseek(fin, slot * BLOCKSIZE + 1, SEEK_SET);
	fread(tabla, sizeof(*tabla), 1, fin);
	fread(poz, sizeof(punct), 1, fin);
	fread(p1, 1, 1, fin);
	fread(c1, 1, 1, fin);
	fread(p2, 1, 1, fin);
	fread(c2, 1, 1, fin);
	fread(data, sizeof(time_t), 1, fin);
	fread(timp, sizeof(time_t), 1, fin);
	fread(points, sizeof(int), 1,fin);
	fread(level, sizeof(int), 1, fin);
	fclose(fin);
	char i, j, ok, nrl = 0;
	for (i = 0; i < NRLT; i++)
	{
		ok = 0;
		for (j = 0; j < NRCT; j++)
			if ((*tabla)[i][j])
			{
				ok = 1;
				attron(COLOR_PAIR((*tabla)[i][j]));
				mvaddstr(i + 1, 2 * j + 1,"  ");
				attroff(COLOR_PAIR((*tabla)[i][j]));
			}
	if (ok)
		nrl++;
	}
	return nrl;
}

int can_reach(punct* start, punct *finish, const char (*piesa)[4][4], char (*tabla)[NRLT][NRCT])
{//parcurgere bf pentru a vedea daca se poate ajunge de la start la finish
	const char dl[3] = {0, 0, 1}, dc[3] = {1, -1, 0};
	char viz[NRLT][NRCT] = {{0}};
	punct c[NRLT * NRCT + 1], go;
	int p = 0, u = 0, k;
	memcpy(&c[p], start, sizeof (punct));
	for (p = 0; p <= u; p++)
		for (k = 0; k < 3; k++)
		{
			go.row = c[p].row + dl[k];
			go.col = c[p].col + dc[k];
			if ((viz[go.row][go.col] == 0) && (muta_piesa(go, piesa, tabla)))
			 {
				 u++;
				 memcpy(&c[u], &go, sizeof(punct));
				 viz[go.row][go.col] = 1;
				 if (memcmp(&go, finish, sizeof(punct)) == 0)
					 return 1;
			 }
		}
	return 0;
}

punct calc_shadow(punct *poz, const char (*piesa)[4][4], char (*tabla)[NRLT][NRCT])
{
	punct go;
	memcpy(&go, poz, sizeof(punct));
	go.row++;
	while (muta_piesa(go, piesa, tabla))
		go.row++;
	go.row--;
	return go;
}

void new_game(WINDOW *wnd, int level, char loading)
{
	int i, j, key;
	char tabla[20][10] = {{0}}, ghost_mode = 0, shadow_mode = 1;
	const char piese[7][4][4][4] = {{ { {0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0} }}, 
		{ { {0}, {1, 1, 1, 1}, {0}, {0} }, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0} }},
		{ { {0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0}}, { {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {0}}},
		{ { {0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0}}, { {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0}}},
		{ {{0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0}}, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 1}, {0}},
		{{0, 0, 0, 1}, {0, 1, 1, 1}, {0}, {0}}, {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0}}},
		{ {{0}, {0, 1, 1, 1,}, {0, 0, 0, 1}, {0}}, {{0, 0, 1, 1}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0}},
		{{0, 1, 0, 0}, {0, 1, 1, 1}, {0}, {0}}, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0}}},
		{ {{0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0}}, {{0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1,0}, {0}},
		{{0, 0, 1, 0}, {0, 1, 1, 1}, {0}, {0}}, {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0}}}};
	const char rot[7] = {1, 2, 2, 2, 4, 4, 4};

	clear();
	// desenez tabla de joc
	init_pair(7, COLOR_WHITE, COLOR_WHITE);
	attron(COLOR_PAIR(7));
	move(0, 0);
	addch(ACS_ULCORNER);
	move(0, NRCE + 1);
	addch(ACS_URCORNER);
	for (i = 1; i < NRLE + 1; i++)
	{
		move(i, 0);
		addch(ACS_VLINE);
		move(i, NRCE + 1);
		addch(ACS_VLINE);
	}
	move(NRLE + 1, 0);
	addch(ACS_LLCORNER);
	move(NRLE + 1, NRCE + 1);
	addch(ACS_LRCORNER);
	for (j = 1; j < NRCE + 1; j++)
	{
		move(0, j);
		addch(ACS_HLINE);
		move(NRLE + 1, j);
		addch(ACS_HLINE);	
	}
	attroff(COLOR_PAIR(7));
	move(PREVL - 1 , PREVC - 1);
	addch(ACS_ULCORNER);
	move(PREVL - 1, PREVC + 10);
	addch(ACS_URCORNER);
	for (i = PREVL; i < PREVL + 5; i++)
	{
		move(i, PREVC - 1);
		addch(ACS_VLINE);
		move(i, PREVC + 10);
		addch(ACS_VLINE);
	}
	move(PREVL + 5, PREVC - 1);
	addch(ACS_LLCORNER);
	move(PREVL + 5, PREVC + 10);
	addch(ACS_LRCORNER);
	for (j = PREVC; j < PREVC + 10; j++)
	{
		move(PREVL - 1, j);
		addch(ACS_HLINE);
		move(PREVL + 5, j);
		addch(ACS_HLINE);	
	}
	time_t start_time = 0;
	struct timeval current_time, lastfall, lastupdate;
	int points = 0;
	char p1, p2,c1, c2, r, nrl = 0, r_ghost = 0;
	punct poz, poz_ghost, poz_shadow;
	srand(time(NULL));
	p1 = rand() % 7; c1 = rand() % 6 + 1; r = 0;
	poz.row = -4;
	poz.col = 3;
	p2 = rand() % 7; c2 = rand() % 6 + 1;
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE,COLOR_GREEN);
	init_pair(3, COLOR_WHITE,COLOR_YELLOW);
	init_pair(4, COLOR_WHITE,COLOR_BLUE);
	init_pair(5, COLOR_WHITE,COLOR_MAGENTA);
	init_pair(6, COLOR_WHITE,COLOR_CYAN);
	if (loading >= 0)
	nrl = load_game(loading, &poz, &p1, &p2, &c1, &c2, &tabla, &(current_time.tv_sec), &start_time, &points, &level);
	memset(&current_time, 0, sizeof(struct timeval));
	memset(&lastfall, 0, sizeof(struct timeval));
	memset(&lastupdate, 0, sizeof(struct timeval));
	int interval = 700 - 50 * (level - 1) ;
	if (interval < INTMIN)
	interval = INTMIN;
	poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
	deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
	mvprintw(8, 25, "Nivelul %d", level);
	mvprintw(9, 25, "%d puncte", points);
	mvprintw(12, 25, "Pentru a parasi jocul fara a salva apasati q");
	mvaddstr(13, 25, "Pentru a salva inainte de a parasi jocul apasati s");
	mvaddstr(14, 25, "Pentru a opri/porni sunetul de fundal apasati o");
	mvaddstr(16, 25, "Modul UMBRA:");
	mvaddstr(17, 30, "Pentru a porni/opri modul umbra apasati u");
	mvaddstr(18, 30, "Pentru a aseza piesa in pozitia umbrei apasati v");
	mvaddstr(19, 25, "Modul FANTOMA:");
	mvaddstr(20, 30, "Pentru a porni/opri modul fantoma apasati g");
	mvaddstr(21, 30, "Pentru a aseza piesa in pozitia \"fantomei\" apasati f");
	mvaddstr(22, 30, "Pentru a roti piesa in modul fantoma apasati r");
	mvaddstr(24, 25, "NOTA: Modul UMBRA si modul FANTOMA nu pot fi pornite in acelasi timp");
  		nodelay(wnd, TRUE);
	print_preview(&(piese[p2][0]), c2);
	Mix_Chunk *mv = Mix_LoadWAV("move.wav"), *fix = Mix_LoadWAV("fix.wav"), *rotate = Mix_LoadWAV("rotate.wav"), *sound = Mix_LoadWAV("tetris-gameboy-02.wav"),
		*collapse = Mix_LoadWAV("collapse.wav"), *tetris = Mix_LoadWAV("tetris.wav"), *end = NULL; 
	int mv_ch = 1, fix_ch = 2, rotate_ch = 3, collapse_ch = 4, tetris_ch = 5, end_ch = 6;
	Mix_Volume(0, MIX_MAX_VOLUME / 3);//muzica de fundal
	Mix_Volume(mv_ch, MIX_MAX_VOLUME);
	Mix_Volume(fix_ch, MIX_MAX_VOLUME);
	Mix_Volume(rotate_ch, MIX_MAX_VOLUME);
	Mix_Volume(collapse_ch, MIX_MAX_VOLUME);
	Mix_Volume(tetris_ch, MIX_MAX_VOLUME);
	Mix_Volume(end_ch, MIX_MAX_VOLUME);
	Mix_PlayChannel(0, sound, -1);
	while (nrl < NRLT)
	 {
		 gettimeofday(&current_time, NULL);
		 if ((current_time.tv_sec - lastfall.tv_sec)* 1000 + (current_time.tv_usec - lastfall.tv_usec)/1000 >= interval)
		 {
			 godown:
			 lastfall = current_time;
			 poz.row++;
			 if (muta_piesa(poz, &(piese[p1][r]), &tabla))
			 {
				poz.row--;
				sterge_piesa(poz, &(piese[p1][r]));
				poz.row++;
			 	deseneaza_piesa(poz, &(piese[p1][r]), c1);
				if (ghost_mode)
					deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
			 }
				else //asez piesa
				{
					poz.row--;
					godown2:
					Mix_PlayChannel(fix_ch, fix, 0);
					if (ghost_mode)
						sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
					ghost_mode = 0;
					deseneaza_piesa(poz, &(piese[p1][r]), c1);
					for (i = 0; i < 4; i++)
					for (j = 0; j < 4; j++)
						if ( (piese[p1][r][i][j]) && (i + poz.row >=0) )
							tabla[i + poz.row][j + poz.col] = c1;
					points += scor(&nrl, &(tabla), level, collapse, collapse_ch, tetris, tetris_ch);
					mvprintw(9, 25, "%d puncte             ",points);
					if (points >= level * TRESH)
					{
						level++;
						mvprintw(8, 25, "Nivelul %d", level);
						interval -= 50;
						if (interval < INTMIN)
							interval = INTMIN;
					}
					refresh();
					p1 = p2;
					c1 = c2;
					r = 0;
					poz.row = -3;
					poz.col = 3;
					p2 = rand() % 7;
					c2 = rand() % 6 + 1;
					print_preview(&(piese[p2][0]), c2);
					if (shadow_mode)
					{
						poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
						deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
					}
					goto godown; //au trecut deja interval msec, deci piesa urmatoare trebuie sa cada imediat
				}
					
		 }
		 key = get_key_pressed();
		 switch(tolower(key))
		 {
			 case KEY_LEFT: if (ghost_mode)
			 		{
						poz_ghost.col --;
						if (muta_piesa(poz_ghost, &(piese[p1][r_ghost]), &tabla))
						{
							poz_ghost.col++;
							sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
							deseneaza_piesa(poz, &(piese[p1][r]), c1);//daca piesa avea bucati sub fantoma
							poz_ghost.col--;
							deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
						}
						else
							poz_ghost.col++;
						break;
					}
			 		poz.col--;
				 	if (muta_piesa(poz, &(piese[p1][r]), &tabla))
					{
						poz.col++;
						Mix_PlayChannel(mv_ch, mv, 0);
						sterge_piesa(poz, &(piese[p1][r]));
						poz.col--;
						deseneaza_piesa(poz, &(piese[p1][r]), c1);
						if (shadow_mode)
						{
							sterge_piesa(poz_shadow, &(piese[p1][r]));
							poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
							deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
						}
					}
					else
						poz.col++;
					break;
			 case KEY_RIGHT:if (ghost_mode)
			 		{
						poz_ghost.col ++;
						if (muta_piesa(poz_ghost, &(piese[p1][r_ghost]), &tabla))
						{
							poz_ghost.col--;
							sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
							deseneaza_piesa(poz, &(piese[p1][r]), c1);
							poz_ghost.col++;
							deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
						}
						else
							poz_ghost.col--;
						break;
					}
					poz.col++;
			 		if (muta_piesa(poz, &(piese[p1][r]), &tabla))
					{
						poz.col--;
						Mix_PlayChannel(mv_ch, mv, 0);
						sterge_piesa(poz, &(piese[p1][r]));
						poz.col++;
						deseneaza_piesa(poz, &(piese[p1][r]), c1);
						if (shadow_mode)
						{
							sterge_piesa(poz_shadow, &(piese[p1][r]));
							poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
							deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
						}
					}
					else
						poz.col--;
					break;
			 case KEY_DOWN: if (ghost_mode)
			 		{
						poz_ghost.row ++;
						if (muta_piesa(poz_ghost, &(piese[p1][r_ghost]), &tabla))
						{
							poz_ghost.row--;
							sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
							deseneaza_piesa(poz, &(piese[p1][r]), c1);
							poz_ghost.row++;
							deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
						}
						else
							poz_ghost.row--;
						break;
					}
			 		gettimeofday(&current_time, NULL);
				 	goto godown;
					break;
			 case 'r':	if (ghost_mode)
			 		{
			 			if (muta_piesa(poz_ghost, &(piese[p1][(r_ghost + 1) % rot[p1]]), &tabla))
			 			{
							sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
							deseneaza_piesa(poz, &(piese[p1][r]), c1);
							r_ghost = (r_ghost + 1) % rot[p1];
							deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
						}
					}
					break;
			 case KEY_UP: 	if (ghost_mode)
			 		{
						poz_ghost.row--;
						if ((poz_ghost.row >= 0) && (muta_piesa(poz_ghost, &(piese[p1][r_ghost]), &tabla)))
						{
							poz_ghost.row++;
							sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
							deseneaza_piesa(poz, &(piese[p1][r]), c1);
							poz_ghost.row--;
							deseneaza_piesa(poz_ghost, &(piese[p1][r_ghost]), 0);
						}
						else
							poz_ghost.row++;
						break;
					}
			 		r = (r + 1) % rot[p1];
			 		if (p1 == 0)
						break;
			 		if (muta_piesa(poz, &(piese[p1][r]), &tabla))
					{
						Mix_PlayChannel(rotate_ch, rotate, 0);
						sterge_piesa(poz, &(piese[p1][(r - 1 + rot[p1]) % rot[p1]]));
						deseneaza_piesa(poz, &(piese[p1][r]), c1);
						if (shadow_mode)
						{
							sterge_piesa(poz_shadow, &(piese[p1][(r - 1 + rot[p1]) % rot[p1]]));
							poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
							deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
						}
					}
					else
						r = (r - 1 + rot[p1]) % rot[p1];
					break;
			 case 'p':	nodelay(wnd, FALSE);
				 	mvaddstr(PREVL + 2, PREVC + 15, "PAUZA");
			 		for(key = getch(); (tolower(key) != 'p') && (tolower(key) != 'q'); key = getch());
					nodelay(wnd, TRUE);
					if (tolower(key) == 'q')
				 	{
						quit:
						end = Mix_LoadWAV("gameover.wav");
						Mix_PlayChannel(end_ch, end, 0);
		 				quit2://opresc muzica si eliberez memoria
						Mix_HaltChannel(0);
		 				Mix_HaltChannel(mv_ch);
		 				Mix_HaltChannel(fix_ch);
		 				Mix_HaltChannel(rotate_ch);
		 				Mix_HaltChannel(collapse_ch);
		 				Mix_HaltChannel(tetris_ch);
		 				Mix_FreeChunk(mv);
		 				Mix_FreeChunk(fix);
		 				Mix_FreeChunk(rotate);
		 				Mix_FreeChunk(collapse);
		 				Mix_FreeChunk(tetris);
		 				Mix_FreeChunk(sound);
						if (tolower(key) == 's')
							return;
						while (Mix_Playing(end_ch));//astept sa se termine muzica de end-game
						Mix_FreeChunk(end);
						highscores(wnd, points);
						return;
					}
			 		mvaddstr(PREVL + 2, PREVC + 15, "     ");
					break;
			 case 'q':	goto quit;
				 	break;
			 case 's':	save_game(wnd, poz, p1, p2, c1, c2, &tabla, current_time.tv_sec, start_time, points, level);
			 	 	goto quit2;
					break;
			 case 'g':	ghost_mode = 1 - ghost_mode;
					if (shadow_mode)
					{
						shadow_mode = 0;
						sterge_piesa(poz_shadow, &(piese)[p1][r]);
					}
			 		if (ghost_mode == 0)
					{
						sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
						deseneaza_piesa(poz, &(piese[p1][r]),c1);
						break;
					}
					memcpy(&poz_ghost, &poz, sizeof(punct));
					r_ghost = r;
					deseneaza_piesa(poz_ghost, &(piese[p1][r]), 0);
					break;
			case 'f':	if (ghost_mode == 0)
						break;
					if (can_reach(&poz, &poz_ghost, &(piese[p1][r_ghost]), &tabla) == 0)
						break;
					poz_ghost.row++;
					if (muta_piesa(poz_ghost, &(piese[p1][r_ghost]), &tabla))
					{
						poz_ghost.row--;
						break;
					}
					poz_ghost.row--;
					sterge_piesa(poz, &(piese[p1][r]));
					memcpy(&poz, &poz_ghost, sizeof (punct));
					r = r_ghost;
					goto godown2;
					break;
			case 'v':	if (shadow_mode == 0)
						break;
					sterge_piesa(poz, &(piese)[p1][r]);
					memcpy(&poz, &poz_shadow, sizeof(punct));
					goto godown;
					break;
			case 'u':	shadow_mode = 1 - shadow_mode;
					if (ghost_mode)
					{
						 ghost_mode = 0;
						 sterge_piesa(poz_ghost, &(piese[p1][r_ghost]));
						 deseneaza_piesa(poz, &(piese[p1][r]),c1);
					}	 
					if (shadow_mode == 0)
					{
						sterge_piesa(poz_shadow, &(piese)[p1][r]);
						break;
					}
					poz_shadow = calc_shadow(&poz, &(piese[p1][r]), &tabla);
					deseneaza_piesa(poz_shadow, &(piese[p1][r]), 0);
					break;
			case 'o':	if (Mix_Playing(0))
						Mix_HaltChannel(0);
					else
						Mix_PlayChannel(0, sound, -1);
					break;
		 }
		 if (current_time.tv_sec - lastupdate.tv_sec >= 1)
		 {
			 mvprintw(7, 25, "Ora si data curente: %s",  ctime(&(current_time.tv_sec)));
			 start_time++;
			 mvprintw(10, 25, "Durata jocului: %02d:%02d:%02d", start_time / 3600, (start_time - start_time / 3600 * 3600) / 60, start_time %  60);
			 lastupdate = current_time;
			 refresh();
		 } 
	 }
	 goto quit;

}

void print_menu(punct o1, punct o2, punct o3, punct o4, punct o5, char sel)
{
	clear();
	mvaddstr(o1.row, o1.col, "Joc nou");
	mvaddstr(o2.row, o2.col, "Incarca joc");
	mvaddstr(o3.row, o3.col, "Alege nivelul de pornire");
	mvaddstr(o4.row, o4.col, "Scoruri record");
	mvaddstr(o5.row, o5.col, "Iesi");
	switch (sel){
	case 0: attron(A_BOLD);
		mvaddstr(o1.row, o1.col, "Joc nou");
		attroff(A_BOLD);
		break;
	case 1: attron(A_BOLD);
		mvaddstr(o2.row, o2.col, "Incarca joc");
		attroff(A_BOLD);
		break;
	case 2: attron(A_BOLD);
		mvaddstr(o3.row, o3.col, "Alege nivelul de pornire");
		attroff(A_BOLD);
		break;
	case 3: attron(A_BOLD);
		mvaddstr(o4.row, o4.col, "Scoruri record");
		attroff(A_BOLD);
		break;
	case 4: attron(A_BOLD);
		mvaddstr(o5.row, o5.col, "Iesi");
		attroff(A_BOLD);
		break;
	}
	refresh();
}	

void set_level(int *level)
{
	clear();
	mvaddstr(1, 1, "Alegeti nivelul de pornire:  ");
	printw("%d", *level);
	curs_set(1);
	refresh();
	int c, nr = 1, i = 1;	
	for (c = getch(); (c != 10) || (i == 0); c = getch())
	{
		switch (c)
		{
			case KEY_BACKSPACE: if (i)
				{
					nr /= 10;
					move(1, i + 29);
					addch(' ');
					move(1, i + 29);
					i--;
				}
					 break;
			default: if ( (c >= '0') && (c <= '9') && (i < 2) )
				{
					nr = nr * 10 + c - 48;
					i++;
					addch(c);
				}
		}
	}
	*level = nr;
	curs_set(0);
}


void main_menu(WINDOW *wnd)
{
	char os = -1, slot; //optiunea selectata in main menu
	int c = 0, level = 1;
	punct o1 = {8, 10}, o2 = {10, 10}, o3 = {12, 10}, o4 = {14, 10}, o5 = {16, 10};
	print_menu(o1, o2, o3, o4, o5, os);
	nodelay(wnd, FALSE);
	Mix_Chunk *sound = Mix_LoadWAV("menutheme.wav"), *mv = Mix_LoadWAV("menumove.wav"), *sel = Mix_LoadWAV("menusel.wav");
	Mix_PlayChannel(0, sound, -1);
	while (1)	
	{
		c = getch();
		switch(c){
			case KEY_UP:	Mix_PlayChannel(1, mv, 0);
		 			os = (os + 4) % 5;
		 			break;
			case KEY_DOWN:	Mix_PlayChannel(1, mv, 0);
					os = (os + 1) % 5;
		   			break;
			case 10:	Mix_PlayChannel(2, sel, 0);
					switch(os){
						case 0: Mix_HaltChannel(0);
							new_game(wnd, level, -1);
							Mix_PlayChannel(0, sound, -1);
							os = -1;
							break;
						case 1: slot = print_saves(wnd, 1);
							os = -1;
							if (slot < 0)
								break;
							Mix_HaltChannel(0);
							new_game(wnd, level, slot);
							Mix_PlayChannel(0, sound, -1);
							break;
						case 2: set_level(&level);
							os = 0;
							break;
						case 3: Mix_HaltChannel(0);
							highscores(wnd, -1);
							Mix_PlayChannel(0, sound, -1);
							break;
						case 4: return;
						}
		}	
		print_menu(o1, o2, o3, o4, o5, os);
	}
	Mix_HaltChannel(0);
	Mix_HaltChannel(1);
	Mix_HaltChannel(2);
	Mix_FreeChunk(sound);
	Mix_FreeChunk(mv);
	Mix_FreeChunk(sel);
}


int initsound()
{
	int audio_rate = 22050;		//frecventa
	Uint16 audio_format = AUDIO_S16SYS; 	//.wav-urile sunt pe 16 biti
	int audio_channels = 2;		//stereo
	int audio_buffers = 2048;	//bucata de melodie tinuta in memorie(bytes)
	if (SDL_Init(SDL_INIT_AUDIO) != 0) //initializez SDL
		return 1;
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) //initializez mixer
		return 1;
	return 0;
}

int main()
{
  /* Initializarea ferestrei si determinarea dimensiunilor acesteia. */
	WINDOW* wnd = initscr();
	curs_set(0);
	clear();
	noecho();
	cbreak();
	keypad(wnd, TRUE);
	nodelay(wnd, TRUE);
	start_color();
	initsound();
	main_menu(wnd);
	Mix_CloseAudio();
	SDL_Quit();	
	endwin();
  return 0;
}

