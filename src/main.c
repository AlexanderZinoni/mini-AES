#include "table.h"
#include "mini-AES.h"

int main(){
    size_t n = 4;
    uint ppoly = 0x13; // X^4 + X + 1

    uint *tab_exp = get_tab_exp(n, ppoly);
    uint *tab_log = fill_tab_log(n, tab_exp);
    uint *sum_table = fill_sum_lookup(n);
    uint *prod_table = fill_prod_lookup(n, tab_exp, tab_log);

    uint inv = inv_mod(0x2, tab_log, tab_exp, n);
    printf("Inverse de 0x2 = 0x%X\n", inv);
    print_mat(prod_table, 1<<n);

    free_table(tab_exp);
    free_table(tab_log);
    free_table(sum_table);
    free_table(prod_table);

    return 0;
}