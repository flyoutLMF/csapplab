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
    "none",
    /* First member's full name */
    "刘慕梵",
    /* First member's email address */
    "19307130248@fudan.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8//分别是单字双字的大小

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))//将大小和分配位合并，返回这个值

#define GET(p) (*(unsigned int*)(p))//返回p处的字
#define PUT(p, val) (*(unsigned int*)(p) = (val))//将val存放在p处

#define GET_SIZE(p) (GET(p) & ~0x7)//p是头部或脚部，从p处返回大小
#define GET_ALLOC(p) (GET(p) & 0x1)//p是头部或脚部，从p处返回分配位

#define HDRP(bp) ((char *)(bp) - WSIZE)//返回块的头部的指针
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)//返回块的尾部的指针

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE))//返回指向下一个块的指针
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE))//返回指向上一个块的指针

#define PAGESIZE mem_pagesize()

static char* heap_listp;

static void* extend_heap(size_t words);//拓展堆
static void* coalesce(void *bp);//合并空闲块
static void* find_hit(size_t asize);//找到合适的块
static void place(void* bp, size_t asize);//放置块
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
    {
        return -1;
    }
    PUT(heap_listp, 0);
    PUT(heap_listp + WSIZE, PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);

    if(extend_heap(PAGESIZE / DSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    if(size == 0)
        return NULL;
    size_t newsize = ALIGN(size + SIZE_T_SIZE);//对齐
    char *bp;
    
    if((bp = find_hit(newsize)) != NULL)//找到合适的块
    {
        place(bp, newsize);//放置块
        return bp;
    }

    //如果找不到，则拓展堆
    size_t extendsize = MAX(newsize, PAGESIZE);//拓展大小为两者较大值
    if((bp = extend_heap(extendsize / DSIZE)) == NULL)
        return NULL;
    place(bp, newsize);
    heap_listp = bp;
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));//设置分配位为0

    heap_listp = coalesce(ptr);//合并空闲块
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    heap_listp = newptr;
    if (newptr == NULL)
      return NULL;
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

static void* extend_heap(size_t words)//拓展堆
{
    char *bp;
    size_t size;

    size = (words % 2)? (words + 1) * DSIZE : words * DSIZE;//需要拓展的字节
    if((bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));//设置新分配的堆块的头部
    PUT(FTRP(bp), PACK(size, 0));//设置尾部
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));//设置堆末尾标识

    return coalesce(bp);//与前面的空闲块合并
}

static void* coalesce(void *bp)//合并空闲块
{
    size_t prev_alloc, next_alloc, size;

    prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));//前一个块的分配位
    next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));//后一个块的分配位
    size = GET_SIZE(HDRP(bp));//最后空闲块的大小

    if(prev_alloc && next_alloc)//独立的空闲块
        return bp;
    else if(!prev_alloc && next_alloc)//前面有空闲块而后面没有
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else if(prev_alloc && !next_alloc)//前面没有空闲块而后面有
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else//前后都有空闲块
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp))) + GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

static void* find_hit(size_t asize)//找到合适的块
{
    void *bp;
    size_t size;
    //首次适配
    for(bp = heap_listp; (size = GET_SIZE(HDRP(bp))) > 0; bp = NEXT_BLKP(bp))
    {
        if(!GET_ALLOC(HDRP(bp)) && size >= asize)
            return bp;
    }
    return NULL;
}

static void place(void* bp, size_t asize)//放置块
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t leftsize = size - asize;

    if(leftsize >= 2 * DSIZE)//如果分割后剩下的块大小大于等于最小块大小
    {
        PUT(HDRP(bp), PACK(asize, 1));//前面部分设置为已分配
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(leftsize, 0));//后面设置为空闲
        PUT(FTRP(bp), PACK(leftsize, 0));
    }
    else//否则整个块设置为已分配
    {
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
    }
}
