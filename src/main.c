#include "table.h"
#include "mini-AES.h"


int main(){
    size_t n = 4;
    uint ppoly = 0x13; // X^4 + X + 1, (19 in decimal)

    // Initialise lookup tables
    uint *tab_exp = fill_tab_exp(n, ppoly);
    uint *tab_log = fill_tab_log(n, tab_exp);
    uint *prod_table = fill_prod_lookup(n, tab_exp, tab_log);

    char msg[] = "Very secret message. ";
    size_t len = strlen(msg);
    size_t padded_len = len + (2 - len % 2);
    char cipher_hex[2 * padded_len + 1];
    char decrypted[padded_len + 1];

    uint key[4] = {0x1, 0x3, 0x0, 0xE};

    int rounds = 16;

    enc_msg(msg, key, tab_log, tab_exp, prod_table, rounds, cipher_hex);
    dec_msg(cipher_hex, key, tab_log, tab_exp, prod_table, rounds, decrypted);

    printf("Original:  %s\n", msg);
    printf("Encrypted: %s\n", cipher_hex);
    printf("Decrypted: %s\n", decrypted);

    free_table(tab_exp);
    free_table(tab_log);
    free_table(prod_table);

    return 0;
}
