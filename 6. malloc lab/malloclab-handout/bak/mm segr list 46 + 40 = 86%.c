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
# define heapcheck(lineno) printf("\n***Checkheap in func %s***\n", __func__); mm_heapcheck(lineno)
# define P(s) printf(s)
#else
# define heapcheck(lineno)
# define P(s)
#endif

/*** my define here ***/
#define WSIZE 4 
#define DSIZE 8
#define CHUNKSIZE (1<<12)
#define MIN_BLK_SIZE (2*DSIZE)

#define PACK(size, alloc) ((size) | (alloc))
#define GET(p)            (*(unsigned int *)(p))
#define PUT(p, val)       (*(unsigned int *)(p) = (unsigned int)(val))
#define GET_SIZE(p)       (GET(p) & ~0x7)
#define GET_ALLOC(p)      (GET(p) & 0x1)
#define HDRP(bp)          ((char *)(bp) - WSIZE)
#define FTRP(bp)          ((char *)(bp) + GET_SIZE(HDRP(bp)) -DSIZE)
#define NEXT_BLKP(bp)     ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)     ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Return the pointer to the prev/next free block int the explicit free list */
#define PREV_FREEB(bp)    ((void *)GET(bp))
#define NEXT_FREEB(bp)    ((void *)GET((char *)(bp) + WSIZE))
/* Return the prev/next pointer of the given bp */
#define PREVP(bp) (bp)
#define NEXTP(bp) ((char *)(bp) + WSIZE)

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
static inline void connect_block(void *, void *);
static inline void remove_from_list(void *);
static void insert_to_list(void *);
static inline int get_index(size_t);


static char *elist_head;

/*  slist[0-26]  ----   < 2^5 ~ < 2^31
 *  slist[27]    ----   > 2^31
 */
#define LIST_SIZE 28
static char *slist[LIST_SIZE];

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    int i;
    for (i = 0; i < LIST_SIZE; i++) slist[i] = NULL;

    if ((elist_head = mem_sbrk(6*WSIZE)) == (void *)-1) 
	return -1;
    PUT(elist_head, 0);                               /* Alignment padding */
    PUT(elist_head + (1*WSIZE), PACK(2*DSIZE, 1));    /* Prologue header */
    PUT(elist_head + (4*WSIZE), PACK(2*DSIZE, 1));    /* Prologue footer */
    PUT(elist_head + (5*WSIZE), PACK(0, 1));          /* Epilogue header */
    
    elist_head += (2*WSIZE);
    //connect_block(elist_head, elist_head);

    unsigned int *ptr;
    if ((ptr = extend_heap(CHUNKSIZE / WSIZE)) == NULL) 
	return -1;

//heapcheck(__LINE__);

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
//printf("malloc size: %u\n", size);
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
heapcheck(__LINE__);
	return bp;
    } 
    /* No fit found. Get more memory and place the block */
    else {
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
	    return NULL;
	place(bp, asize);
heapcheck(__LINE__);
	return bp;
    }


}

/*
 * mm_free - Free a block
 */
void mm_free(void *ptr) 
{
P("***free***\n");
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);

//heapcheck(__LINE__);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
//heapcheck(__LINE__);
//printf("~~~~in realloc~~~\n");
//printf("bp size: %u,  require size: %u\n", GET_SIZE(HDRP(ptr)), size);

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
	return place(ptr, asize);
    }

    /* asize > osize */
    else 
        return true_realloc(ptr, asize);
}

