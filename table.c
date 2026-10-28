#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint;


uint *new_table(size_t n){

	uint *table = malloc((1<<n)*sizeof(uint));

	if (table == NULL){
		fprintf(stderr, "Table allocation failed.\n");
        exit(EXIT_FAILURE);
	}

	return table;
}


void free_table(uint *table){
	if (table != NULL){
		free(table);
	}
}


uint sum_poly(size_t n, uint x, uint y){
	return x^y;
}


uint mult_poly(size_t n, uint x, uint y){
	uint res = 0;

	// Product of x and y.
	for (size_t i = 0; i < n; i++){  
		if ((x & (1 << i)) != 0){      // if the term x^i is not zero.
			res ^= y << i;			 // multiply y by it and add it to the result.
		}
	}

	return res;
}


uint mult_poly_mod(size_t n, uint x, uint y, uint ppoly){
	uint res = mult_poly(n,x,y);

	// Reduction mod ppoly.
    for (int i = 2 * (int)n - 2; i >= (int)n; i--) {
        if ((res & (1 << i)) != 0) {       // if the term res^i is not zero, it must be reduced mod ppoly
            res ^= (ppoly << (i - n));    // then subtract ppoly^(i-n) to the result.
        }
    }

	return res;
}


void fill_tab_exp(uint *tab, size_t n, uint ppoly){
	size_t k = (1<<n) - 1;  // size of the multiplicative group.
	uint tmp = 1U;          // 1 in F2^n[X].

	for (size_t e = 0; e<k; e++){
		tab[e] = tmp;

		tmp <<= 1; // multiply tmp by X.

		if ((tmp & (1<<n)) !=0){  
			tmp ^= ppoly;  // reduce modulo ppoly
		}
	}
}


void fill_tab_log(uint *tab_log, uint *tab_exp, size_t n){
	size_t k = (1<<n) - 1;  // size of the multiplicative group.	
	size_t tmp;

	// tab_log[0] is infinity, works if n<32 here.
	tab_log[0] = (uint) -1;

	for (size_t e = 0; e<k; e++){
		tmp = (size_t) tab_exp[e];
		tab_log[tmp] = (uint) e;
	}
}


uint inv_mod(uint poly, uint *tab_log, uint *tab_exp, size_t n){
	if (poly == 1){
		return 1;
	}

	uint exp = tab_log[poly];
	return tab_exp[(1<<n) - 1 - exp];
}


int main(){
	printf("main\n");
}