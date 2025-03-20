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
#include <unistd.h>

#define SIZE_CLASS_COUNT 60

static void *heap_base_ptr = NULL;//points to the base address of the heap
static size_t heap_size = 0;//size of heap in bytes
static void *heap_end_ptr = NULL;//points to the end of the heap this address itself is not writable

#define INI_HEAP_SIZE 1024//in bytes
#define HEADER_SIZE 16//in bytes

typedef struct{
	size_t size;
	void *next_adr;
}header;

static header *size_class_arr[SIZE_CLASS_COUNT];

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

//gets the pointer to the byte after the header
static void *get_return_ptr(header *cur_block)
{
	return (void *) (((uintptr_t) cur_block) + sizeof(header));
}

//gets header pointer from pointer returned to my_alloc
static header *get_block_ptr(void *ptr)
{
	return (header *) (((uintptr_t) ptr) - sizeof(header));
}

static int get_size_class_index(size_t size)
{
	int leftmost_ind = 0;
	int iter_count = 0;

	for(int i = 1 << 3; i > 0; i <<= 1){
		if((i & size) > 0){
			leftmost_ind = iter_count;
		}

		++iter_count;
	}

	return leftmost_ind;
}

/*
 * removes cur_block from size class size_class
 * if prev block is NULL if cur_block is the first in the list
 * marks cur_block as allocated
 * unsets list end
*/
static void remove_block(header *cur_block, header *prev_block, size_t size_class)
{
	header *next_block;

	if(is_list_end(cur_block)){
		next_block = NULL;
	}else{
		next_block = get_next_block_ptr(cur_block);
	}

	if(prev_block == NULL){
		size_class_arr[size_class] = next_block;
	}else{
		set_next_block_ptr(prev_block, next_block);

		if(is_list_end(cur_block)){
			set_block_list_end(prev_block);
		}
	}

	unset_block_list_end(cur_block);
	set_block_allocated(cur_block);
}

/*
 * sets block as unallocated
 */
static void insert_block(header *cur_block, int size_class)
{
	set_block_free(cur_block);
	header *next_block = size_class_arr[size_class];

	size_class_arr[size_class] = cur_block;

	set_next_block_ptr(cur_block, next_block);

	if(next_block == NULL){
		set_block_list_end(cur_block);
	}
}

/*
 * gets block at least req_size bytes from the approiate size class
 * returns null of no block is found
*/
static header *get_block(size_t req_size)
{
	int size_class_index = get_size_class_index(req_size);

	if(size_class_arr[size_class_index] == NULL){
		return NULL;
	}

	header *prev_block = NULL;
	header *cur_block = size_class_arr[size_class_index];

	while(1){
		if(get_block_size(cur_block) >= req_size){
			remove_block(cur_block, prev_block, size_class_index);

			return cur_block;
		}

		if(is_list_end(cur_block)){
			break;
		}

		prev_block = cur_block;
		cur_block = get_next_block_ptr(cur_block);
	}

	return NULL;
}

/*
 * sets size
 * sets allocated bit
*/
static header *alloc_new_block(size_t size)
{
	header *new_block_ptr = heap_end_ptr;
	sbrk(size);

	heap_end_ptr = (void *) (((uintptr_t) heap_end_ptr) + size);

	set_block_size(new_block_ptr, size);
	set_block_allocated(new_block_ptr);

	return new_block_ptr;
}

void *my_alloc(size_t size)
{
	size_t req_block_size = get_aligned_size(size);

	//heap initialization
	if(heap_base_ptr == NULL){
		heap_base_ptr = sbrk(0);

		heap_end_ptr = heap_base_ptr;

		heap_size = 0;

		for(int i = 0; i < SIZE_CLASS_COUNT; ++i){
			size_class_arr[i] = NULL;
		}
	}

	header *cur_block = get_block(req_block_size);

	if(cur_block == NULL){
		cur_block = alloc_new_block(req_block_size);
	}

	return get_return_ptr(cur_block);
}

void my_free(void *ptr)
{
	header *cur_block = get_block_ptr(ptr);
	size_t size = get_block_size(cur_block);
	int size_class = get_size_class_index(size);

	insert_block(cur_block, size_class);
}
