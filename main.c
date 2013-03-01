#define DEBUG
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define SIZEROW 10
#define SIZECOL 12
#define MAXNUM 20
#define QNUM 3
#define INF 1000000000

#ifdef DEBUG
#include <time.h>
#include <windows.h>
#endif

struct Move {
  int r, c, k;
};

struct gameState {
	int kmap[SIZEROW+2][SIZECOL+2],
		pmap[SIZEROW+2][SIZECOL+2],
		player, eplayer,
		turn,
		qMoves;

	struct Move mPlayer[3];
	int nums[3][MAXNUM+1];
};

void init (struct gameState *gms) {
	int i; 

	memset (gms, 0, sizeof(*gms));

	scanf ("%d", &(gms->player));

	gms->player++;
	gms->eplayer = 3 - gms->player;
	gms->turn = 1;
	gms->qMoves = 0;

	for (i = 0; i <= SIZEROW+1; i++) {
		gms->kmap[i][0] = -1;
		gms->kmap[i][SIZECOL+1] = -1;
	}
	for (i = 0; i <= SIZECOL+1; i++) {
		gms->kmap[0][i] = -1;
		gms->kmap[SIZEROW+1][i] = -1;
	}

	for (i = 1; i <= MAXNUM; i++) 
		gms->nums[1][i] = gms->nums[2][i] = QNUM;
	
#ifdef DEBUG
	srand(time(NULL));
#endif
}

#ifdef DEBUG

void getRandomK (struct gameState *gms) {
	int i;
	
	do {
		i = rand() % 20 + 1;
	} while (gms->nums[gms->turn][i] == 0);

	gms->mPlayer[gms->turn].k = i;
}

void fillMap (struct gameState *gms) {
	int i, j;

	for (i = 1; i <= SIZEROW; i++) {
		for (j = 1; j <= SIZECOL; j++) {
			gms->kmap[i][j] = rand() % 20 + 1;
			gms->pmap[i][j] = rand() % 3;
		}
	}
}

void printState (const struct gameState *gms) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD wOldColorAttrs;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 

	int i, j, score[3];

	memset(score, 0, sizeof(score));

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
			SetConsoleTextAttribute(h, (gms->pmap[i][j] ? 1 << gms->pmap[i][j] : 0) | FOREGROUND_INTENSITY);
			printf("%4d", gms->kmap[i][j]);

			score[gms->pmap[i][j]] += gms->kmap[i][j];
		}

		SetConsoleTextAttribute(h, wOldColorAttrs);
		printf("\n");
	}

	printf("k = %d\n", gms->mPlayer[gms->turn].k);

	for (i = 1; i <= 2; i++) {
		SetConsoleTextAttribute(h, (1 << i) | FOREGROUND_INTENSITY);
		printf("%4d", score[i]);
	}
	printf("\n");

	SetConsoleTextAttribute(h, wOldColorAttrs);
}

void inc (struct gameState *gms, int r, int c, int k) {
	if (gms->kmap[r][c] > 0) {
		if (gms->pmap[r][c] == gms->turn && gms->kmap[r][c] < MAXNUM)
			gms->kmap[r][c]++;
		else if (gms->pmap[r][c] != gms->turn && gms->kmap[r][c] < k)
			gms->pmap[r][c] = gms->turn;
	}
}

int cost (struct gameState *gms, int r, int c, int k) {
	if (gms->kmap[r][c] == 0) 
		return -1;
	
	if (gms->kmap[r][c] > 0) {
		if (gms->pmap[r][c] == gms->turn && gms->kmap[r][c] < MAXNUM)
			return 1;
		else if (gms->pmap[r][c] != gms->turn && gms->kmap[r][c] < k)
			return gms->kmap[r][c];
	}

	return 0;
}

void makeMove (struct gameState *gms) {
	struct Move mv = gms->mPlayer[gms->turn];

	gms->kmap[mv.r][mv.c] = mv.k;
	gms->pmap[mv.r][mv.c] = gms->turn;

	inc(gms, mv.r, mv.c-1, mv.k);
	inc(gms, mv.r, mv.c+1, mv.k);
	inc(gms, mv.r+1, mv.c, mv.k);
	inc(gms, mv.r-1, mv.c, mv.k);

	if (mv.r % 2) {
		inc(gms, mv.r-1, mv.c+1, mv.k);
		inc(gms, mv.r+1, mv.c+1, mv.k);
	} else {
		inc(gms, mv.r-1, mv.c-1, mv.k);
		inc(gms, mv.r+1, mv.c-1, mv.k);
	}

	gms->nums[gms->turn][mv.k]--;
	gms->qMoves++;
}

#endif

int value (struct gameState *gms, int r, int c, int k) {
	int scr = 0;

	scr += cost(gms, r, c-1, k);
	scr += cost(gms, r, c+1, k);
	scr += cost(gms, r+1, c, k);
	scr += cost(gms, r-1, c, k);

	if (r % 2) {
		scr += cost(gms, r-1, c+1, k);
		scr += cost(gms, r+1, c+1, k);
	} else {
		scr += cost(gms, r-1, c-1, k);
		scr += cost(gms, r+1, c-1, k);
	}

	return scr;
}

void readMove (struct Move *mv) {
	scanf("%d %d %d", &(mv->k), &(mv->r), &(mv->c));
}

void getRandomMove (struct gameState *gms) {
	int r, c;

	do {
		r = rand() % SIZEROW + 1;
		c = rand() % SIZECOL + 1;
	} while (gms->pmap[r][c] > 0);

	gms->mPlayer[gms->turn].r = r;
	gms->mPlayer[gms->turn].c = c;
}

void getGreedlyMove (struct gameState *gms) {
	int r, c,
		i, j,
		max = -INF;

	for (i = 1; i <= SIZEROW; i++)
		for (j = 1; j <= SIZECOL; j++) 
			if (gms->pmap[i][j] == 0) {
				int t = value(gms, i, j, gms->mPlayer[gms->turn].k);

				if (t > max) {
					max = t;
					r = i, c = j;
				}
			}

	gms->mPlayer[gms->turn].r = r;
	gms->mPlayer[gms->turn].c = c;
}

void getMove (struct gameState *gms) {

}

int end (const struct gameState *gms) {
	return gms->qMoves == SIZEROW * SIZECOL;
}

void game (struct gameState *gms) {
	while (!end (gms)) {
		if (gms->turn == gms->eplayer) {
#ifdef DEBUG
			getRandomK(gms);
			getGreedlyMove(gms);
#else
			readMove(&(gms->mPlayer[gms->turn]));
#endif
		} else {
#ifdef DEBUG
			getRandomK(gms);
			
			printf("k = %d\n", gms->mPlayer[gms->turn].k);
			scanf("%d %d", &((gms->mPlayer[gms->turn]).r), &((gms->mPlayer[gms->turn]).c));

			// getGreedlyMove(gms);
#else
			scanf("%d", &((gms->mPlayer[gms->turn]).k));
			getMove(gms);
#endif
		}

		makeMove(gms);

#ifdef DEBUG
		printState(gms);
		// scanf("%*d");
#endif
		gms->turn = 3 - gms->turn;
	}
}

int main () {
	struct gameState gms;

	init(&gms);

#ifdef DEBUG
		printState(&gms);
#endif

	game(&gms);

	return 0;
}
