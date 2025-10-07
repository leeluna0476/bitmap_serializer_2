#include <stdio.h>

void	print_basic(char pattern[10]) {
	for (int j = 7; j >= 0; --j) {
		for (int i = 9; i >= 0; --i) {
			if ((pattern[i] >> j) & 1)
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
}

void	print_italic(char pattern[10]) {
	int	space = 1;
	for (int j = 7; j >= 0; --j) {
		for (int i = 9; i >= 0; --i) {
			int	check = i + space;
			if (check >= 0 && check <= 9 && (pattern[check] >> j) & 1)
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
		if (j == 5 || j == 3)
			--space;
	}
}

int	main(void) {
	char	one[10] = { 0x00, 0x01, 0x01, 0x01, 0xff, 0xff, 0x41, 0x21, 0x01, 0x00 };
	char	zero[10] = { 0x00, 0x7e, 0x7e, 0x81, 0x81, 0x81, 0x81, 0x7e, 0x7e, 0x00 };
	char	two[10] = { 0x00, 0x79, 0x79, 0x89, 0x89, 0x89, 0x89, 0x6f, 0x6f, 0x00 };
	char	three[10] = { 0x00, 0x66, 0x7e, 0x99, 0x99, 0x89, 0x81, 0x42, 0x66, 0x00 };

	print_basic(zero);
	print_italic(zero);
	print_basic(one);
	print_italic(one);
	print_basic(two);
	print_italic(two);
	print_basic(three);
	print_italic(three);

	return 0;
}