/* realloc the memory in terms of mm_malloc and mm_free directly */
static void *true_realloc(void *ptr, size_t size)
{
P("***true realloc***\n");
    size_t p_size = GET_SIZE(HDRP(ptr));
    size_t asize = ALIGN(size);
    char *prev = PREV_BLKP(ptr);
    char *next = NEXT_BLKP(ptr);
    size_t prev_size = GET_SIZE(HDRP(prev));
    size_t next_size = GET_SIZE(HDRP(next)); 

    /* First choice: prev and next block are both free and big enough */
    if (!GET_ALLOC(HDRP(prev)) && !GET_ALLOC(HDRP(next)) 
	    && p_size + prev_size + next_size >= asize) {
	remove_from_list(prev);
	remove_from_list(next);

	if (p_size + prev_size + next_size - asize >= MIN_BLK_SIZE) {
	    memmove(prev, ptr, p_size - 2*WSIZE);
	    char *rbp = (char *)prev + asize;
	    PUT(HDRP(prev), PACK(asize, 1));
	    PUT(FTRP(prev), PACK(asize, 1));
	    PUT(HDRP(rbp), PACK(p_size + prev_size + next_size - asize, 0));
	    PUT(FTRP(rbp), PACK(p_size + prev_size + next_size - asize, 0));
	    insert_to_list(rbp);
	    return prev;

	} else {
	    PUT(HDRP(prev), PACK(p_size + prev_size + next_size, 1));
	    PUT(FTRP(next), PACK(p_size + prev_size + next_size, 1));
	    memmove(prev, ptr, p_size - 2*WSIZE);
	    return prev;
	}

    }

    /* Next block is free and big enough */
    else if (!GET_ALLOC(HDRP(next)) && next_size + p_size >= asize) {
	remove_from_list(next);

	if (next_size - asize + p_size >= MIN_BLK_SIZE) {
	    char *rbp = (char *)ptr + asize;
	    PUT(HDRP(ptr), PACK(asize, 1));
	    PUT(FTRP(ptr), PACK(asize, 1));
	    PUT(HDRP(rbp), PACK(next_size - asize + p_size, 0));
	    PUT(FTRP(rbp), PACK(next_size - asize + p_size, 0));
	    insert_to_list(rbp);
	    return ptr;
	} else {
	    PUT(HDRP(ptr), PACK(p_size + next_size, 1));
	    PUT(FTRP(next), PACK(p_size + next_size, 1));
	    return ptr;
	}
    }

    /* Prev block is free and big enough */
    else if (!GET_ALLOC(HDRP(prev)) && prev_size + p_size >= asize) {
	remove_from_list(prev);

	if (p_size + prev_size - asize >= MIN_BLK_SIZE) {
	    memmove(prev, ptr, p_size - 2*WSIZE);
	    char *rbp = (char *)prev + asize;
	    PUT(HDRP(prev), PACK(asize, 1));
	    PUT(FTRP(prev), PACK(asize, 1));
	    PUT(HDRP(rbp), PACK(p_size + prev_size - asize, 0));
	    PUT(FTRP(rbp), PACK(p_size + prev_size - asize, 0));
	    insert_to_list(rbp);
	    return prev;

	} else {
	    PUT(HDRP(prev), PACK(p_size + prev_size, 1));
	    PUT(FTRP(prev), PACK(p_size + prev_size, 1));
	    memmove(prev, ptr, p_size - 2*WSIZE);
	    return prev;
	}
    }

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
//heapcheck(__LINE__);
    return newptr;
}

static void *extend_heap(size_t words) 
{
//printf("~~~~in extend heap~~~\n");
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
	return NULL;

    PUT(HDRP(bp), PACK(size, 0));    /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));    /* Free block footer */

    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  /* New epiplogue header */
//printf("\n elist_head prev:%x, next:%x", GET(PREVP(elist_head)), GET(NEXTP(elist_head)));
//printf("\n elist_head nextBlock:%x", NEXT_FREEB(elist_head));
//heapcheck(__LINE__);

    //return bp;    
/* Coalesce if the previous block was free */
    return coalesce(bp);
}

