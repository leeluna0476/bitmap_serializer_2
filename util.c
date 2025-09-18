#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char	*get_input_in_one_str() {
	char	*input = NULL;
	size_t	line_cap = 0;
	ssize_t	line_len = 0;

	vec_t	whole_str;
	whole_str.p = malloc(BUFSIZE);
	if (!(whole_str.p)) {
		return NULL;
	}

	whole_str.len = 0;
	whole_str.cap = BUFSIZE;

	while ((line_len = getdelim(&input, &line_cap, '\n', stdin)) > 0) {
		if (!input) {
			return NULL;
		}

		input[line_len] = '\0';
		// requirement: cap >= len + 1
		while (whole_str.cap <= whole_str.len + line_len) {
			whole_str.p = realloc(whole_str.p, whole_str.cap << 1);
			if (!whole_str.p) {
				free(input);
				return NULL;
			}
			whole_str.cap <<= 1;
		}
		strlcpy(whole_str.p + whole_str.len, input, whole_str.cap);
		whole_str.len += line_len;
	}

	free(input);
	return whole_str.p;
}

int	longest_line_len(char **strings) {
	int	longest = 0;
	int	i = 0;
	while (strings[i]) {
		int	each_len = strlen(strings[i]);
		if (each_len > longest)
			longest = each_len;
		++i;
	}

	return longest;
}

int	how_many_lines(char **strings) {
	int	i = 0;
	while (strings[i]) {
		++i;
	}

	return i;
}

static size_t	skip_delimeter_or_else(const char *s, char c, bool skip_del) {
	const char	*i = s;

	while (*i) {
		if ((skip_del && *i != c) || (!skip_del && *i == c))
			break ;
		++i;
	}
	return (i - s);
}

static void	*expand_vector(vvec_t *vec) {
	char	**update;

	update = realloc(vec->p, (vec->cap << 1));
	if (!update)
		return (NULL);
	vec->p = update;
	vec->cap <<= 1;
	return (vec);
}

static void	*delete_strings(char **strings) {
	while (*strings) {
		free(*strings);
		++strings;
	}
	free(strings);
	return (NULL);
}

static char	*substr(const char *s, unsigned int start, size_t len) {
	const size_t	s_len = strlen(s);
	size_t			substr_size;
	char			*substr;

	if (len <= s_len - start)
		substr_size = len;
	else
		substr_size = s_len - start;
	substr = malloc(substr_size + 1);
	if (substr)
		strlcpy(substr, s + start, substr_size + 1);
	return (substr);
}

char	**split(const char *s, char c) {
	vvec_t	vec;
	size_t	token_len;
	size_t	i;

	vec.cap = 4 * sizeof(char *);
	vec.p = malloc(vec.cap);
	if (!vec.p)
		return (NULL);
	i = 0;
	while (*s) {
		if ((i + 1) * sizeof(char *) == vec.cap) {
			if (!expand_vector(&vec))
				return (delete_strings(vec.p));
		}
		token_len = skip_delimeter_or_else(s, c, false);
		vec.p[i] = substr(s, 0, token_len);
		if (!vec.p[i])
			return (delete_strings(vec.p));
		vec.p[++i] = NULL;
		s += token_len;
		s += skip_delimeter_or_else(s, c, true);
	}
	return (vec.p);
}
