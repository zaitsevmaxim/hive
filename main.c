#define DEBUG
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZEROW 10
#define SIZECOL 12

struct Move {
  int r, c, k;
};

struct gameState {
	int kmap[SIZEROW+2][SIZECOL+2],
		pmap[SIZEROW+2][SIZECOL+2],
		player, eplayer,
		turn;

	struct Move mPlayer[3];
};

void init (struct gameState *gms) {
	int i; 

	memset (gms, 0, sizeof(gms));

	scanf ("%d", &(gms -> player));

	gms -> player++;
	gms -> turn = 1;

	for (i = 0; i <= SIZEROW+1; i++) {
		gms -> kmap[i][0] = -1;
		gms -> kmap[i][SIZECOL+1] = -1;
	}
	for (i = 0; i <= SIZECOL+1; i++) {
		gms -> kmap[0][i] = -1;
		gms -> kmap[SIZEROW+1][i] = -1;
	}
}

#ifdef DEBUG
#include <windows.h>

void getRandomK (struct gameState *gms) {
	gms -> mPlayer[gms -> turn].k = rand() % 20 + 1;
}

void fillMap (struct gameState *gms) {
	int i, j;

	for (i = 1; i <= SIZEROW; i++) {
		for (j = 1; j <= SIZECOL; j++) {
			gms -> kmap[i][j] = rand() % 20 + 1;
			gms -> pmap[i][j] = rand() % 3;
		}
	}
}

void printState (const struct gameState *gms) {
	HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 

	int i, j; 

	GetConsoleScreenBufferInfo(h, &csbiInfo);
	wOldColorAttrs = csbiInfo.wAttributes; 

	printf("    ");
	for (i = 1; i <= SIZECOL; i++) {
		printf("%4d", i);
	}
	printf("\n");

	for (i = 1; i <= SIZEROW; i++) {
		printf("%2d", i);
		if (i % 2 == 1)
			printf("  ");
		for (j = 1; j <= SIZECOL; j++) {
			SetConsoleTextAttribute(h, (gms -> pmap[i][j] ? 1 << gms -> pmap[i][j] : 0) | FOREGROUND_INTENSITY);
			printf("%4d", gms -> kmap[i][j]);
		}

		SetConsoleTextAttribute (h, wOldColorAttrs);
		printf("\n");
	}
}

#endif

void readMove (struct Move *mv) {
	scanf ("%d %d %d", &(mv -> k), &(mv -> r), &(mv -> c));
}

void getMove (struct gameState *gms) {

}

int end(const struct gameState *gms) {
	return 1;
}

void game (struct gameState *gms) {
	while (!end (gms)) {
		if (gms -> turn != gms -> player) {
			readMove (&(gms -> mPlayer[gms -> eplayer]));
		} else {
#ifdef DEBUG
			getRandomK (gms);
			scanf ("%d %d", &((gms -> mPlayer[gms -> player]).r), &((gms -> mPlayer[gms -> player]).c));
#else
			scanf ("%d", &((gms -> mPlayer[gms -> player]).k));
			getMove (gms);
#endif
		}

#ifdef DEBUG
		printState (gms);
#endif
		gms -> turn = 3 - gms -> turn;
	}
}

int main () {
	struct gameState gms;
	int i, j;
	
	init (&gms);

#ifdef DEBUG
	fillMap(&gms);
	printState(&gms);
#endif

	// game (&gms);

	return 0;
}
