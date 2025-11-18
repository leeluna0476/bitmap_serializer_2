#include <stdio.h>
#include <string.h>

extern unsigned char digits[10][10]; // ../letter.c

// big_int points a 10byte-sized unsigned char array.
void left_shift_big_int(const unsigned char *big_int, unsigned char *buf, int Nbits);
void right_shift_big_int(const unsigned char *big_int, unsigned char *buf, int Nbits);
void mask_big_int(unsigned char *big_int);

// implement (data << 1) | data | (data >> 1) for a 10B data.
// leave the first and the tenth bits to zero by using the func 'mask' since they represent whitespace.
void bold(unsigned char *data, unsigned char *buf);

void extract_pattern(unsigned char *data);

int main(void) {
	int i;
	for (i = 0; i < 6; ++i) {
		unsigned char buf[10];
		bold(digits[i], buf);

		extract_pattern(digits[i]);
		printf("\n");
		extract_pattern(buf);
		printf("\n");
	}
}

void left_shift_big_int(const unsigned char *big_int, unsigned char *buf, int Nbits) {
	int i, Nbytes = Nbits / 8, Nbits_mod = Nbits % 8;
	for (i = Nbytes; i < 9; ++i) {
		buf[i - Nbytes] |= (big_int[i] << Nbits_mod) | (big_int[i + 1] >> (8 - Nbits_mod));
	}
	buf[9 - Nbytes] = big_int[9] << Nbits_mod;
	for (i = 10 - Nbytes; i < 10; ++i) {
		buf[i] |= 0;
	}
}

void right_shift_big_int(const unsigned char *big_int, unsigned char *buf, int Nbits) {
	int i, Nbytes = Nbits / 8, Nbits_mod = Nbits % 8;
	for (i = 9; i >= Nbytes; --i) {
		buf[i] |= (big_int[i - Nbytes] >> Nbits_mod) | (big_int[i - Nbytes - 1] << (8 - Nbits_mod));
	}
	for (i = 0; i < Nbytes; ++i) {
		buf[i] |= 0;
	}
}

void mask_big_int(unsigned char *big_int) {
	big_int[0] = 0;
	big_int[9] = 0;
}

void bold(unsigned char *data, unsigned char *buf) {
	memcpy(buf, digits[i], sizeof(buf));
	left_shift_big_int(digits[i], buf, 8);
	right_shift_big_int(digits[i], buf, 8);
	mask_big_int(buf);
}

void extract_pattern(unsigned char *data) {
	int i, j;
	for (j = 7; j >= 0; --j) {
		for (i = 9; i >= 0; --i) {
			printf("%c", " *"[(data[i] >> j) & 1]);
		}
		printf("\n");
	}
}
