#ifndef MINI_AES
#define MINI_AES

#include "table.h"

uint mat_vec_prod(size_t taille, uint vec);

uint sub_byte(uint byte, size_t n, uint *tab_log, uint *tab_exp);

void SubBytes(size_t n, uint *tab_log, uint *tab_exp, uint *state_mat);

void ShiftRows(uint *state_mat);

void MixColumns(uint *stat_mat, uint ppoly);

void AddRoundKey(uint *state_mat, uint *key_mat);

#endif