#ifndef XMEM_H
#define XMEM_H

/* total size of the managed dynamic memory in quarter-KBs (256-byte pages) */
#define MEMORY_SIZE_QKB  42

/* 2^(XMEM_ALIGN) bytes alignment boundary when allocating memory blocks */
/* this typically refers to the data bus width of the processor */
#define XMEM_ALIGN  0

#include <stdint.h>
#include <stddef.h>

typedef unsigned char byte;
typedef size_t size_x;

#ifndef MEMORY_SIZE_QKB
#error "MEMORY_SIZE_QKB definition is required with total dynamic memory size in quarter-KBs"
#endif

/* global MEMORY[] array */
#if XMEM_ALIGN > 0
__attribute__ ((aligned(1ul << XMEM_ALIGN)))
#endif
byte MEMORY[MEMORY_SIZE_QKB * 256ul];

uint32_t defrag_run;    /* counter of performed defragmentations (mostly for testing) */

/* initialise or reinitialise the entire memory */
/* needs to be called initially before referring to any other xmem function */
/* will return the size of the memory */
size_x x_meminit(void);

/* allocate block or change size of already allocated one */
/*
    when calling for initial allocation the variable must be previously initialised with NULL
    the new memory block is cleared
    if the block is being extended, the extension area is cleared
    will update the supplied variable with the pointer, or NULL in case of unsuccessful allocation
	The function usually returns pointer to the memory block with the exception of cases when a
	block is already allocated and needs changing its size. If unsuccessful, the function will
	return NULL but the original block will not be affected
*/
void *x_malloc(byte **var, size_x sz);

/* free allocated block */
/*
    will do nothing if the parameter doesn't point to a valid allocated block
    will update the variable with NULL
	return 0 if successful, or -1 for error
*/
int x_free(byte **var);

/* return the actual size of an allocated block */
size_x x_blksize(byte *v);

/* return the size of the largest continuous currently available block */
size_x x_avail(void);

/* return the total size of the currently available memory (could be fragmented in many separate blocks) */
size_x x_total(void);

/* optimise the memory and try to free up a block with size (sz) or more */
/* NOTE: this is typically an internal function, but can be called externally in case of a need */
void x_defrag(void);

/* ultra-aggressive defragmenation which reorders all headers and data blocks */
/* not called automatically; call at own risk */
/* address of all allocated data blocks are updated, however, any pointers held in external variables will become invalid */
void x_defrag_ultra(void);

/* list all currently allocated blocks (mainly for tracking down memory leaks) */
void x_list_alloc(void);

/* set name to an allocated block */
/* will return -1 if unsuccessful, or 0 in case of success */
/* block names are optional and with main purpose of tracking down memory leaks */
int x_setname(byte *v, char *name);

/* return the name of a block or NULL */
char *x_getname(byte *v);

#endif /* XMEM_H */
