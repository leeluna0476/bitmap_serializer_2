#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp_file_format.h"
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "util.h"

extern const char digits[10][8];
extern const char alpha_s[26][8];
extern const char punctuations[3][8];

#define BLACK 0x00
#define WHITE 0xff

#define HORZ_SPACE 2
#define VERT_SPACE 2

char	is_supported_char(char c, int di, int dj) {
	char	ret = 0;
	if (isdigit(c)) {
		ret = (digits[c - '0'][di] >> (7 - dj)) & 1;
	}
	else if (islower(c)) {
		ret = (alpha_s[c - 'a'][di] >> (7 - dj)) & 1;
	}
	else if (c >= ',' && c <= '.') {
		ret = (punctuations[c - ','][di] >> (7 - dj)) & 1;
	}
	return ret;
}

int	main() {
	// 나중에 처음부터 2차원 배열로 받는 거로 바꿔볼까?
	char	*whole_str = get_input_in_one_str();
	if (!whole_str) {
		perror("malloc");
		return 1;
	}
	char	**splitted_str = split(whole_str, '\n');
	if (!splitted_str) {
		free(whole_str);
		perror("malloc");
		return 1;
	}

	int	line_num = how_many_lines(splitted_str);
	int	longest_len = longest_line_len(splitted_str); // 개행 미포함.

	int	letter_width = 12;
	int	letter_height = 12;
	int	image_width = longest_len * letter_width + 4;
	int	image_height = line_num * letter_height + 4;


/////INFO//////HEADER///////////////////////////////////
	struct BmpInfoHeader	info_header;

	info_header.size = sizeof(struct BmpInfoHeader);
	info_header.width = image_width;
	info_header.height = image_height;
	info_header.color_plane = 1;
	info_header.bits_per_pixel = BITS_DEFAULT;
	// BI_RGB = 0. no compression.
	info_header.compression = 0;
	info_header.image_size = 0;
	// resolution = 미터당 픽셀 밀집도. 기본값 = 0.
	info_header.horizontal_resolution = 0;
	info_header.vertical_resolution = 0;
	// palette를 사용하지 않을 때(bits_per_pixel > 8)는 0.
	info_header.color_number = 1 << info_header.bits_per_pixel;
	info_header.color_number_important = 0;

	// 색상 개수 * 4. 바이트 단위.
	uint32_t	color_table_size = info_header.color_number << 2;

/////FILE//////HEADER///////////////////////////////////
	struct BmpFileHeader	file_header;

	file_header.type = 0x4d42;
	// file header size + info header size + palette size + pixel data
	// 각 픽셀 크기는 bits_per_pixel에 따름, 픽셀 개수는 너비와 동, 패딩 사이즈 = (4 - (pixel % 4)) % 4
	// palette 크기는 조건부. 비트 깊이가 8비트 이하일 때만 적용.
	// 사이즈는 모두 바이트 단위.
	// 픽셀 하나에 할당되는 바이트 수. // 8로 올림을 한 뒤 잘랐는데 지금 시점에서는 의미 없어보임.
	unsigned short	pixel_size = (info_header.bits_per_pixel + 7) >> 3;
	// 행 하나에 할당되는 바이트 수.
	int	row_size_byte = info_header.width * pixel_size;
	// 각 행을 4의 배수로 패딩.
	// 모자란 바이트 수 % 4 (4 - 0 = 4 방지)
	int	padding = (4 - (row_size_byte % 4)) % 4;
	// 패딩 처리한 행의 바이트 수.
	int	padded_row_size = row_size_byte + padding;
	// 패딩 처리한 너비 * 높이.
	// 이미지에 삽입할 픽셀 데이터의 최종 크기.
	int	padded_matrix_size = info_header.height * padded_row_size;
	file_header.size = \
					   sizeof(struct BmpFileHeader) \
					   + sizeof(struct BmpInfoHeader) \
					   + color_table_size \
					   + padded_matrix_size;

	file_header.reserved_1 = 0;
	file_header.reserved_2 = 0;
	file_header.offbits = \
						  sizeof(struct BmpFileHeader) \
						  + sizeof(struct BmpInfoHeader) \
						  + color_table_size;

/////COLOR//////TABLE///////////////////////////////////

	unsigned char	*color_table = malloc(color_table_size);
	if (!color_table) {
		free(whole_str);
		perror("malloc");
		return 1;
	}

	for (unsigned short i = 0; i < info_header.color_number; ++i) {
		unsigned int	index = i << 2;
		color_table[index] = i; // Blue
		color_table[index + 1] = i; // Green
		color_table[index + 2] = i; // Red
		color_table[index + 3] = 0; // Reserved (항상 0)
	}

/////PIXEL//////DATA////////////////////////////////////
	unsigned char	**pixel_data = malloc(image_height * sizeof(unsigned char *));
	if (!pixel_data) {
		free(whole_str);
		free(color_table);
		perror("malloc");
		return 1;
	}
	for (int i = 0; i < image_height; ++i) {
		pixel_data[i] = malloc(padded_row_size);
		if (!pixel_data[i]) {
			for (int j = 0; j < i; ++j) {
				free(pixel_data[j]);
			}
			free(pixel_data);
			return 1;
		}
	}

	int	wdiff = (letter_width - 8) / 2;
	int	hdiff = (letter_height - 8) / 2;

	int _i = 0;
	int	_j = 0;
	int si = -1;
	char	pattern[2] = { 0x00, 0xff };
	for (int i = 0; i < image_height; ++i) {
		int	j;
		for (j = 0; j < 2; ++j) {
			pixel_data[i][j] = 0xff;
		}
		if (i < 2 || i >= image_height - 2) {
			for (; j < padded_row_size; ++j) {
				if (j < image_width)
					pixel_data[i][j] = 0xff;
				else
					pixel_data[i][j] = 0x00;
			}
		}
		else {
			int	sj = -1;
			if (!(_i % letter_height)) {
				_i = 0;
				++si;
			}
			int di = _i - hdiff;
			int pidx;
			for (; j < image_width - 2; ++j) { // 0 <= _j < 14
				if (!(_j % letter_width)) {
					if (sj == -1 || sj < strlen(splitted_str[si]))
						++sj;
					_j = 0;
				}
				int dj = _j - wdiff; // -2 <= dj < 12
				pidx = !(di >= 0 && di < 8 && dj >= 0 && dj < 8
						&& is_supported_char(splitted_str[si][sj], di, dj));
				pixel_data[i][j] = pattern[pidx];
				++_j;
			}
			for (; j < padded_row_size; ++j) {
				pidx = j < image_width;
				pixel_data[i][j] = pattern[pidx];
			}
			++_i;
		}
	}

	int fd = open("test.bmp", O_RDWR | O_CREAT, 0644);
	if (fd < 0) {
		free(whole_str);
		free(color_table);
		free(pixel_data);
		perror("open");
		return 1;
	}

	write(fd, &(file_header), sizeof(file_header));
	write(fd, &(info_header), sizeof(info_header));
	write(fd, color_table, color_table_size);

	for (int i = image_height - 1; i >= 0; --i) {
		write(fd, pixel_data[i], padded_row_size);
	}

	close(fd);
	for (int i = 0; i < image_height; ++i) {
		free(pixel_data[i]);
	}
	free(pixel_data);
	free(color_table);
	free(whole_str);
}
