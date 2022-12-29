#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "xmem.h"

#define XBLK_NAME_LEN   0   /* maximum length of block names */
#define SMALL_SCREEN    1   /* keep at 0 unless using a 20-character (or less) wide screen */

#define pTOP (xhdr_t *) (pMEMORY + xmem_bytes)  /* pointer to the top of memory */

/* memory block header structure */
typedef struct {
    void **cvar;   /* C variable */
	byte *data;    /* pointer to the allocated data block */
	size_x len;    /* length of the allocated block */
    #if XBLK_NAME_LEN > 0
        char name[XBLK_NAME_LEN + 1];    /* up to four letters for name (optional) */
    #endif
} xhdr_t;

/* block headers */
static byte *dcur;		/* points to the first unallocated data byte */
static xhdr_t *hcur;	/* points to the current last header */

byte *pMEMORY;  /* dynamic memory array */
//unsigned short defrag_cnt = 0;


size_x x_meminit(void) {
    xmem_bytes = (MEMORY_SIZE_QKB * 256ul);
    pMEMORY = MEMORY;
    memset((byte *) pMEMORY, 0, (size_t) xmem_bytes);
    dcur = (byte *) pMEMORY;
    hcur = pTOP;
    defrag_run = 0;
    return (size_x) xmem_bytes;
}


/* internal function */
/* find header based on data pointer; return NULL if the header can not be found */
xhdr_t *findxhdr(byte *d) {
    if(d) {
        xhdr_t *h = pTOP;
        while(--h >= hcur) {
            if(h->cvar && h->data == d) return h;
        }
    }
    return NULL;    /* unknown header (memory leak?) */
}


size_x x_blksize(byte *v) {
    xhdr_t *h = findxhdr(v);
    return (size_x) (h ? h->len : 0);
}


int x_setname(byte *v, char *name) {
    xhdr_t *h = findxhdr(v);
    if(h == NULL) return -1;
    #if XBLK_NAME_LEN > 0
        strncpy(h->name, name, XBLK_NAME_LEN);
        h->name[XBLK_NAME_LEN] = 0; /* ensure name termination */
    #endif
    return 0;
}


char *x_getname(byte *v) {
    xhdr_t *h = findxhdr(v);
    #if XBLK_NAME_LEN > 0
        return (h ? h->name : NULL);
    #else
        return NULL;
    #endif
}


size_x x_avail(void) {
    size_x t = (size_x) ((byte *) hcur - dcur);
    xhdr_t *h = pTOP;
    while(--h >= hcur) {
        if(h->cvar == NULL && h->len > t) t = h->len;
    }
    return t;
}


size_x x_total(void) {
    size_x t = (size_x) ((byte *) hcur - dcur);
    xhdr_t *h = pTOP;
    while(--h >= hcur) {
        if(h->cvar == NULL) t += h->len;
    }
    return t;
}


