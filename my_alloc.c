/*
 * segregated fits
 * operates within a 64 bit framework all addresses will have 16 as a factor
 *
 * the lower 4 bits of any address and size will be 0 because of the the 16 alignment
 * header layout:
 * 0	set if block is allocated
 * 4-63	upper bits of size in bytes including header
 *
 * 0	set if block is at the end of the list
 * 4-63	upper bits of the address of the next block in the list
 *
 * size classes are separated based on the leftmost 1 in the binary representation of size
*/
#include <stdlib.h>
#include <stdint.h>

static void *heap_base_ptr = NULL;//points to the base address of the heap
static size_t heap_size = 0;//size of heap in bytes
static void *heap_end_ptr = NULL;//points to the end of the heap this address itself is not writable

#define INI_HEAP_SIZE 1024//in bytes
#define HEADER_SIZE 16//in bytes

static void *size_class_arr[60];

typedef struct{
	size_t size;
	void *next_adr;
}header;

static size_t get_block_size(header *header_ptr)
{
	return header_ptr->size & (~0xf);
}

static int is_allocated(header *header_ptr)
{
	return header_ptr->size & 0x1;
}

static void *get_next_block_ptr(header *header_ptr)
{
	return ((void *) (((uintptr_t) header_ptr->next_adr) & (~0xf)));
}

static int is_list_end(header *header_ptr)
{
	return (uintptr_t) header_ptr->next_adr & 0x1;
}

static void set_block_allocated(header *header_ptr)
{
	header_ptr->size |= 0x1;
}

static void set_block_free(header *header_ptr)
{
	header_ptr->size &= ~0x1;
}

static void set_block_list_end(header *header_ptr)
{
	header_ptr->next_adr = (void *) ((uintptr_t) header_ptr->next_adr | 0x1);
}

static void unset_block_list_end(header *header_ptr)
{
	header_ptr->next_adr = (void *) ((uintptr_t) header_ptr->next_adr & ~0x1);
}

static void set_block_size(header *header_ptr, size_t size)
{
	header_ptr->size = size & (~0xf);
}

static void set_next_block_ptr(header *header_ptr, void *next_block_ptr)
{
	header_ptr->next_adr = (void *) (((uintptr_t) next_block_ptr) & (~0xf));
}

//converts a requested size to an aligned block size with at least req bytes of free space
static size_t get_aligned_size(size_t req)
{
	req += sizeof(header);

	if((req & 0xf) > 0){
		req += 0x10;//add to keep alignment
		req &= (~0xf);
	}

	return req;
}
