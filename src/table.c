#include "table.h"


uint *new_table(size_t n){

	uint *table = malloc(n*sizeof(uint));

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


uint sum_poly(uint x, uint y){
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


uint *get_tab_exp(size_t n, uint ppoly){
	size_t k = (1<<n) - 1;  // size of the multiplicative group.
	uint tmp = 1U;          // 1 in F2^n[X].

	uint *tab_exp = new_table(k);

	for (size_t e = 0; e<k; e++){
		tab_exp[e] = tmp;

		tmp <<= 1; // multiply tmp by X.

		if ((tmp & (1<<n)) !=0){  
			tmp ^= ppoly;  // reduce modulo ppoly
		}
	}

	return tab_exp;
}


uint *fill_tab_log(size_t n, uint *tab_exp){
	size_t k = (1<<n) - 1;  // size of the multiplicative group.	
	size_t tmp;

	uint *tab_log = new_table(k + 1);


	// tab_log[0] is infinity, works if n<32 here.
	tab_log[0] = (uint) -1;

	for (size_t e = 0; e<k; e++){
		tmp = (size_t) tab_exp[e];
		tab_log[tmp] = (uint) e;
	}

	return tab_log;
}


uint inv_mod(uint poly, uint *tab_log, uint *tab_exp, size_t n){
	if (poly == 1){
		return 1;
	}

	uint exp = tab_log[poly];
	return tab_exp[(1<<n) - 1 - exp];
}


uint *fill_sum_lookup(size_t n){
	size_t k = (1<<n);

	uint *sum_table = new_table(k*k);

	for (size_t i = 0; i<k; i++){
		for (size_t j = 0; j<k; j++){
			sum_table[i*k+j] = i^j;
		}
	} 

	return sum_table;
}


uint *fill_prod_lookup(size_t n, uint *tab_exp, uint *tab_log){
	size_t k = (1<<n);

	uint *prod_table = new_table(k*k);

	for (size_t i = 0; i<k; i++){
		for (size_t j = 0; j<k; j++){
			if (i==0 || j==0){
				prod_table[i*k+j] = 0;
			} else {
			prod_table[i*k+j] = tab_exp[(tab_log[i]+tab_log[j]) % (k-1)];
			}
		}
	}

	return prod_table;
}


void print_mat(const uint *mat, size_t n){
	printf("[");

	for (size_t i = 0; i<n; i++){
		printf("[");
		for (size_t j = 0; j<n; j++){
			printf("%X,", mat[i*n+j]);
		}
		printf("\b]");
		if ((i+1)!=n)
			printf("\n");
	}
	printf("]\n");
}
