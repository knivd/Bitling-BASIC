#include "common.h"


// case-insensitive strcmp() function
int istrcmp(char *s1, char *s2) {
    //if(s1 == NULL) return (int) (s2 ? *s2 : 0);
    //if(s2 == NULL) return (int) (s1 ? *s1 : 0);
    for( ; toupper(*s1) == toupper(*s2) && *s1; s1++ ,s2++);
    return (toupper(*s1) - toupper(*s2));
}


// case-insensitive strncmp() function
int istrncmp(char *s1, char *s2, int n) {
    //if(s1 == NULL) return (int) (s2 ? *s2 : 0);
    //if(s2 == NULL) return (int) (s1 ? *s1 : 0);
    //if(n == 0) return 0;
    for(--n ; toupper(*s1) == toupper(*s2) && *s1 && n; s1++ ,s2++, n--);
    return (toupper(*s1) - toupper(*s2));
}


// get (an absolute value) number from input text
// DEC (also 0dDEC), $HEX (also 0xHEX), %BIN (also 0bBIN) formats are allowed
// will return 0 if a valid number has been taken
// on exit (**src) will point the first character after the number
#define GET_ABSOLUTE_NUMBER 1   // set 0 to enable getting signed values
int getNumber(char **src, data_t *par) {
    char *prog = *src;
    while(*prog == ' ') prog++; // skip leading spaces
    *src = prog;    // update (*src) here in case there is an abrupt exit later
    if(isdigitC(*prog)
            #if GET_ABSOLUTE_NUMBER == 0
                || *prog == '-' || *prog == '+'
            #endif
            || *prog == '.' || *prog =='$' || *prog == '%') {   /* numeric constants */
		uint8_t t;
		INT_T i = 0;

        /* release memory held by old strings in the destination structure */
        /* if(par->type == TYPE_STR) x_free((byte **) &par->s); */

        /* hexadecimal unsigned constants staring with '0x' */
        if((*prog == '0' && (*(prog + 1) == 'x' || *(prog + 1) == 'X')) || *prog == '$') {
            t = 16;     /* maximum number of hexadecimal digits (depends on the size of integers) */
            if(*prog == '$') prog++; else prog += 2;    /* '$' prefix or '0x' prefix */
            while(t-- && isXdigitC(*prog)) {
                uint8_t c = *(prog++);
                if(c >= '0' && c <= '9') c = (c - '0');
                else if(c >= 'a' && c <= 'f') c = (c - 'a' + 10);
                else if(c >= 'A' && c <= 'F') c = (c - 'A' + 10);
                else { prog--; break; }
                i = (i << 4) + c;
            }
            if(t == 0) return -1;
            par->i = (INT_T) i;
            par->type = TYPE_INT;   /* hexadecimal numbers are always considered integer */
        }

        /* binary unsigned constants staring with '0b' */
        else if((*prog == '0' && (*(prog + 1) == 'b' || *(prog + 1) == 'B')) || *prog == '%') {
            t = 64; /* maximum number of binary digits (depends on the size of integers) */
            if(*prog == '%') prog++; else prog += 2;    /* '%' prefix or '0x' prefix */
            while(t-- && (*prog == '0' || *prog == '1')) i = (i << 1) + (*(prog++) - '0');
            if(t == 0) return -1;
            par->i = (INT_T) i;
            par->type = TYPE_INT;   /* binary numbers are always considered integer */
        }

        /* Not-A-Number parameter */
        #if ENABLE_NAN > 0
        else if(!istrncmp(prog, ".NAN", 4) && *(prog + 4) <= ' ') {
            prog += 4;
            par->f = NAN;
            par->type = TYPE_FPN;   /* floating point type */
        }
        #endif

        /* negative infinity */
        #if ENABLE_INF > 0
        else if(!istrncmp(prog, "-INF", 4) && *(prog + 4) <= ' ') {
            prog += 4;
            par->f = (FPN_T) -INF;
            par->type = TYPE_FPN;   /* floating point type */
        }
        #endif

        /* positive infinity */
        #if ENABLE_INF > 0
        else if(!istrncmp(prog, "+INF", 4) && *(prog + 4) <= ' ') {
            prog += 4;
            par->f = (FPN_T) INF;
            par->type = TYPE_FPN;   /* floating point type */
        }
        #endif

        /*
		decimal constants (optionally starting with '0d')
		supported format: [sign] nnn [.nnn [ e/E [sign] nnn [.nnn] ] ]
        */
        else if(isdigitC(*prog)
                #if GET_ABSOLUTE_NUMBER == 0
                    || *prog == '-' || *prog == '+'
                #endif
                || *prog == '.') {
            if(*prog == '0' && (*(prog + 1) == 'd' || *(prog + 1) == 'D')) prog += 2;   /* just skip the '0d' part */
            FPN_T v = 0.0;      /* fully constructed floating point value */
            FPN_T f = 0.0;      /* factor */
            FPN_T e = 0.0;      /* exponent */
            int8_t dp = 0;      /* 1:'decimal point found' flag */
            int8_t ef = 0;      /* 1:'E found' flag */
            int8_t es = 1;      /* 'E' sign 1:positive, -1:negative */
            int8_t vs = 1;      /* main value sign 1:positive, -1:negative */
            t = 60;             /* maximum number of characters in the number */
            #if GET_ABSOLUTE_NUMBER == 0
                if(*prog == '-') { vs = -1; prog++; }
                if(*prog == '+') { vs = 1; prog++; }
            #endif
            while(t-- && *prog) {
                if(isdigitC(*prog)) {
                    if(!ef) {
                        if(!dp) i = (10 * i) + (*prog - '0');
                        else { v += (f * (*prog - '0')); f *= 0.1; }
                    }
                    else {
                        if(!dp) e = (10 * e) + (*prog - '0');
                        else { e += (f * (*prog - '0')); f *= 0.1; }
                    }
                    prog++;
                }
                else {
                    if(*prog == '.') {
                        if(!dp) {
                            if(!ef) v = (FPN_T) i;
                            dp = 1; f = 0.1;
                            prog++;
                        }
                        else return -1;
                    }
                    else if(*prog == 'e' || *prog == 'E') {
                        if(!ef) {
                            if(!dp) v = (FPN_T) i;
                            dp = 0; ef = 1;
                            prog++;
                            if(*prog == '-' || *prog == '+') {
                                if(*(prog++) == '-') { es = -1; f = 0.1; }
                            }
                        }
                        else return -1;
                    }
                    else break;
                }
            }
            if(t == 0) return -1;
            if(dp || ef) {  /* floating point numbers */
                v *= pow(10, (e * es));
                par->f = vs * v;
                par->type = TYPE_FPN;   /* floating point type */
            }
            else {          /* integer types */
                if(e != 0.0) i *= (INT_T) pow(10, (e * es));
                par->i = i * vs;
                par->type = TYPE_INT;   /* integer type */
            }
        }
        else return -1;
    }
    else return -1;
    *src = prog;
    return 0;
}
