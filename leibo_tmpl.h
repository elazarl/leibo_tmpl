#pragma once
#include <stdbool.h>

struct leibo_string_view {
	char *str;
	int size;
};

struct leibo_tmpl {
	struct leibo_string_view *pieces;
	struct leibo_string_view *params;
	int npieces;
	int nparams;
};

struct leibo_tmpl_iterator {
	int i;
	struct leibo_string_view current;
	int param_index; // index of parameter, -1 if we're not at a parameter
	int piece_index; // index of string piece. -1 if we're not in a string piece
};

#define leibo_tmpl_iterator_zero {0, {0, 0}, 0, 0}

bool leibo_tmpl_iterator_next(struct leibo_tmpl *tmpl, struct leibo_tmpl_iterator *it);
struct leibo_tmpl *leibo_tmpl_from_str(char *str);
struct leibo_tmpl *leibo_tmpl_from_stringpiece(struct leibo_string_view sv);
void leibo_tmpl_set_param(struct leibo_tmpl *tmpl, int index, struct leibo_string_view sv);
void leibo_tmpl_set_param_str(struct leibo_tmpl *tmpl, int index, char *str);
void leibo_tmpl_free(struct leibo_tmpl **ptmpl);

bool leibo_string_view_equal_str(struct leibo_string_view rhs, char *lhs);
bool leibo_string_view_equal(struct leibo_string_view rhs, struct leibo_string_view lhs);

#ifndef LEIBO_REALLOC
#define LEIBO_REALLOC realloc
#define LEIBO_FREE free
#endif
