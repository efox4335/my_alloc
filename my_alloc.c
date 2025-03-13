/*
 * segregated fits
 * operates within a 64 bit framework all addresses will have 16 as a factor
 *
 * the lower 4 bits of any address and size will be 0 because of the the 16 alignment
 * header layout:
 * 0	allocated status
 * 4-63	upper bits of size in bytes
 *
 * 0	set if block is at the end of the list
 * 4-63	upper bits of the address of the next block in the list
 *
 * size classes are separated based on the leftmost 1 in the binary representation of size
*/
#include <stdlib.h>

static void *heap_base_ptr = NULL;//points to the base address of the heap
static size_t heap_size = 0;//size of heap in bytes
static void *heap_end_ptr = NULL;//points to the end of the heap this address itself is not writable

#define INI_HEAP_SIZE = 1024//in bytes

#define HEADER_SIZE = 16//in bytes

static void *size_class_arr[60];
