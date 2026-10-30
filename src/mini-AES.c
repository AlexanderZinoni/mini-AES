#include "mini-AES.h"

static const uint MATRIX_A[4] = {0xB, 0xD, 0xE, 0x7};
static const uint VECTOR_B = 0x5;

void print_state(uint s[4]){
    for (int i = 0; i<4; i++){
        printf("%X", s[i]);
    }
    printf(" \n");
}


uint mat_vec_prod(size_t taille, uint vec, const uint mat[4]) {
    uint tmp = 0;

    for (size_t i = 0; i < taille; i++) {
        // __builtin_parity renvoie 1 si le nombre de bits à 1 est impair (produit scalaire modulo 2)
        if (__builtin_parity(vec & mat[i])) {
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
	byte = mat_vec_prod(n, byte, MATRIX_A);
	byte ^= VECTOR_B;

	return byte;
}

uint inv_sub_byte(uint byte, size_t n, uint *tab_log, uint *tab_exp){
	if (byte == VECTOR_B){
		return 0;
	}

	byte ^= VECTOR_B;
	byte = mat_vec_prod(n, byte, MATRIX_A); // A is an orthogonal matrix
	byte = inv_mod(byte, tab_log, tab_exp, n);
	return byte;
}

void SubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat){
	for(size_t i = 0; i<4; i++){
		state_mat[i] = sub_byte(state_mat[i], n, tab_log, tab_exp);	
	}
}

void InvSubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat){
	for(size_t i = 0; i<4; i++){
		state_mat[i] = inv_sub_byte(state_mat[i], n, tab_log, tab_exp);	
	}
}

void ShiftRows(uint *state_mat){
	uint tmp = state_mat[1];
	state_mat[1] = state_mat[3];
	state_mat[3] = tmp;
}

void MixColumns(uint *state_mat, uint *prod_table){
	uint a = 0x2;
	uint b = 0x3;

	uint s0 = state_mat[0];
	uint s1 = state_mat[1];
	uint s2 = state_mat[2];
	uint s3 = state_mat[3];

	state_mat[0] = fast_prod(4, a, s0, prod_table) ^ fast_prod(4, b, s1, prod_table);
	state_mat[1] = fast_prod(4, a, s1, prod_table) ^ fast_prod(4, b, s0, prod_table);
	state_mat[2] = fast_prod(4, a, s2, prod_table) ^ fast_prod(4, b, s3, prod_table);
	state_mat[3] = fast_prod(4, a, s3, prod_table) ^ fast_prod(4, b, s2, prod_table);
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
	k2k3 = (sub_byte((k2k3>>4), 4, tab_log, tab_exp)^(1<<(i%4)))<<4 | sub_byte((k2k3 & 0xF), 4, tab_log, tab_exp);
	uint new_k0k1 = k2k3 ^ (key_mat[0]<<4 | key_mat[1]);
	uint new_k2k3 = new_k0k1 ^ (key_mat[2]<<4 | key_mat[3]);
	key_mat[0] = new_k0k1>>4;
	key_mat[1] = new_k0k1&0xF;
	key_mat[2] = new_k2k3>>4;
	key_mat[3] = new_k2k3&0xF;
}


void mini_aes_enc(uint msg[4], const uint key[4], uint *tab_log, uint *tab_exp, uint *prod_table, int rounds){
	uint key_cpy[4];

	for (int i=0; i<4; i++){
		key_cpy[i] = key[i];
	}

	AddRoundKey(msg, key_cpy);

	for(int i = 1; i<rounds; i++){
		SubBytes(4, tab_log, tab_exp, msg);
		ShiftRows(msg);
		MixColumns(msg, prod_table);
		next_key(key_cpy, i, tab_log, tab_exp);
		AddRoundKey(msg, key_cpy);
	}
}


void mini_aes_dec(uint msg[4], const uint key[4], uint *tab_log, uint *tab_exp, uint *prod_table, int rounds){
	uint key_cpy[4];
	for (int i=0; i<4; i++){
		key_cpy[i] = key[i];
	}
	
	uint all_keys[rounds][4];

	for (int k=0; k<4; k++){
		all_keys[0][k] = key_cpy[k];
	}

	for (int i=1; i<rounds; i++){
		next_key(key_cpy, i, tab_log, tab_exp);
		for (int k=0; k<4; k++){
			all_keys[i][k] = key_cpy[k];
		}
	}

	for(int i = 1; i<rounds; i++){
		AddRoundKey(msg, all_keys[rounds-i]);
		MixColumns(msg, prod_table);
		ShiftRows(msg);
		InvSubBytes(4, tab_log, tab_exp, msg);
	}

	AddRoundKey(msg, all_keys[0]);
}


