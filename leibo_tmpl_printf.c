#include "leibo_tmpl_printf.h"

#include <stdio.h>

int leibo_tmpl_fprintf(FILE *file, struct leibo_tmpl *tmpl, ...) {
	int char_printed = 0;
	struct leibo_tmpl_iterator it = leibo_tmpl_iterator_zero;
	while (leibo_tmpl_iterator_next(tmpl, &it)) {
		char_printed += fprintf(file, "%.*s", it.current.size, it.current.str);
	}
	return char_printed;
}

int leibo_tmpl_printf(struct leibo_tmpl *tmpl, ...) {
	return leibo_tmpl_fprintf(stdout, tmpl);
}
