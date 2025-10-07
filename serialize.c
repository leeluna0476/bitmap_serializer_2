#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file_format.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "util.h"

extern const char digits[10][10];
//extern const char alpha_s[26][8];
//extern const char punctuations[3][8];
int	letter_width = 10; // 나중에 sizeof로 바꾸기.
int	letter_height = 8;

#define HORZ_SPACE 2
#define VERT_SPACE 2

//char	is_supported_char(char c, int di, int dj) {
//	char	ret = 0;
//	if (isdigit(c)) {
//		ret = (digits[c - '0'][di] >> (7 - dj)) & 1;
//	}
//	else if (islower(c)) {
//		ret = (alpha_s[c - 'a'][di] >> (7 - dj)) & 1;
//	}
//	else if (c >= ',' && c <= '.') {
//		ret = (punctuations[c - ','][di] >> (7 - dj)) & 1;
//	}
//	return ret;
//}

struct BmpInfoHeader	generate_info_header(int width, int height) {
	struct BmpInfoHeader	info_header;

	info_header.size = sizeof(struct BmpInfoHeader);
	info_header.width = width;
	info_header.height = height;
	info_header.colour_plane = 1;
	info_header.bits_per_pixel = BITS_DEFAULT;
	// BI_RGB = 0. no compression.
	info_header.compression = 0;
	info_header.image_size = 0;
	// resolution = 미터당 픽셀 밀집도. 기본값 = 0.
	info_header.horizontal_resolution = 0;
	info_header.vertical_resolution = 0;
	// palette를 사용하지 않을 때(bits_per_pixel > 8)는 0.
	info_header.colour_number = 1 << info_header.bits_per_pixel;
	info_header.colour_number_important = 0;

	return info_header;
}

struct BmpFileHeader	generate_file_header(struct BmpInfoHeader *info_header) {
	struct BmpFileHeader	file_header;

	file_header.type = 0x4d42;
	// file header size + info header size + palette size + pixel data
	// 각 픽셀 크기는 bits_per_pixel에 따름, 픽셀 개수는 너비와 동, 패딩 사이즈 = (4 - (pixel % 4)) % 4
	// palette 크기는 조건부. 비트 깊이가 8비트 이하일 때만 적용.
	// 사이즈는 모두 바이트 단위.
	// 픽셀 하나에 할당되는 바이트 수. // 8로 올림을 한 뒤 잘랐는데 지금 시점에서는 의미 없어보임.
	unsigned short	pixel_size = (info_header->bits_per_pixel + 7) >> 3;
	// 행 하나에 할당되는 바이트 수.
	int	row_size_byte = info_header->width * pixel_size;
	// 각 행을 4의 배수로 패딩.
	// 모자란 바이트 수 % 4 (4 - 0 = 4 방지)
	int	padding = (4 - (row_size_byte % 4)) % 4;
	// 패딩 처리한 행의 바이트 수.
	int	padded_row_size = row_size_byte + padding;
	// 패딩 처리한 너비 * 높이.
	// 이미지에 삽입할 픽셀 데이터의 최종 크기.
	int	colour_table_size = info_header->colour_number << 2;
	int	padded_matrix_size = info_header->height * padded_row_size;
	file_header.size = \
					   sizeof(struct BmpFileHeader) \
					   + sizeof(struct BmpInfoHeader) \
					   + colour_table_size \
					   + padded_matrix_size;

	file_header.reserved_1 = 0;
	file_header.reserved_2 = 0;
	file_header.offbits = \
						  sizeof(struct BmpFileHeader) \
						  + sizeof(struct BmpInfoHeader) \
						  + colour_table_size;

	return file_header;
}

unsigned char	*generate_colour_table(unsigned int colour_table_size) {
	unsigned char	*colour_table = malloc(colour_table_size);
	unsigned int	colour_number = colour_table_size >> 2;
	if (colour_table) {
		for (unsigned short i = 0; i < colour_number; ++i) {
			unsigned int	index = i << 2;
			colour_table[index] = i; // Blue
			colour_table[index + 1] = i; // Green
			colour_table[index + 2] = i; // Red
			colour_table[index + 3] = 0; // Reserved (항상 0)
		}
	}
	return colour_table;
}

