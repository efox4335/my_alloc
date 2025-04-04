#include <stdlib.h>

/*
 * sets a chunk of memory according to rand function with seed in seed arg
 * done so checking data integrity can be done by just storing the seed instead of an entire chunk
 * length is in bytes
 */
void set_mem(unsigned char *restrict ptr, int seed, size_t length);

/*
 * returns length if section is valid invalid byte if not
 * length is in bytes
 * seed should be the same passed to set_mem
 */
int check_mem(unsigned char *restrict ptr, int seed, size_t length);
