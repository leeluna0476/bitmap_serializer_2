#ifndef UTIL_H
# define UTIL_H

#include <stdlib.h>
#include <sys/types.h>

#define BUFSIZE 256

typedef struct vvector
{
	char	**p;
	size_t	cap;
}	vvec_t;

typedef struct vector {
	char	*p;
	size_t	len;
	size_t	cap;
} vec_t;

char	*get_input_in_one_str(void);
void	term_print_digit(int digit, char *p);
int		longest_line_len(char **strings);
int		how_many_lines(char **strings);
char	**split(const char *s, char c);

#endif
