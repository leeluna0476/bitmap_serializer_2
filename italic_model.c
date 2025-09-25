#include <stdio.h>

void	print_basic(char pattern[10][8]) {
	for (int j = 0; j < 8; ++j) {
		for (int i = 9; i >= 0; --i) {
			if (pattern[i][j])
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
}

void	print_italic(char pattern[10][8]) {
	int	space = 1;
	for (int j = 0; j < 8; ++j) {
		for (int i = 9; i >= 0; --i) {
			int	check = i + space;
			if (check >= 0 && check <= 9 && pattern[check][j])
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
		if (j == 2 || j == 4)
			--space;
	}
}

int	main(void) {
	char	one[10][8] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 0, 1, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 1, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	char	zero[10][8] = {
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 0 },
		{ 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 1, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0 },
	};

	print_basic(zero);
	print_italic(zero);

	return 0;
}
