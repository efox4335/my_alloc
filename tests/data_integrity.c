/*
 * the test operates in 9 passes
 * 1st pass:
 * 	setting the block length between max block size and 1 bytes
 * 	setting the seed for that block
 * 	allocate each block
 *
 * 2nd pass:
 * 	set each blocks data
 *
 * 3rd pass:
 * 	check each blocks data
 *
 * 4th pass:
 * 	free every block that has a 1 as the rightmost bit
 *
 * 5th pass:
 * 	check each blocks data that is still allocated
 *
 * 6th pass:
 * 	reallocate previously freed blocks
 *
 * 8th pass:
 * 	set previously freed blocks
 *
 * 9th pass:
 * 	check each blocks data
*/

#include "my_alloc.h"
#include "utils.h"
#include <stdio.h>

#define MAX_BLOCK_SIZE 1000000
#define SEED 139485
#define BLOCK_COUNT 1000

#define ALLOCATED 1
#define FREE 0

static int step = 1;

typedef struct{
	unsigned char *ptr;
	size_t length;
	int seed;//seed passed to set_mem
	int is_allocated;
}block_metadata;

void allocate_block(block_metadata *block)
{
	block->ptr = my_alloc(block->length);
	block->is_allocated = ALLOCATED;
}

void free_block(block_metadata *block)
{
	my_free(block->ptr);
	block->is_allocated = FREE;
}

/*
 * only checks allocated blocks
*/
void check_blocks(block_metadata *block_arr, size_t length)
{
	for(size_t i = 0; i < length; ++i){
		if(block_arr[i].is_allocated == ALLOCATED){
			size_t bad_byte = check_mem(block_arr[i].ptr, block_arr[i].seed, block_arr[i].length);

			if(bad_byte != block_arr[i].length){
				printf("error bad data at byte %lu in block %lu at step %d\n", bad_byte, i, step);

				exit(1);
			}
		}
	}
}

void set_blocks(block_metadata *block_arr, size_t length)
{
	for(size_t i = 0; i < length; ++i){
		if(block_arr[i].is_allocated == ALLOCATED){
			set_mem(block_arr[i].ptr, block_arr[i].seed, block_arr[i].length);
		}
	}
}

void allocate_blocks(block_metadata *block_arr, size_t length)
{
	for(size_t i = 0; i < length; ++i){
		if(block_arr[i].is_allocated == FREE){
			allocate_block(&block_arr[i]);
		}
	}
}

/*
 * does not allocate blocks
*/
void initalize_block_arr(block_metadata *block_arr, size_t length, size_t max_size, int cur_seed)
{
	srand(cur_seed);

	for(size_t i = 0; i < length; ++i){
		block_arr[i].length = max_size - (rand() % max_size);
		block_arr[i].seed = rand();
		block_arr[i].is_allocated = FREE;
	}
}

void check_dup_ptr(block_metadata *block_arr, size_t length)
{
	for(size_t i = 0; i < length; ++i){
		for(size_t k = i + 1; k < length; ++k){
			if(block_arr[i].ptr == block_arr[k].ptr){
				printf("duplicate ptr in block %ld and %ld at step %d\n", i, k, step);
			}
		}
	}
}

int main(void)
{
	block_metadata block_arr[BLOCK_COUNT];

	initalize_block_arr(block_arr, BLOCK_COUNT, MAX_BLOCK_SIZE, SEED);
	allocate_blocks(block_arr, BLOCK_COUNT);
	++step;

	check_dup_ptr(block_arr, BLOCK_COUNT);

	set_blocks(block_arr, BLOCK_COUNT);
	++step;

	check_blocks(block_arr, BLOCK_COUNT);
	++step;

	for(size_t i = 0; i < BLOCK_COUNT; ++i){
		if((block_arr[i].seed & 1) == 1){
			free_block(&block_arr[i]);
		}
	}
	++step;

	check_blocks(block_arr, BLOCK_COUNT);
	++step;

	allocate_blocks(block_arr, BLOCK_COUNT);

	for(size_t i = 0; i < BLOCK_COUNT; ++i){
		if((block_arr[i].seed & 1) == 1){
			set_mem(block_arr[i].ptr, block_arr[i].seed, block_arr[i].length);
		}
	}
	++step;

	check_dup_ptr(block_arr, BLOCK_COUNT);

	check_blocks(block_arr, BLOCK_COUNT);

	return 0;
}
