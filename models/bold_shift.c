#include <stdio.h>

unsigned char zero[8] = { 0x3c, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x3c };

void print_normal(void) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 7; j >= 0; --j) {
			printf("%c", " *"[(zero[i] >> j) & 1]);
		}
		printf("\n");
	}
}

void print_bold(void) {
	for (int i = 0; i < 8; ++i) {
		unsigned char line = (zero[i] << 1) | zero[i] | (zero[i] >> 1);
		for (int j = 7; j >= 0; --j) {
			printf("%c", " *"[(line >> j) & 1]);
		}
		printf("\n");
	}
}

void print_italic(void) { // 끝이 잘린다.
	for (int i = 0; i < 8; ++i) {
		unsigned char line = zero[i];
		if (i < 3)
			line >>= 1;
		else if (i > 4)
			line <<= 1;
		for (int j = 7; j >= 0; --j) {
			printf("%c", " *"[(line >> j) & 1]);
		}
		printf("\n");
	}
}

int main(void) {
	print_normal();
	printf("\n");
	print_bold();
	printf("\n");
	print_italic();
	printf("\n");
}
