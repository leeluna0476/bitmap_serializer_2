#include <stdio.h>
extern char digits[10][8];

#define BLACK "0"
#define WHITE " "

int	main(void) {
	int height = 12;
	int width = 12;
	int hdiff = (height - 8) / 2;
	int wdiff = (width - 8) / 2;
	for (int k = 0; k < 10; ++k) {
		for (int i = 0; i < height; ++i) {
			int di = i - hdiff;
			printf("\033[%dC", width*k);
			for (int j = 0; j < width; ++j) {
				int dj = j - wdiff;
				if (i >= hdiff && i < height - hdiff && j >= wdiff && j < width - wdiff && ((digits[k][di] >> (7 - dj)) & 1)) {
					printf(BLACK);
				}
				else {
					printf(WHITE);
				}
			}
			printf("\n");
		}
		printf("\033[%dA", height);
	}
	printf("\033[%dB", height);
	return 0;
}
