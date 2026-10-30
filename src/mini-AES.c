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

void MixColumns(uint *state_mat, uint ppoly){
	uint a = 0x2;
	uint b = 0x3;

	uint s0 = state_mat[0];
	uint s1 = state_mat[1];
	uint s2 = state_mat[2];
	uint s3 = state_mat[3];

	state_mat[0] = mult_poly_mod(4, a, s0, ppoly) ^ mult_poly_mod(4, b, s1, ppoly);
	state_mat[1] = mult_poly_mod(4, a, s1, ppoly) ^ mult_poly_mod(4, b, s0, ppoly);
	state_mat[2] = mult_poly_mod(4, a, s2, ppoly) ^ mult_poly_mod(4, b, s3, ppoly);
	state_mat[3] = mult_poly_mod(4, a, s3, ppoly) ^ mult_poly_mod(4, b, s2, ppoly);
}

void AddRoundKey(uint *state_mat, uint *key_mat){
	for (int i = 0; i<4; i++){
		state_mat[i] ^= key_mat[i];
	}
}


int get_c(int i) {
    static const int c_values[16] = {
        1, 1, 2, 2, 1, 2, 2, 1, 
        1, 1, 1, 2, 1, 1, 1, 1
    };
    
    if (i < 0 || i > 15){
       	fprintf(stderr, "i out of range in get_c\n");
       	return 1; 
    }
    return c_values[i];
}


void next_key(uint *key_mat, int i, uint *tab_log, uint *tab_exp){
	int c = get_c(i);
	uint k2k3 = key_mat[2]<<4 | key_mat[3];
	k2k3 = ((k2k3 >> (8-c)) | (k2k3<<c)) & 0xFF; // circular left shift of c bits
	k2k3 = (sub_byte((k2k3>>4), 4, tab_log, tab_exp)^(1<<i))<<4 | sub_byte((k2k3 & 0xF), 4, tab_log, tab_exp);
	uint new_k0k1 = k2k3 ^ (key_mat[0]<<4 | key_mat[1]);
	uint new_k2k3 = new_k0k1 ^ (key_mat[2]<<4 | key_mat[3]);
	key_mat[0] = new_k0k1>>4;
	key_mat[1] = new_k0k1&0xF;
	key_mat[2] = new_k2k3>>4;
	key_mat[3] = new_k2k3&0xF;
}


void mini_aes_enc(uint msg[4], uint key[4], uint *tab_log, uint *tab_exp, uint ppoly){

	AddRoundKey(msg, key);
	next_key(key, 0, tab_log, tab_exp);
	int round_nb = 16;

	for(int i = 1; i<round_nb; i++){
		SubBytes(4, tab_log, tab_exp, msg);
		ShiftRows(msg);
		MixColumns(msg, ppoly);
		next_key(key, i, tab_log, tab_exp);
		AddRoundKey(msg, key);
	}
}