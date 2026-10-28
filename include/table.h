#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint;

uint *new_table(size_t n);

void free_table(uint *table);

uint sum_poly(uint x, uint y);

uint mult_poly(size_t n, uint x, uint y);

uint mult_poly_mod(size_t n, uint x, uint y, uint ppoly);

uint *get_tab_exp(size_t n, uint ppoly);

uint *fill_tab_log(size_t n, uint *tab_exp);

uint inv_mod(uint poly, uint *tab_log, uint *tab_exp, size_t n);

uint *fill_sum_lookup(size_t n);

uint *fill_prod_lookup(size_t n, uint *tab_exp, uint *tab_log);

void print_mat(const uint *mat, size_t n);