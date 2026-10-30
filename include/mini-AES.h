#ifndef MINI_AES
#define MINI_AES

#include "table.h"

void print_state(uint s[4]);

uint mat_vec_prod(size_t taille, uint vec, const uint mat[4]);

uint sub_byte(uint byte, size_t n, uint *tab_log, uint *tab_exp);

uint inv_sub_byte(uint byte, size_t n, uint *tab_log, uint *tab_exp);

void SubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat);

void InvSubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat);

void ShiftRows(uint *state_mat);

void MixColumns(uint *stat_mat, uint ppoly);

void AddRoundKey(uint *state_mat, uint *key_mat);

int get_c(int i);

void next_key(uint *key_mat, int i, uint *tab_log, uint *tab_exp);

void mini_aes_enc(uint msg[4], uint key[4], uint *tab_log, uint *tab_exp, uint ppoly, int rounds);

void mini_aes_dec(uint msg[4], uint key[4], uint *tab_log, uint *tab_exp, uint ppoly, int rounds);

#endif