void x_list_alloc(void) {
    xhdr_t *p, *t, *h = pTOP;
    #if !SMALL_SCREEN
        long c = 0;
        size_x used = 0;
        while(--h >= hcur) {
            if(h->cvar) used += h->len;
        }
        printf("\r\nalloc: %li,  unalloc: %li,  largavl: %li,  totavl: %li",
                            (long) used, (long) ((byte *) hcur - dcur),
                            (long) x_avail(), (long) x_total());
    #endif
    printf("\r\n");
    h = pTOP;
    while(--h >= hcur) {
        if(h->data == pMEMORY) break;
    }
    if(h < hcur) return;
    unsigned short num = 0, err = 0;
    for( ; h; ) {
        num++;
        #if !SMALL_SCREEN
            #if XBLK_NAME_LEN > 0
                printf("%3li: H:%8p,  V:%8p,  M:%8p..%8p,  L:%6li,  N:%s\r\n",
            #else
                printf("%3li: H:%8p,  V:%8p,  M:%8p..%8p,  L:%6li\r\n",
            #endif
        #else
            #if XBLK_NAME_LEN > 0
                printf("%4p:%4p:%4li:%s\r\n",
            #else
                printf("%4p:%4p:%4p:%4li\r\n",
            #endif
        #endif
            #if !SMALL_SCREEN
                (long) ++c,
            #endif
            (void *) h,
            #if !SMALL_SCREEN || XBLK_NAME_LEN == 0
                (void *) h->cvar,
            #endif
            (void *) h->data,
            #if !SMALL_SCREEN
                (void *) (h->data + h->len - 1),
            #endif
            (long) h->len
            #if XBLK_NAME_LEN > 0
                , h->name
            #endif
		);
		p = NULL;
		t = pTOP;
		while(--t >= hcur) {    /* find the next block immediately after (h->data) */
			if(t->data > h->data && (p == NULL || t->data < p->data)) p = t;
		}
		if(p && (h->data + h->len) != p->data) {  /* memory leak! */
            err++;
            #if !SMALL_SCREEN
                #if XBLK_NAME_LEN > 0
                    printf("%3li: ..........,  V:%8p,  M:%8p..%8p,  L:%6li,  N:%s\r\n",
                #else
                    printf("%3li: ..........,  V:%8p,  M:%8p..%8p,  L:%6li\r\n",
                #endif
            #else
                #if XBLK_NAME_LEN > 0
                    printf(".........:%4li:%s\r\n",
                #else
                    printf("....:%4p:%4p:%4li\r\n",
                #endif
            #endif
                #if !SMALL_SCREEN
                    (long) ++c,
                #endif
                #if !SMALL_SCREEN || XBLK_NAME_LEN == 0
                    (void *) h->cvar,
                    (void *) (h->data + h->len),
                #endif
                #if !SMALL_SCREEN
                    (void *) (p->data - 1),
                #endif
                (long) (p->data - (h->data + h->len))
                #if XBLK_NAME_LEN > 0
                    , h->name
                #endif
			);
		}
        h = p;
        #if SMALL_SCREEN
            getchar();
        #endif
    };
    #if SMALL_SCREEN
        printf("-- %u blk, %u lks\r\n", num, err);
        getchar();
    #else
        printf("%-- u blocks, %u leaks\r\n", num, err);
    #endif
}


/* aggressive memory optimisation by full reorder */
void x_defrag_ultra(void) {
    defrag_run++;
    xhdr_t *h = pTOP;
    xhdr_t *t = h;
    while(--h >= hcur) {        /* record only the occupied headers */
        if(h->cvar) memmove(--t, h, sizeof(xhdr_t));
    }
    hcur = t;
    dcur = pMEMORY;
    if(hcur == pTOP) return;    /* all gone */
    while(1) {
        t = NULL;
        h = pTOP;
        while(--h >= hcur) {    /* find the block with the lowest address */
            if(h->data >= dcur) {
                if(t == NULL || h->data <= t->data) t = h;
            }
        }
        if(t == NULL) break;
        if(dcur != t->data) {
            byte *td = t->data;
            t->data = dcur;
            *(t->cvar) = dcur;
            memmove(dcur, td, (size_t) t->len);
        }
        dcur += t->len;
    }
}


/* optimise the memory */
void x_defrag(void) {
    defrag_run++;
    unsigned char rf = 0;
    xhdr_t *t, *h = pTOP;
    while(--h >= hcur) {
        rf = 0;
        t = pTOP;
        while(--t >= hcur) {
            if((h->data + h->len) == t->data && t-> len && t->cvar == NULL) {   /* the next block is free */
                if(h->cvar == NULL || h->len >= (t->len * 3)) {
                    h->len += t->len;   /* combine the two blocks */
                    t->len = 0;         /* mark the header for removal */
                    t = pTOP;           /* restart the inner loop */
                    rf = 1;
                }
                else break;
            }
        }
        if(rf) h = pTOP;    /* restart the outer loop */
    }
    dcur = pMEMORY;
    h = pTOP;
    while(--h >= hcur) {    /* find the top of actual data blocks */
        if(h->cvar && (h->data + h->len) > dcur) dcur = h->data + h->len;
    }
    t = h = pTOP;
    while(--h >= hcur) {    /* remove the marked headers (those with length 0) */
        if(h->len && h->data < dcur) memmove(--t, h, sizeof(xhdr_t));
    }
    hcur = t;
}


int x_free(byte **var) {
    if(var == NULL || (*var == NULL)) return 0;
    if((*var < pMEMORY) || (*var > (byte *) pTOP)) return -1;
    xhdr_t *h = findxhdr(*var);
    if(h == NULL) return -1;
    *var = NULL;
    h->cvar = NULL;     /* mark the block as free */
    //if((++defrag_cnt % 31) == 0) x_defrag();    /* run memory defragmentation from time to time */
    return 0;
}


void *x_malloc(byte **var, size_x sz) {
    if(var == NULL || sz == 0) { x_free(var); return NULL; }    /* size 0 is request to release the block */
    #if SIZE_MAX > LONG_MAX
        if(sz > LONG_MAX) return NULL;  /* unsupported block size */
    #endif
    if(*var && (*var < pMEMORY || *var > (byte *) pTOP)) return NULL;   /* (*var) is pointing outside the managed area */
    #if XMEM_ALIGN > 0
        sz = ((sz >> XMEM_ALIGN) + !!((unsigned long) sz % (1ul << XMEM_ALIGN))) << XMEM_ALIGN; /* round up to alignment boundary */
    #endif
    signed char attempts = 2;
    retry:;
    if(attempts < 0) return NULL;   /* the story ends here */
    xhdr_t *z = findxhdr(*var);     /* possible outcomes:
                                    [1] (!*var && !z) - a new block to be allocated
                                    [2] (!*var && z)  - impossible
                                    [3] (*var && !z)  - error (the variable is unaware that its block has been freed)
                                    [4] (*var && z)   - resize an existing data block */
    if(*var && z == NULL) return NULL;  /* case [3]; a data block is supplied but not recognised */
    xhdr_t *h, *t;
    if(z) { /* case [4]; the block can only be found if (*var) not NULL */
        if(sz == z->len) return z->data;    /* nothing is needed to do */
        if((z->data + z->len) == dcur) {    /* luckily it happens that (z) is the last allocated block */
            if((byte *) (z->data + sz) > (byte *) hcur) {
                x_defrag(); attempts--; goto retry;
            }
            if(sz > z->len) memset((z->data + z->len), 0, (size_t) (sz - z->len));  /* clear the expansion */
            z->len = sz;
            dcur = z->data + z->len;
            return z->data;
        }
    }
    byte *dptr = dcur;
    h = NULL;   /* this will be the header with which we work */
    t = pTOP;
    while(--t >= hcur) {    /* find the smallest free block able accommodate (sz) bytes */
        if(t->cvar == NULL && t->len >= sz && (h == NULL || t->len < h->len)) {
            h = t;
            if(h->len == sz) break; /* spot on */
        }
    }
    if(h) {     /* a suitable free block with length >= sz is found */
        if((sz + sizeof(long)) < h->len) {  /* try to carve off part from a large free block */
            if(attempts) {
                if((byte *) (hcur - 1) < dcur) {    /* no room for a new header */
                    x_defrag(); attempts--; goto retry;
                }
            }
            if(dcur <= (byte *) (hcur - 1)) {       /* create new header for the remainder */
                hcur--;
                hcur->cvar = NULL;
                hcur->data = h->data + sz;
                hcur->len = h->len - sz;
                h->len = sz;
            }
        }
        sz = h->len;
        dptr = h->data;
    }
    else {  /* a new block will have to be allocated */
        if((byte *) (hcur - 1) < (dcur + sz)) { /* not enough memory */
            x_defrag(); attempts--; goto retry;
        }
        dcur += sz;
        h = --hcur;
    }
    h->cvar = (void **) var;
    h->len = sz;
    h->data = dptr;
    *var = dptr;
    #if XBLK_NAME_LEN > 0
        h->name[0] = 0;
    #endif
    if(z) {     /* case [4] continuation; transfer the old block to the new place */
        if(h->len > z->len) {
            memset((h->data + z->len), 0, (size_t) (h->len - z->len));  /* clear the rest of the block */
            memcpy(h->data, z->data, (size_t) z->len);
        }
        else memcpy(h->data, z->data, (size_t) h->len);
        z->cvar = NULL; /* mark the old block as free */
    }
    else memset(h->data, 0, (size_t) h->len);
    return h->data;
}