unsigned char	**generate_pixel_data(struct BmpInfoHeader *info_header, struct BmpFileHeader *file_header, char **split_str, int line_num) {
	int	padded_matrix_size = file_header->size - sizeof(struct BmpFileHeader) - sizeof(struct BmpInfoHeader) - (info_header->colour_number << 2);
	int	padded_row_size = padded_matrix_size / info_header->height;
	unsigned char	**pixel_data = malloc(info_header->height * sizeof(unsigned char *));
	if (!pixel_data)
		return NULL;
	for (int i = 0; i < info_header->height; ++i) {
		pixel_data[i] = malloc(padded_row_size);
		if (!pixel_data[i]) {
			for (int j = 0; j < i; ++j) {
				free(pixel_data[i]);
			}
			free(pixel_data);
			return NULL;
		}
	}
	int	line_i = -1;
	int	line_j = -1;
	char	pattern[2] = { 0xff, 0x00 };
	for (int i = 0; i < info_header->height; ++i) {
		int	j;
		for (j = 0; j < 2; ++j) {
			pixel_data[i][j] = pattern[0];
		}
		// 실제 글자를 찍는 줄만 이 쪽에서 처리.
		if (i % (2 + letter_height) >= 2) {
			if (!((i - 2) % (2 + letter_height))) {
				++line_i;
			}
			line_j = -1;
			for (; j < info_header->width - 2; ++j) {
				if (!((j - 2) % letter_width)) {
					++line_j;
				}
				int	letter_i = i - (2 + (2 + letter_height) * line_i);
				int	letter_j = j - (2 + letter_width * line_j);;
				char	c = split_str[line_i][line_j];
				pixel_data[i][j] = pattern[(digits[c - '0'][9 - letter_j] >> (7 - letter_i)) & 1];
			}
		}
		// 줄간 공백은 이 쪽으로 이동.
		for (; j < padded_row_size; ++j) {
			pixel_data[i][j] = pattern[j >= info_header->width];
		}
	}
	return pixel_data;
}

int	main() {
	int ret = 0;
	// 나중에 처음부터 2차원 배열로 받는 거로 바꿔볼까?
	char	*whole_str = get_input_in_one_str();
	if (!whole_str) {
		perror("malloc");
		ret = 1;
		goto DONE;
	}
	char	**split_str = split(whole_str, '\n');
	free(whole_str);
	if (!split_str) {
		perror("malloc");
		ret = 1;
		goto DONE;
	}

	int	line_num = how_many_lines(split_str);
	int	longest_len = longest_line_len(split_str); // 개행 미포함.

	int	image_width = longest_len * letter_width + 4;
	int	image_height = line_num * letter_height + 4;


	struct BmpInfoHeader	info_header = generate_info_header(image_width, image_height);
	struct BmpFileHeader	file_header = generate_file_header(&info_header);
	int	colour_table_size = info_header.colour_number << 2;
	unsigned char	*colour_table = generate_colour_table(colour_table_size);
	if (!colour_table) {
		perror("malloc");
		ret = 1;
		goto FREE_STRING;
	}

/////PIXEL//////DATA////////////////////////////////////
	unsigned char	**pixel_data = generate_pixel_data(&info_header, &file_header, split_str, line_num);
	if (!pixel_data) {
		perror("malloc");
		ret = 1;
		goto FREE_COLOUR_TABLE;
	}

/////PRINT///////////////////////////////////////////////
	int fd = open("test.bmp", O_RDWR | O_CREAT, 0644);
	if (fd < 0) {
		perror("open");
		ret = 1;
		goto FREE_PIXEL_DATA;
	}

	write(fd, &(file_header), sizeof(file_header));
	write(fd, &(info_header), sizeof(info_header));
	write(fd, colour_table, colour_table_size);

	int	padded_matrix_size = file_header.size - sizeof(struct BmpFileHeader) - sizeof(struct BmpInfoHeader) - (info_header.colour_number << 2);
	int	padded_row_size = padded_matrix_size / info_header.height;
	for (int i = image_height - 1; i >= 0; --i) {
		write(fd, pixel_data[i], padded_row_size);
	}

	close(fd);
FREE_PIXEL_DATA:
	for (int i = 0; i < image_height; ++i) {
		free(pixel_data[i]);
	}
	free(pixel_data);
FREE_COLOUR_TABLE:
	free(colour_table);
FREE_STRING:
	for (int i = 0; split_str[i]; ++i) {
		free(split_str[i]);
	}
	free(split_str);
DONE:
	return ret;
}
