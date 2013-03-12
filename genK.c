#include <stdio.h>
#include <windows.h>
#define SIZE 60

int main () {
  int i, j, t,
		nums[2][21];
	
	freopen("k.txt", "w", stdout);

	srand(time(NULL));

	for (i = 1; i <= 20; i++) {
		nums[0][i] = 3;
		nums[1][i] = 3;
	}

	i = 0;
	for (t = 0; t < 2*SIZE; t++) {
		do {
			j = rand() % 20 + 1;
		} while (nums[i][j] == 0);

		printf("%d ", j);

		nums[i][j]--;

		i = 1 - i;
	}
	printf("\n");

	return 0;
}
