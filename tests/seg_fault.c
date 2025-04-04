/*
 * tests weather a seg fault will happen if buffer is written to
*/

#include "my_alloc.h"
#include "utils.h"

#define BUF_SIZE 1000
#define SEED 2342235

int main(void)
{
	void *ptr = my_alloc(BUF_SIZE);

	set_mem(ptr, SEED, BUF_SIZE);

	int ret_val = 0;

	my_free(ptr);

	return ret_val;
}
