#include <stdlib.h>

/*
 * sets a chunk of memory according to rand function with seed in seed arg
 * done so checking data integrity can be done by just storing the seed instead of an entire chunk
 * length is in bytes
 */
static void set_mem(char *restrict ptr, int seed, size_t length)
{
	srand(seed);

	int cur_int = rand();
	unsigned char cur_byte;
	int cur_part = 0;

	for(size_t i = 0; i < length; ++i){
		cur_byte = cur_int;
		cur_int >>= 8;
		++cur_part;

		ptr[i] = cur_byte;

		if(cur_part >= 4){
			cur_int = rand();
			cur_part = 0;
		}
	}
}

/*
 * returns 1 if section is valid 0 if not
 * length is in bytes
 * seed should be the same passed to set_mem
 */
static int check_mem(char *restrict ptr, int seed, size_t length)
{
	srand(seed);

	int cur_int = rand();
	unsigned char cur_byte;
	int cur_part = 0;

	for(size_t i = 0; i < length; ++i){
		cur_byte = cur_int;
		cur_int >>= 8;
		++cur_part;

		if(ptr[i] != cur_byte){
			return i;
		}

		if(cur_part >= 4){
			cur_int = rand();
			cur_part = 0;
		}
	}

	return length;
}
