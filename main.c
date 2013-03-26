#define DEBUG
#define MCTRL
#define HANDLE_TACTIC

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

FILE *kFile;
#endif

struct Move {
	int r, c, k;
};

struct gameState {
	int kmap[SIZEROW+2][SIZECOL+2],
		pmap[SIZEROW+2][SIZECOL+2],
		player, eplayer,
		turn,
		qMoves,
		AttQ, DefQ;		

	struct Move move[3];
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

	kFile = fopen("k.txt", "r");
#endif
}

void getK (struct gameState *gms) {
	int i;

#ifndef DEBUG
	scanf("%d", &i);
#else 
	if (kFile)
		fscanf(kFile, "%d", &i);
	else
		do {
			i = rand() % 20 + 1;
		} while (gms->nums[gms->turn][i] == 0);
#endif

	gms->move[gms->turn].k = i;
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

#ifdef DEBUG
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

	printf("k = %d\n", gms->move[gms->turn].k);

	for (i = 1; i <= 2; i++) {
		SetConsoleTextAttribute(h, (1 << i) | FOREGROUND_INTENSITY);
		printf("%4d", score[i]);
	}
	printf("\n");

	SetConsoleTextAttribute(h, wOldColorAttrs);
}
#endif

int maxK (struct gameState *gms, int p) {
	int i = MAXNUM;
	
	while (gms->nums[p][i] == 0)
		i--;

	return i;
}

int cost (struct gameState *gms, int r, int c, int k) {
	if (gms->kmap[r][c] == 0) 
		return -1;
	
	if (gms->kmap[r][c] > 0) {
		if (gms->pmap[r][c] == gms->turn && gms->kmap[r][c] < MAXNUM) 
			return 1 + 
#ifdef HANDLE_TACTIC
				gms->DefQ*
#endif
				(maxK(gms, 3 - gms->turn) > gms->kmap[r][c]);
		
		else if (gms->pmap[r][c] != gms->turn && gms->kmap[r][c] < k)
			return 
#ifdef HANDLE_TACTIC
				gms->AttQ* 
#endif
				gms->kmap[r][c];
	}
	
	return 0;
}

void inc (struct gameState *gms, int r, int c, int k) {
	if (gms->kmap[r][c] > 0) {
		if (gms->pmap[r][c] == gms->turn && gms->kmap[r][c] < MAXNUM)
			gms->kmap[r][c]++;
		else if (gms->pmap[r][c] != gms->turn && gms->kmap[r][c] < k)
			gms->pmap[r][c] = gms->turn;
	}
}

void makeMove (struct gameState *gms) {
	struct Move mv = gms->move[gms->turn];

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

int readMove (struct gameState *gms) {
	scanf("%d %d", &(gms->move[gms->turn].r), &(gms->move[gms->turn].c));

	return gms->kmap[gms->move[gms->turn].r][gms->move[gms->turn].c];
}

#ifdef DEBUG
void getRandomMove (struct gameState *gms) {
	int r, c;

	do {
		r = rand() % SIZEROW + 1;
		c = rand() % SIZECOL + 1;
	} while (gms->pmap[r][c] > 0);

	gms->move[gms->turn].r = r;
	gms->move[gms->turn].c = c;
}
#endif

#ifdef HANDLE_TACTIC
void setQs (struct gameState *gms) {
	gms->AttQ = 1 + (gms->move[gms->turn].k > maxK(gms, 3 - gms->turn) / 2);
	gms->DefQ = 3 - gms->AttQ;
}
#endif

void getGreedlyMove (struct gameState *gms) {
	int r, c,
		i, j,
		max = -INF;

#ifdef HANDLE_TACTIC
	setQs(gms);
#endif

	for (i = 1; i <= SIZEROW; i++)
		for (j = 1; j <= SIZECOL; j++) 
			if (gms->pmap[i][j] == 0) {
				int t = value(gms, i, j, gms->move[gms->turn].k);

				if (t > max) {
					max = t;
					r = i, c = j;
				}
			}

	gms->move[gms->turn].r = r;
	gms->move[gms->turn].c = c;
}

int end (const struct gameState *gms) {
	return gms->qMoves == SIZEROW * SIZECOL;
}

void game (struct gameState *gms) {
	while (!end (gms)) {
		getK(gms);

		if (gms->turn == gms->eplayer) {
#ifdef DEBUG
			getGreedlyMove(gms);
#else
			readMove(gms);
#endif
		} else {
#ifdef DEBUG
#ifdef MCTRL
			printf("k = %d\n", gms->move[gms->turn].k);
			while (readMove(gms));
#else
			getGreedlyMove(gms);
#endif
#else
			scanf("%d", &((gms->move[gms->turn]).k));
			getGreedlyMove(gms);
#endif
		}

		makeMove(gms);

#ifdef DEBUG
		printState(gms);
#ifndef MCTRL
		scanf("%*d");
#endif
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

#ifdef DEBUG
	scanf("%*d");
#endif

	return 0;
}
