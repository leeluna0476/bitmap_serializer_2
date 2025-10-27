#include <stdio.h>

extern char digits[10][10];

int is_pattern_side(int digit, int i, int j) {
	if (i > 0 && i < 9) {
		for (int k = i - 1; k < i + 2; ++k) {
			if ((digits[digit][k] >> j) & 1) // 내 왼쪽, 오른쪽, 또는 내 자신이 1이면 OK.
				return 1;
		}
	}
	return 0;
}

void print_bold(int digit) {
	char *pixel = " *";
	for (int j = 7; j >= 0; --j) {
		for (int i = 9; i >= 0; --i) {
			printf("%c", pixel[is_pattern_side(digit, i, j)]);
		}
		printf("\n");
	}
}

void print_normal(int digit) {
	char *pixel = " *";
	for (int j = 7; j >= 0; --j) {
		for (int i = 9; i >= 0; --i) {
			printf("%c", pixel[(digits[digit][i] >> j) & 1]);
		}
		printf("\n");
	}
}

int main(void) {
	for (int digit = 0; digit < 6; ++digit) {
		print_normal(digit);
		printf("\n");
		print_bold(digit);
		printf("\n");
	}
}
