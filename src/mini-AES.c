#include "mini-AES.h"

static const uint MATRIX_A[4] = {0xB, 0xD, 0xE, 0x7};
static const uint VECTOR_B = 0x5;


uint mat_vec_prod(size_t taille, uint vec) {
    uint tmp = 0;

    for (size_t i = 0; i < taille; i++) {
        // __builtin_parity renvoie 1 si le nombre de bits à 1 est impair (produit scalaire modulo 2)
        if (__builtin_parity(vec & MATRIX_A[i])) {
            tmp |= (1U << i);
        }
    }

    return tmp;
}

uint sub_byte(uint byte, size_t n, uint *tab_log, uint *tab_exp){
	if (byte == 0){
		return VECTOR_B;
	}

	byte = inv_mod(byte, tab_log, tab_exp, n);
	byte = mat_vec_prod(n, byte);
	
	byte ^= VECTOR_B;

	return byte;
}

void SubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat){
	for(size_t i = 0; i<4; i++){
		state_mat[i] = sub_byte(state_mat[i], n, tab_log, tab_exp);	
	}
}


void ShiftRows(uint *state_mat){
	uint tmp = state_mat[1];
	state_mat[1] = state_mat[3];
	state_mat[3] = tmp;
}