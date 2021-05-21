#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TMin INT_MIN
#define TMax INT_MAX

#include "btest.h"
#include "bits.h"

test_rec test_set[] = {
    {"bitReverse", (funct_t)bitReverse, (funct_t)test_bitReverse, 1, "! ~ & ^ | + << >>", 40, 4, {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"mod3", (funct_t)mod3, (funct_t)test_mod3, 1, "! ~ & ^ | + << >>", 90, 4, {{1, TMax}, {TMin, TMax}, {TMin, TMax}}},
    {"float_f2i", (funct_t)float_f2i, (funct_t)test_float_f2i, 1, "$", 30, 4, {{1, 1}, {1, 1}, {1, 1}}},
    {"", NULL, NULL, 0, "", 0, 0, {{0, 0}, {0, 0}, {0, 0}}}};
