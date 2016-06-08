/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Qteam2",
    /* First member's full name */
    "qquan",
    /* First member's email address */
    "qq@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

//#define DEBUG

#ifdef DEBUG
# define heapcheck(lineno) printf("%s\n", __func__); mm_heapcheck(lineno)
#else
# define heapcheck(lineno)
#endif

/*** my define here ***/
#define WSIZE 4 
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MIN_BLK_SIZE (2*DSIZE)

#define PACK(size, alloc) ((size) | (alloc))
#define GET(p)            (*(unsigned int *)(p))
#define PUT(p, val)       (*(unsigned int *)(p) = (val))
#define GET_SIZE(p)       (GET(p) & ~0x7)
#define GET_ALLOC(p)      (GET(p) & 0x1)
#define HDRP(bp)          ((char *)(bp) - WSIZE)
#define FTRP(bp)          ((char *)(bp) + GET_SIZE(HDRP(bp)) -DSIZE)
#define NEXT_BLKP(bp)     ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)     ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

void mm_heapcheck(int);
static void *true_realloc(void *, size_t);
static void *extend_heap(size_t);
static void *coalesce(void *);
static void *find_fit(size_t);
static void *place(void *, size_t);

static char *heap_listp;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
//P("**in init**\n");
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) 
	return -1;
    PUT(heap_listp, 0);                               /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));      /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));      /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));          /* Epilogue header */
    
    heap_listp += (2*WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL) 
	return -1;

heapcheck(__LINE__);

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
heapcheck(__LINE__);

    size_t asize;        /* Adjusted block size */
    size_t extendsize;   /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
	return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    asize = ALIGN(size + 2 * WSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
	place(bp, asize);
	return bp;
    } 
    /* No fit found. Get more memory and place the block */
    else {
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
	    return NULL;
	place(bp, asize);
	return bp;
    }

heapcheck(__LINE__);
/*   old version
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if (p == (void *)-1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
*/
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr) 
{
    //heapcheck(__LINE__);
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) return mm_malloc(size);
    if (size == 0) {
	mm_free(ptr);
	return NULL;
    }

    size_t osize = GET_SIZE(HDRP(ptr));
    size_t asize = ALIGN(size + 2*WSIZE);

    /* no need to realloc if equal */
    if (asize == osize) return ptr;    

    /* new asize is smaller than original size, directly use place() */
    else if (asize < osize) {
	//printf("****new size : %u, o size : %u***\n", size, osize);
	return place(ptr, asize);
    }

    /* asize > osize */
    else 
        return true_realloc(ptr, size);
}

/* realloc the memory in terms of mm_malloc and mm_free directly */
static void *true_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    //copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}
static void *extend_heap(size_t words) 
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
	return NULL;

    PUT(HDRP(bp), PACK(size, 0));    /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));    /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  /* New epiplogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {              /* Case 1 */
	return bp;
    }

    else if (prev_alloc && !next_alloc) {        /* Case 2 */
	size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {        /* Case 3 */
	size += GET_SIZE(HDRP(PREV_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    else if (!prev_alloc && !next_alloc) {       /* Case 4 */
	size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
		GET_SIZE(FTRP(NEXT_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    return bp;
}

static void *find_fit(size_t asize) 
{
    /* Best fit 
    char *bp; 
    char *rst = NULL;
    size_t dsize = 0xffffffff;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize 
				&& GET_SIZE(HDRP(bp)) - asize < dsize) {
	    rst = bp;
	    dsize = GET_SIZE(HDRP(bp)) - asize;
	    printf("%u\n", dsize);
	    if (dsize == 0) return bp;
	}
    }
    return rst;
*/
    /* first fit */
    char *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= asize)
	    return bp;
    }
    return NULL;

}

static void *place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    if (size - asize >= MIN_BLK_SIZE) {
	char *rbp = (char *)bp + asize;
	PUT(HDRP(bp), PACK(asize, 1));
	PUT(FTRP(bp), PACK(asize, 1));
	PUT(HDRP(rbp), PACK(size - asize, 0));
	PUT(FTRP(rbp), PACK(size - asize, 0));
    } else {
	PUT(HDRP(bp), PACK(size, 1));
	PUT(FTRP(bp), PACK(size, 1));
    }
    return bp;
}

void mm_heapcheck(int lineno) {
    // TO DO
    printf("heapcheck at line %d\n", lineno);
    printf("heap_size: 0x%x\n", mem_heapsize());
    printf("heap_lo: 0x%x, heap_hi: 0x%x\n", 
	      (unsigned int) mem_heap_lo(), (unsigned int) mem_heap_hi());
    char *bp;
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	printf("Block addr: 0x%x  Size: %d Alloc: %x \n", 
		(unsigned int)bp, (size_t)GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
    }


}










