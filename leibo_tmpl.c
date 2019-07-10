#include "leibo_tmpl.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct vec {
	int cap;
	int len;
	struct leibo_string_view *strs;
};
void vec_push(struct vec *vec, struct leibo_string_view str);
int vec_find(struct vec *vec, struct leibo_string_view str);

bool leibo_string_view_equal(struct leibo_string_view rhs, struct leibo_string_view lhs);

struct leibo_tmpl *leibo_tmpl_from_stringpiece(struct leibo_string_view sv) {
	struct leibo_tmpl *tmpl = LEIBO_REALLOC(NULL, sizeof(*tmpl));
	char *text = sv.str;
	char *param_start;
	char *param_end;
	struct vec pieces = {};
	struct vec params = {};
	for (;;) {
		struct leibo_string_view sv_text;
		struct leibo_string_view sv_param;
		struct leibo_string_view sv_param_placeholder = {};
		int param_index;

		param_start = strstr(sv.str, "{{");
		if (param_start == NULL) {
			vec_push(&pieces, sv);
			break;
		}
		// piece of text so far
		sv_text.str = sv.str;
		sv_text.size = param_start-sv.str;
		if (sv_text.size > 0) {
			vec_push(&pieces, sv_text);
		}

		sv_param.str = param_start + strlen("{{");
		param_end = strstr(param_start, "}}");
		if (param_end == NULL) {
			goto error;
		}
		sv_param.size = param_end-(param_start+strlen("{{"));

		param_index = vec_find(&params, sv_param);
		if (param_index < 0) {
			vec_push(&params, sv_param);
			param_index = params.len-1;
		}
		sv_param_placeholder.size = -param_index-1;
		vec_push(&pieces, sv_param_placeholder);

		// remove processed prefix
		sv.size -= (param_end+strlen("}}")-sv.str);
		sv.str = param_end+strlen("}}");
	}

	tmpl->pieces = pieces.strs;
	tmpl->npieces = pieces.len;
	tmpl->params = params.strs;
	tmpl->nparams = params.len;
	return tmpl;
error:
	leibo_tmpl_free(&tmpl);
	return NULL;
}

void leibo_tmpl_free(struct leibo_tmpl **ptmpl) {
	struct leibo_tmpl empty = {};
	if (!ptmpl || !(*ptmpl)) {
		return;
	}
	LEIBO_FREE((*ptmpl)->params);
	LEIBO_FREE((*ptmpl)->pieces);
	LEIBO_FREE(*ptmpl);
	*ptmpl = NULL;
}

void leibo_tmpl_set_param(struct leibo_tmpl *tmpl, int index, struct leibo_string_view sv) {
	tmpl->params[index] = sv;
}

void leibo_tmpl_set_param_str(struct leibo_tmpl *tmpl, int index, char *str) {
	struct leibo_string_view sv = { str, strlen(str) };
	leibo_tmpl_set_param(tmpl, index, sv);
}

struct leibo_tmpl *leibo_tmpl_from_str(char *str) {
	struct leibo_string_view sv = {str, strlen(str)};
	return leibo_tmpl_from_stringpiece(sv);
}

bool leibo_tmpl_iterator_next(struct leibo_tmpl *tmpl, struct leibo_tmpl_iterator *it) {
	struct leibo_string_view sv;
	if (it->i == tmpl->npieces) {
		return false;
	}
	it->current = tmpl->pieces[it->i];
	if (it->current.size < 0) {
		it->param_index = -it->current.size-1;
		it->piece_index = -1;
		it->current = tmpl->params[it->param_index];
	} else {
		it->param_index = -1;
		it->piece_index = it->i;
	}
	it->i++;
	return it->i <= tmpl->npieces;
}

void vec_push(struct vec *vec, struct leibo_string_view str) {
	if (vec->len >= vec->cap) {
		vec->cap = (vec->cap == 0) ? 8 : vec->cap * 1.7;
		vec->strs = LEIBO_REALLOC(vec->strs, vec->cap*sizeof(*vec->strs));
	}
	vec->strs[vec->len] = str;
	vec->len++;
}

int vec_find(struct vec *vec, struct leibo_string_view str) {
	int i;
	for (i=0;i<vec->len;i++) {
		if (leibo_string_view_equal(vec->strs[i], str)) {
			return i;
		}
	}
	return -1;
}

void vec_free(struct vec *vec) {
	LEIBO_FREE(vec->strs);
	vec->strs = NULL;
	vec->len = 0;
	vec->cap = 0;
}

bool leibo_string_view_equal(struct leibo_string_view rhs, struct leibo_string_view lhs) {
	if (rhs.size != lhs.size) {
		return false;
	}
	return memcmp(rhs.str, lhs.str, rhs.size) == 0;
}

bool leibo_string_view_equal_str(struct leibo_string_view rhs, char *lhs) {
	struct leibo_string_view lhs_sv = { lhs, strlen(lhs) };
	return leibo_string_view_equal(rhs, lhs_sv);
}