static void *coalesce(void *bp)
{
//P("~~~~in coalesce~~~\n");
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));


    if (prev_alloc && !next_alloc) {        /* Case 2: next block is free */
	/* Remove the next from the free list */
//printf("NEXT Block addr: 0x%x  Size: %d Alloc: %x \n", 
//		(unsigned int)NEXT_BLKP(bp), (size_t)GET_SIZE(HDRP(NEXT_BLKP(bp))), 
//		GET_ALLOC(HDRP(NEXT_BLKP(bp))));

	remove_from_list(NEXT_BLKP(bp));

	size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {        /* Case 3: prev block is free */
	/* Remove the prev from the free list */
	remove_from_list(PREV_BLKP(bp));

	size += GET_SIZE(HDRP(PREV_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    else if (!prev_alloc && !next_alloc) {       /* Case 4: prev and next block are free */
	/* Remove the prev, the next from the free list */
	remove_from_list(PREV_BLKP(bp));
	remove_from_list(NEXT_BLKP(bp));

	size += GET_SIZE(HDRP(PREV_BLKP(bp))) + 
		GET_SIZE(FTRP(NEXT_BLKP(bp)));
	PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
	PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
	bp = PREV_BLKP(bp);
    }

    insert_to_list(bp);
    return bp;
}

static void *find_fit(size_t asize) 
{
    /* first fit */
    int index = get_index(asize);

    while (index < LIST_SIZE) {
	if (slist[index] != NULL) {
	    if (GET_SIZE(HDRP(slist[index])) >= asize) return slist[index];

	    char *bp = NEXT_FREEB(slist[index]);
	    for (; bp != slist[index]; bp = NEXT_FREEB(bp)) {
		if (GET_SIZE(HDRP(bp)) >= asize)
	        return bp;
	    }
	}
	index++;   
    } 

    return NULL;

}

/* Place asize memory from free block bp */
static void *place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t alloc = GET_ALLOC(HDRP(bp));
    if (size - asize >= MIN_BLK_SIZE) {
	if (!alloc) remove_from_list(bp);	

	char *rbp = (char *)bp + asize;
	PUT(HDRP(bp), PACK(asize, 1));
	PUT(FTRP(bp), PACK(asize, 1));
	PUT(HDRP(rbp), PACK(size - asize, 0));
	PUT(FTRP(rbp), PACK(size - asize, 0));

	insert_to_list(rbp);
	//coalesce(rbp);
    } else {
	if (!alloc) remove_from_list(bp);
	PUT(HDRP(bp), PACK(size, 1));
	PUT(FTRP(bp), PACK(size, 1));
    }
    return bp;
}

static inline void connect_block(void *b1, void *b2)
{
    PUT(NEXTP(b1), b2);
    PUT(PREVP(b2), b1);
}

static inline void remove_from_list(void *bp)
{
//printf("~~~~going to REMOVE 0x%x, size:%u \n", (size_t)bp, GET_SIZE(HDRP(bp)));
    int index = get_index(GET_SIZE(HDRP(bp)));
    char* prev = PREV_FREEB(bp);
    char* next = NEXT_FREEB(bp);
    if (slist[index] == bp) {
	if (next == bp) {
	    slist[index] = NULL;
	    return;
	} else 
	    slist[index] = next;
    }
    connect_block(prev, next);
}

static void insert_to_list(void *bp)
{
//printf("~~~~going to INSERT 0x%x, size:%u \n", (size_t)bp, GET_SIZE(HDRP(bp)));    
    int index = get_index(GET_SIZE(HDRP(bp)));

    if (slist[index] != NULL) {
	connect_block(PREV_FREEB(slist[index]), bp);
	connect_block(bp, slist[index]);
	slist[index] = bp;
    } else {
	slist[index] = bp;
	connect_block(bp, bp);
    }
}

static inline int get_index(size_t size)
{
    int i = 5;
    while ((size >> i) != 0) i++;
    return i - 5;

}

void mm_heapcheck(int lineno) {
    // TO DO
    //printf("heapcheck at line %d\n", lineno);
    //printf("heap_size: 0x%x\n", mem_heapsize());
    //printf("heap_lo: 0x%x, heap_hi: 0x%x\n", 
//	      (unsigned int) mem_heap_lo(), (unsigned int) mem_heap_hi());

    /* Check all the blocks 
    printf("All the Blocks : \n");
    char *bp;
    for (bp = elist_head; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
	printf("    Block addr: 0x%x  Size: %d Alloc: %x \n", 
		(unsigned int)bp, (size_t)GET_SIZE(HDRP(bp)), GET_ALLOC(HDRP(bp)));
    }
*/
    /* Check the explicit free list */
    printf("Blocks in the Free List: \n");
    int i;
    for (i = 0; i < LIST_SIZE; i++) {
	if (slist[i] == NULL) continue;
	printf("  Slist[%d]:\n", i);
	printf("    Block addr: 0x%x  Size: %d Alloc: %x \n", 
		(unsigned int)slist[i], (size_t)GET_SIZE(HDRP(slist[i])), 
		GET_ALLOC(HDRP(slist[i])));

size_t size = (size_t)GET_SIZE(HDRP(slist[i]));
if (i != get_index(size)) {
    printf("SIZE ERROR!!!\ni = %d, size = %u\n", i, size);
    exit(0);
}
	char *bp = NEXT_FREEB(slist[i]);

	for (; bp != slist[i]; bp = NEXT_FREEB(bp)) {
	    printf("    Block addr: 0x%x  Size: %d Alloc: %x \n", 
		(unsigned int)bp, (size_t)GET_SIZE(HDRP(bp)), 
		GET_ALLOC(HDRP(bp)));

size_t size = (size_t)GET_SIZE(HDRP(bp));
if (i != get_index(size)) {
    printf("SIZE ERROR!!!\ni = %d, size = %u\n", i, size);
    exit(0);
}
	} 

    }


}