void decToHexa(int n, char *hex) {
    char hexaDeciNum[100];
    int i = 0;

    if (n == 0) {
        hex[0] = '0';
        hex[1] = '\0';
        return;
    }

    while (n != 0) {
        int temp = n % 16;

        if (temp < 10)
            hexaDeciNum[i++] = temp + '0';
        else
            hexaDeciNum[i++] = temp + 'A' - 10;

        n /= 16;
    }

    int j = 0;
    while (i > 0)
        hex[j++] = hexaDeciNum[--i];

    hex[j] = '\0';
}

void ASCIItoHEX(const char *ascii, char *hex) {
    char part[10];
    int j = 0;

    for (int i = 0; ascii[i] != '\0'; i++) {
        decToHexa((unsigned char)ascii[i], part);

        for (int k = 0; part[k] != '\0'; k++)
            hex[j++] = part[k];
    }

    hex[j] = '\0';
}

void enc_msg(const char *msg, const uint key[4],
             uint *tab_log, uint *tab_exp,
             uint *prod_table, int rounds,
             char *cipher_hex) {

    size_t len = strlen(msg);

    // PKCS#7 padding: block size = 2 bytes
    size_t padding = 2 - (len % 2);
    size_t padded_len = len + padding;

    size_t j = 0;

    for (size_t i = 0; i < padded_len; i += 2) {

        uint state[4];

        // Get the two bytes of the current block
        uint byte1 = (i < len) ? (unsigned char)msg[i] : padding;
        uint byte2 = (i + 1 < len) ? (unsigned char)msg[i + 1] : padding;

        // Split bytes into four hexadecimal digits (nibbles)
        state[0] = byte1 >> 4;
        state[1] = byte1 & 0xF;
        state[2] = byte2 >> 4;
        state[3] = byte2 & 0xF;

        // Encrypt the block
        mini_aes_enc(state, key, tab_log, tab_exp, prod_table, rounds);

        // Convert encrypted state to hexadecimal
        for (int k = 0; k < 4; k++) {
            cipher_hex[j++] = (state[k] < 10)
                              ? state[k] + '0'
                              : state[k] - 10 + 'A';
        }
    }

    cipher_hex[j] = '\0';
}

void dec_msg(const char *cipher_hex, const uint key[4],
             uint *tab_log, uint *tab_exp,
             uint *prod_table, int rounds,
             char *msg) {

    size_t len = strlen(cipher_hex);

    // Ciphertext must contain complete blocks (4 hex digits)
    if (len % 4 != 0) {
        fprintf(stderr, "Invalid ciphertext length.\n");
        msg[0] = '\0';
        return;
    }

    size_t j = 0;

    for (size_t i = 0; i < len; i += 4) {

        uint state[4];

        // Convert hexadecimal characters into nibbles
        for (int k = 0; k < 4; k++) {
            char c = cipher_hex[i + k];

            if (c >= '0' && c <= '9')
                state[k] = c - '0';
            else if (c >= 'A' && c <= 'F')
                state[k] = c - 'A' + 10;
            else if (c >= 'a' && c <= 'f')
                state[k] = c - 'a' + 10;
            else {
                fprintf(stderr, "Invalid hexadecimal character.\n");
                msg[0] = '\0';
                return;
            }
        }

        // Decrypt block
        mini_aes_dec(state, key, tab_log, tab_exp, prod_table, rounds);

        // Reconstruct the original bytes
        msg[j++] = (char)((state[0] << 4) | state[1]);
        msg[j++] = (char)((state[2] << 4) | state[3]);
    }

    // Null terminate
    msg[j] = '\0';

    // Remove PKCS#7 padding
    if (j > 0) {
        unsigned char padding = (unsigned char)msg[j - 1];

        if (padding >= 1 && padding <= 2) {
            int valid = 1;

            for (size_t i = j - padding; i < j; i++) {
                if ((unsigned char)msg[i] != padding) {
                    valid = 0;
                    break;
                }
            }

            if (valid) {
                j -= padding;
                msg[j] = '\0';
            }
        }
    }
}