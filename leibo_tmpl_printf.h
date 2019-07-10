#pragma once

#include "leibo_tmpl.h"

#include <stdio.h>

int leibo_tmpl_printf(struct leibo_tmpl *tmpl, ...);
int leibo_tmpl_fprintf(FILE *file, struct leibo_tmpl *tmpl, ...);
