#ifndef COMMON_H
#define	COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <setjmp.h>

#include "platform/eadog204.h"  // LCD EA DOGM204
#include "platform/keybd.h"     // matrix keyboard
#include "platform/flash.h"     // flash functions
#include "platform/flashdrv.h"  // flash storage
#include "platform/hardware.h"  // generic hardware functions
#include "xmem.h"               // dynamic memory manager

// data types
#define INT_T   int32_t         // integer number type
#define FPN_T   float           // floating point number type
#define STR_T   char*           // string type
#define PTR_T   void*           // void pointer type (for arrays)

// enable/disable support of special numeric values
#define ENABLE_NAN  1
#define ENABLE_INF  0

#if ENABLE_NAN > 0
#ifndef NAN
    #define NAN ((FPN_T) 0.0 / 0.0)
#endif
#endif

#if ENABLE_INF > 0
#ifndef INF
    #define INF ((FPN_T) 1.0 / 0.0)
#endif
#endif

// generic definition of data types
typedef enum { TYPE_NONE = 0, TYPE_INT, TYPE_FPN, TYPE_STR } data_type_t;

// definition of a generic data container
typedef struct {
    union {
        INT_T i;        // integer number
        FPN_T f;        // floating point (real) number
        STR_T s;        // pointer to zero-terminated 8-bit character string
        PTR_T p;        // unspecified pointer to data
    };
    data_type_t type;   // data type in the container
} data_t;

#ifndef BIT
    #define BIT(n) (1ull << (n))
#endif

#define isdigitC(c) ((c) >= '0' && (c) <= '9')
#define isXdigitC(c) (isdigitC(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isidchr1C(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c) == '_' || (c) == '$')
#define isidchrC(c) (isidchr1C(c) || isdigitC(c))
#define skipSpaces(s) { while(s && *s == ' ') s++; }

// register naming macros
#define _REG(R, P) _REG__(R, P)             // example: _REG(R, BTN) where LED is defined as A,4 --> will compile as RA4
#define REG(R, P) REGX(R, P)                // example: REG(TRIS, LED) where LED is defined as A,4 --> will compile as TRISAbits.TRISA4
#define REGX(R, P, b) REGX__(R, P, b)       // example: REGX(TRIS, A, 4) --> will compile as TRISAbits.TRISA4
#define REG_PPS(P) REG_PPS__(P)             // example: REG_PPS(LED) where LED is defined as A,4 --> will compile as RA4PPS
#define REG_IOC(P, pol) REG_IOC__(P, pol)   // example: REG_IOC(LED, N) where LED is defined as A,4 --> will compile as IOCANbits.IOCAN4

#define COMMA ,
#define _REG__(R, P, b) R ## P ## b                         // INTERNAL - do not use!
#define REGX__(R, P, b) R ## P ## bits.R ## P ## b          // INTERNAL - do not use!
#define REG_PPS__(P, b) R ## P ## b ## PPS                  // INTERNAL - do not use!
#define REG_IOC__(P, b, pol) IOC ## P ## pol ## bits.IOC ## P ## pol ## b   // INTERNAL - do not use!

#define _ON     1
#define _OFF    0

int istrcmp(char *s1, char *s2);            // case-insensitive strcmp() function
int istrncmp(char *s1, char *s2, int n);    // case-insensitive strncmp() function
int getNumber(char **src, data_t *par);     // will return 0 if a valid number has been taken, or a negative number otherwise

// system flags preserved between reset events
__persistent struct {
    struct {
        uint8_t sleep   : 1;    // indicates need to enter sleep mode
        uint8_t scrlock : 1;    // (cleared on start) scroll lock flag
        uint8_t was_r   : 1;    // (cleared on start)used by the editor to mark incomplete block during transmission
        uint8_t         : 5;
    };
    uint32_t timer;             // free-running millisecond counter
} sys_flags;

#endif
