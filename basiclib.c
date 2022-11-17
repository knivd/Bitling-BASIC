#include "basic.h"
#include "basiclib.h"


// NOTE: the functions enclosed in this block are HARDWARE-DEPENDENT!
// ==================================================================================================

#define getDIN(P, b, into) { REGX(ANSEL, P, b) = 0; REGX(TRIS, P, b) = DIR_INPUT; into = _REG__(R, P, b); }
#define setDOUT(P, b, bit) { REGX(ANSEL, P, b) = 0; REGX(TRIS, P, b) = DIR_OUTPUT; REGX(LAT, P, b) = !!(bit); }

void cm_dout(void) {
    popAcc(&bas.B, TYPE_INT);   // bit
    popAcc(&bas.A, TYPE_INT);   // channel
    switch(bas.A.i) {
        case 0: setDOUT(B, 0, bas.B.i); break;
        case 1: setDOUT(B, 1, bas.B.i); break;
        case 2: setDOUT(B, 2, bas.B.i); break;
        case 3: setDOUT(B, 3, bas.B.i); break;
        case 4: setDOUT(B, 4, bas.B.i); break;
        case 5: setDOUT(B, 5, bas.B.i); break;
        case 6: setDOUT(B, 6, bas.B.i); break;
        case 7: setDOUT(B, 7, bas.B.i); break;
        default: break;
    }
}


void fn_din(void) {
    popAcc(&bas.A, TYPE_INT);   // channel
    switch(bas.A.i) {
        case 0: getDIN(B, 0, bas.A.i); break;
        case 1: getDIN(B, 1, bas.A.i); break;
        case 2: getDIN(B, 2, bas.A.i); break;
        case 3: getDIN(B, 3, bas.A.i); break;
        case 4: getDIN(B, 4, bas.A.i); break;
        case 5: getDIN(B, 5, bas.A.i); break;
        case 6: getDIN(B, 6, bas.A.i); break;
        case 7: getDIN(B, 7, bas.A.i); break;
        default: bas.A.i = -1; break;
    }
    pushAcc(&bas.A);

}


void fn_ain(void) {
    popAcc(&bas.A, TYPE_INT);   // channel
    switch(bas.A.i) {
        case 0: { REGX(ANSEL, B, 0) = 1; REGX(TRIS, B, 0) = DIR_INPUT; break; }
        case 1: { REGX(ANSEL, B, 1) = 1; REGX(TRIS, B, 1) = DIR_INPUT; break; }
        case 2: { REGX(ANSEL, B, 2) = 1; REGX(TRIS, B, 2) = DIR_INPUT; break; }
        case 3: { REGX(ANSEL, B, 3) = 1; REGX(TRIS, B, 3) = DIR_INPUT; break; }
        case 4: { REGX(ANSEL, B, 4) = 1; REGX(TRIS, B, 4) = DIR_INPUT; break; }
        case 5: { REGX(ANSEL, B, 5) = 1; REGX(TRIS, B, 5) = DIR_INPUT; break; }
        case 6: { REGX(ANSEL, B, 6) = 1; REGX(TRIS, B, 6) = DIR_INPUT; break; }
        case 7: { REGX(ANSEL, B, 7) = 1; REGX(TRIS, B, 7) = DIR_INPUT; break; }
        default: { bas.A.i = -1; goto end_ain; }
    }
    bas.A.i = adcRead((uint8_t) (8 + bas.A.i));
    end_ain:;
    pushAcc(&bas.A);
}

// ==================================================================================================


void __(void) {
    error(E_SYNTAX);
}


void op_unary_plus(void) {
    popAcc(&bas.A, TYPE_NONE);
    if(bas.A.type == TYPE_INT && bas.A.i < 0) bas.A.i = -(bas.A.i);
    else if(bas.A.type == TYPE_FPN && bas.A.f < 0.0) bas.A.f = -(bas.A.f);
    else if(bas.A.type == TYPE_STR) error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_unary_minus(void) {
    popAcc(&bas.A, TYPE_NONE);
    if(bas.A.type == TYPE_INT && bas.A.i > 0) bas.A.i = -(bas.A.i);
    else if(bas.A.type == TYPE_FPN && bas.A.f > 0.0) bas.A.f = -(bas.A.f);
    else if(bas.A.type == TYPE_STR) error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_add(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) {
        bas.A.i += bas.B.i;
        pushAcc(&bas.A);
    }
    else if(bas.A.type == TYPE_FPN) {
        bas.A.f += bas.B.f;
        pushAcc(&bas.A);
    }
    else if(bas.A.type == TYPE_STR) {
        size_t nla = strlen(bas.A.s);
        size_t nlb = strlen(bas.B.s);
        if(x_malloc((byte **) &bas.S.s, (size_x) (nla + nlb + 1)) == NULL) error(E_MEMORY);
        memcpy(bas.S.s, bas.A.s, nla);
        memcpy((bas.S.s + nla), bas.B.s, nlb);
        *(bas.S.s + nla + nlb) = 0; // ensure proper termination
        pushAcc(&bas.S);
        x_free((byte **) &bas.A.s);
        x_free((byte **) &bas.B.s);
    }
    else error(E_SYNTAX);
}


void op_sub(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.A.i -= bas.B.i;
    else if(bas.A.type == TYPE_FPN) bas.A.f -= bas.B.f;
    else error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_mul(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.A.i *= bas.B.i;
    else if(bas.A.type == TYPE_FPN) bas.A.f *= bas.B.f;
    else error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_div(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) {
        if(bas.B.i != 0) bas.A.i /= bas.B.i;
        else {
            #if ENABLE_NAN > 0
                bas.A.f = NAN;
                bas.A.type = TYPE_FPN;
            #else
                error(E_DIV_ZERO);
            #endif
        }
    }
    else if(bas.A.type == TYPE_FPN) {
        if(bas.B.f != 0.0) bas.A.f /= bas.B.f;
        else {
            #if ENABLE_NAN > 0
                bas.A.f = NAN;
                bas.A.type = TYPE_FPN;
            #else
                error(E_DIV_ZERO);
            #endif
        }
    }
    else error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_mod(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) {
        if(bas.B.i != 0) bas.A.i %= bas.B.i;
        else {
            #if ENABLE_NAN > 0
                bas.A.f = NAN;
                bas.A.type = TYPE_FPN;
            #else
                error(E_DIV_ZERO);
            #endif
        }
    }
    else if(bas.A.type == TYPE_FPN) {
        if(bas.B.f != 0.0) bas.A.f = fmod(bas.A.f, bas.B.f);
        else {
            #if ENABLE_NAN > 0
                bas.A.f = NAN;
                bas.A.type = TYPE_FPN;
            #else
                error(E_DIV_ZERO);
            #endif
        }
    }
    else error(E_SYNTAX);
    pushAcc(&bas.A);
}


void op_power(void) {
    popAcc(&bas.B, TYPE_FPN);
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = pow(bas.A.i, bas.B.i);
    pushAcc(&bas.A);
}


void op_shiftl(void) {
    popAcc(&bas.B, TYPE_INT);
    popAcc(&bas.A, TYPE_INT);
    bas.A.i <<= bas.B.i;
    pushAcc(&bas.A);
}


void op_shiftr(void) {
    popAcc(&bas.B, TYPE_INT);
    popAcc(&bas.A, TYPE_INT);
    bas.A.i >>= bas.B.i;
    pushAcc(&bas.A);
}


void op_equal(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !(bas.A.i != bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !(fabs(bas.A.f - bas.B.f) >= FPN_ACCURACY);
    else bas.Z.i = !strcmp(bas.A.s, bas.B.s);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_not_equal(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !(bas.A.i == bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !(fabs(bas.A.f - bas.B.f) < FPN_ACCURACY);
    else bas.Z.i = !!strcmp(bas.A.s, bas.B.s);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_sm_equal(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !!(bas.A.i <= bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !!(bas.A.f <= bas.B.f);
    else bas.Z.i = !!(strcmp(bas.A.s, bas.B.s) <= 0);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_smaller(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !!(bas.A.i < bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !!(bas.A.f < bas.B.f);
    else bas.Z.i = !!(strcmp(bas.A.s, bas.B.s) < 0);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_gr_equal(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !!(bas.A.i >= bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !!(bas.A.f >= bas.B.f);
    else bas.Z.i = !!(strcmp(bas.A.s, bas.B.s) >= 0);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_greater(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.Z.i = !!(bas.A.i > bas.B.i);
    else if(bas.A.type == TYPE_FPN) bas.Z.i = !!(bas.A.f > bas.B.f);
    else bas.Z.i = !!(strcmp(bas.A.s, bas.B.s) > 0);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
}


void op_and(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.A.i &= bas.B.i;
    else if(bas.A.type == TYPE_FPN) bas.A.i = !!(bas.A.f != 0.0 && bas.B.f != 0.0);
    else error(E_TYPE);
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void op_or(void) {
    popAcc(&bas.B, TYPE_NONE);
    popAcc(&bas.A, TYPE_NONE);
    promote();
    if(bas.A.type == TYPE_INT) bas.A.i |= bas.B.i;
    else if(bas.A.type == TYPE_FPN) bas.A.i = !!(bas.A.f != 0.0 || bas.B.f != 0.0);
    else error(E_TYPE);
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void op_xor(void) {
    popAcc(&bas.B, TYPE_INT);
    popAcc(&bas.A, TYPE_INT);
    bas.A.i ^= bas.B.i;
    pushAcc(&bas.A);
}


void op_not(void) {
    popAcc(&bas.A, TYPE_INT);
    bas.A.i = ~bas.A.i;
    pushAcc(&bas.A);
}


void cm_rewind(void) {
    bas.data_current = bas.data_entry;
}


void cm_pop(void) {
    if(bas.caX == 0 || bas.cas[bas.caX - 1].cursub == NULL) error(E_NO_SUB);
    bas.caX--;
}


void fn_rnd(void) {
    popAcc(&bas.A, TYPE_INT);
    if(bas.A.i) srand((unsigned int) bas.A.i);
    bas.A.f = (FPN_T) ((double) rand() / ((double) RAND_MAX + 1.0));
    bas.A.type = TYPE_FPN;
    pushAcc(&bas.A);
}


void fn_abs(void) {
    popAcc(&bas.A, TYPE_NONE);
    if(bas.A.type == TYPE_FPN) bas.A.f = fabs(bas.A.f);
    else if(bas.A.type == TYPE_INT) bas.A.i = labs(bas.A.i);
    else error(E_TYPE);
    pushAcc(&bas.A);
}


void fn_round(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = round(bas.A.f);
    pushAcc(&bas.A);
}


void fn_trunc(void) {
    popAcc(&bas.A, TYPE_INT);
    convert(&bas.A, TYPE_FPN);
    pushAcc(&bas.A);
}


void fn_fract(void) {
    popAcc(&bas.A, TYPE_FPN);
    memcpy(&bas.B, &bas.A, sizeof(data_t));
    convert(&bas.B, TYPE_INT);
    bas.A.f -= bas.B.i;
    pushAcc(&bas.A);
}


void fn_logd(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = log10(bas.A.f);
    pushAcc(&bas.A);
}


void fn_logn(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = log(bas.A.f);
    pushAcc(&bas.A);
}


void fn_exp(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = exp(bas.A.f);
    pushAcc(&bas.A);
}


void fn_sqr(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = sqrt(bas.A.f);
    pushAcc(&bas.A);
}


void fn_cbr(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = cbrt(bas.A.f);
    pushAcc(&bas.A);
}


void fn_sin(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = sin(bas.A.f);
    pushAcc(&bas.A);
}


void fn_asin(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = asin(bas.A.f);
    pushAcc(&bas.A);
}


void fn_hsin(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = sinh(bas.A.f);
    pushAcc(&bas.A);
}


void fn_cos(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = cos(bas.A.f);
    pushAcc(&bas.A);
}


void fn_acos(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = acos(bas.A.f);
    pushAcc(&bas.A);
}


void fn_hcos(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = cosh(bas.A.f);
    pushAcc(&bas.A);
}


void fn_tan(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = tan(bas.A.f);
    pushAcc(&bas.A);
}


void fn_atan(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = atan(bas.A.f);
    pushAcc(&bas.A);
}


void fn_htan(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = tanh(bas.A.f);
    pushAcc(&bas.A);
}


void fn_cotan(void) {
    popAcc(&bas.A, TYPE_FPN);
    bas.A.f = tan(bas.A.f);
    if(bas.A.f == 0.0) {
        #if ENABLE_NAN > 0
            bas.A.f = NAN;
            bas.A.type = TYPE_FPN;
        #else
            error(E_DIV_ZERO);
        #endif
    }
    bas.A.f = (FPN_T) (1.0 / bas.A.f);
    pushAcc(&bas.A);
}


void fn_free(void) {
    bas.A.i = x_avail();
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void fn_tdram(void) {
    bas.A.i = (INT_T) (uintptr_t) DMEM;
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void fn_tdwdt(void) {
    bas.A.i = LCD_WIDTH;
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void fn_tdhgt(void) {
    bas.A.i = LCD_HEIGHT;
    bas.A.type = TYPE_INT;
    pushAcc(&bas.A);
}


void fn_peek(void) {
    popAcc(&bas.A, TYPE_INT);
    if(bas.A.i >= 0) {          // access to RAM
        bas.A.i = (uint8_t) *((uint8_t *) bas.A.i);
    }
    else {                      // negative address is access to ROM-1 (i.e. address -1 == ROM address 0)
        bas.A.i = -bas.A.i - 1;
        bas.A.i = flash_readByte((bas.A.i / FLASH_WRITABLE_BYTES), (bas.A.i % FLASH_WRITABLE_BYTES));
    }
    pushAcc(&bas.A);
}


void cm_poke(void) {
    popAcc(&bas.B, TYPE_INT);   // value
    popAcc(&bas.A, TYPE_INT);   // address
    if(bas.A.i >= 0) {          // access to RAM
        *((uint8_t *) bas.A.i) = (uint8_t) bas.A.i;
    }
    else {
        // POKE into the ROM is NOT advisable!
    }
}


void fn_len(void) {
    popAcc(&bas.A, TYPE_STR);
    bas.Z.i = strlen(bas.A.s);
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
    x_free((byte **) &bas.A.s);
}


void fn_left(void) {
    popAcc(&bas.B, TYPE_INT);   // count
    popAcc(&bas.A, TYPE_STR);
    if(bas.B.i < 0) error(E_INDEX);
    if(bas.B.i > strlen(bas.A.s)) bas.B.i = strlen(bas.A.s);
    if(x_malloc((byte **) &bas.S.s, (size_x) (bas.B.i + 1)) == NULL) error(E_MEMORY);
    memcpy(bas.S.s, bas.A.s, (size_t) bas.B.i);
    *(bas.S.s + bas.B.i) = 0;
    pushAcc(&bas.S);
    x_free((byte **) &bas.A.s);
}


void fn_right(void) {
    popAcc(&bas.B, TYPE_INT);   // count
    popAcc(&bas.A, TYPE_STR);
    if(bas.B.i < 0) error(E_INDEX);
    if(bas.B.i > strlen(bas.A.s)) bas.B.i = strlen(bas.A.s);
    if(x_malloc((byte **) &bas.S.s, (size_x) (bas.B.i + 1)) == NULL) error(E_MEMORY);
    memcpy(bas.S.s, (bas.A.s + strlen(bas.A.s) - bas.B.i), (size_t) bas.B.i);
    *(bas.S.s + bas.B.i) = 0;
    pushAcc(&bas.S);
    x_free((byte **) &bas.A.s);
}


void fn_mid(void) {
    popAcc(&bas.B, TYPE_INT);   // count
    popAcc(&bas.Z, TYPE_INT);   // start position (from 1)
    popAcc(&bas.A, TYPE_STR);
    if(bas.B.i < 0 || bas.Z.i < 1) error(E_INDEX);
    bas.Z.i -= 1;   // start index from 1 (standard in BASIC)
    if(bas.Z.i > strlen(bas.A.s)) bas.Z.i = strlen(bas.Z.s);
    if((bas.Z.i + bas.B.i) > strlen(bas.A.s)) bas.B.i = strlen(bas.A.s) - bas.Z.i;
    if(x_malloc((byte **) &bas.S.s, (size_x) (bas.B.i + 1)) == NULL) error(E_MEMORY);
    memcpy(bas.S.s, (bas.A.s + bas.Z.i), (size_t) bas.B.i);
    *(bas.S.s + bas.B.i) = 0;
    pushAcc(&bas.S);
    x_free((byte **) &bas.A.s);
}


void fn_instr(void) {
    popAcc(&bas.Z, TYPE_INT);   // index from where to start searching (from 1)
    popAcc(&bas.B, TYPE_STR);   // string where search is performed
    popAcc(&bas.A, TYPE_STR);   // string to search for
    if(bas.Z.i < 0) error(E_INDEX);
    char *s = strstr(bas.B.s, bas.A.s);
    if(s != NULL) bas.Z.i = (INT_T) (intptr_t) bas.B.s - (INT_T) (intptr_t) s + 1;
    else bas.Z.i = 0;
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
    x_free((byte **) &bas.A.s);
    x_free((byte **) &bas.B.s);
}


void fn_asc(void) {
    popAcc(&bas.A, TYPE_STR);
    bas.Z.i = *bas.A.s;
    bas.Z.type = TYPE_INT;
    pushAcc(&bas.Z);
    x_free((byte **) &bas.A.s);
}


void fn_chr(void) {
    popAcc(&bas.A, TYPE_INT);
    if(x_malloc((byte **) &bas.S.s, (size_x) 2) == NULL) error(E_MEMORY);
    *bas.str_buf = (char) bas.A.i;
    *(bas.str_buf + 1) = 0;
    bas.S.s = bas.str_buf;
    pushAcc(&bas.S);
}


void fn_val(void) {
    popAcc(&bas.A, TYPE_STR);
    char *s = bas.A.s;
    if(getNumber(&s, &bas.Z) < 0) {     // getNumber() also sets the type
        #if ENABLE_NAN > 0
            bas.Z.f = NAN;
            bas.Z.type = TYPE_FPN;
        #else
            bas.Z.f = -0.0;
            bas.Z.type = TYPE_FPN;
        #endif
    }
    pushAcc(&bas.Z);
    x_free((byte **) &bas.A.s);
}


void fn_str(void) {
    popAcc(&bas.B, TYPE_INT);       // number of digits after the decimal point
    if(bas.B.i > 20) bas.B.i = 20;  // limit to 20 positions
    if(bas.B.i < 0) error(E_SYNTAX);
    popAcc(&bas.A, TYPE_NONE);
    if(bas.A.type == TYPE_STR) error(E_TYPE);
    memcpy(bas.str_buf, 0, sizeof(bas.str_buf));
    if(bas.A.type == TYPE_INT) {
        snprintf(bas.str_buf, sizeof(bas.str_buf) - 1, "%li", bas.A.i);
        if(bas.B.i && strlen(bas.str_buf) < (sizeof(bas.str_buf) - 2)) {
            sprintf(&bas.str_buf[strlen(bas.str_buf)], ".");
            while(bas.B.i-- && strlen(bas.str_buf) < (sizeof(bas.str_buf) - 1)) {
                sprintf(&bas.str_buf[strlen(bas.str_buf)], "0");
            }
        }
    }
    else {
        snprintf(bas.str_buf, sizeof(bas.str_buf) - 1, "%1.0G", bas.A.f);
        bas.A.f = bas.A.f - (INT_T) bas.A.f;    // get the fraction part
        memcpy(bas.str_buf2, 0, sizeof(bas.str_buf2));
        snprintf(bas.str_buf2, sizeof(bas.str_buf2) - 1, "%1.20G", bas.A.f);
        if(bas.B.i && strlen(bas.str_buf) < (sizeof(bas.str_buf) - 2)) {
            char *b = &bas.str_buf2[1]; // skip the leading 0 and start with the decimal point
            while(bas.B.i-- && strlen(bas.str_buf) < (sizeof(bas.str_buf) - 1)) {
                bas.str_buf[strlen(bas.str_buf)] = *(b++);
            }
            while(bas.B.i-- && strlen(bas.str_buf) < (sizeof(bas.str_buf) - 1)) {
                sprintf(&bas.str_buf[strlen(bas.str_buf)], "0");
            }
        }
    }
    if(x_malloc((byte **) &bas.S.s, (size_x) strlen(bas.str_buf) + 1) == NULL) error(E_MEMORY);
    strcpy(bas.S.s, bas.str_buf);
    pushAcc(&bas.S);
}


void cm_reset(void) {
    sys_flags.sleep ^= 1;   // tricking the system that it is currently asleep
    Reset();
}


void cm_sleep(void) {
    Reset();    // will enter sleep on the next reset
}


void cm_cls(void) {
    lcdCls();
}


void cm_hloc(void) {
    popAcc(&bas.A, TYPE_INT);
    lcdSetPos((uint8_t) bas.A.i - 1, posY);
}


void cm_vloc(void) {
    popAcc(&bas.A, TYPE_INT);
    lcdSetPos(posX, (uint8_t) bas.A.i - 1);
}


void cm_putch(void) {
    popAcc(&bas.A, TYPE_INT);
    lcdDrawChar((uint8_t) bas.A.i);
}


void cm_defch(void) {
    uint8_t d[8];
    uint8_t t = 8;
    while(t--) {
        popAcc(&bas.A, TYPE_INT);
        d[t] = (uint8_t) bas.A.i;
    }
    popAcc(&bas.A, TYPE_INT);
    lcdDefChar((uint8_t) bas.A.i, (uint8_t *) &d);
}


void cm_selcht(void) {
    popAcc(&bas.A, TYPE_INT);
    if(bas.A.i == 1) lcdSelectROM(ROM_A);
    else if(bas.A.i == 2) lcdSelectROM(ROM_B);
    else if(bas.A.i == 3) lcdSelectROM(ROM_C);
}


void cm_cursor(void) {
    popAcc(&bas.A, TYPE_INT);
    lcdCtrl((uint8_t) bas.A.i & (CURSOR | BLINK));
}


void cm_scroll(void) {
    popAcc(&bas.A, TYPE_INT);
    if((uint8_t) bas.A.i & BIT(0)) lcdScrollUp();
    if((uint8_t) bas.A.i & BIT(1)) lcdScrollDown();
    if((uint8_t) bas.A.i & BIT(2)) lcdScrollLeft();
    if((uint8_t) bas.A.i & BIT(3)) lcdScrollRight();
}


void cm_tdref(void) {
    lcdUpdateScreen();
}


void cm_wait(void) {
    popAcc(&bas.A, TYPE_INT);
    if(bas.A.i > 0) {
        while(bas.A.i--) mSec(1);
    }
}


void fn_timer(void) {
    popAcc(&bas.A, TYPE_INT);
    if(bas.A.i != 0) setTimer((uint32_t) bas.A.i);
    bas.A.i = (INT_T) getTimer();
    pushAcc(&bas.A);
}


void cm_let(void) {
    getToken();
    if(bas.t_token != T_IDENTIFIER) error(E_SYNTAX);
    revert_getToken();
}


void cm_gosub(void) {
    getToken();
    if(bas.t_token != T_SUBID) error(E_SYNTAX);
    revert_getToken();
}


void cm_else(void) {
    if(bas.if_depth == 0) error(E_NO_IF);
    if(*bas.tk_src < 0x03) {    // pre-calculated offset jump
        uint16_t offs = ((uint16_t) *(bas.tk_src + 2)) << 8;
        offs += (uint8_t) *(bas.tk_src + 3);
        bas.src = bas.tk_src + offs;
    }
    else {  // text form - not using pre-calculated offset jump
        getToken();
        uint8_t extra_depth = 0;
        bas.flags.no_str_consts = 1;
        while(bas.t_token != T_ENDIF || extra_depth) {  // skip to the following ENDIF
            if(bas.t_token == T_ETX) error(E_NO_ENDIF);
            if(bas.t_token == T_IF) {
                if(++extra_depth >= MAX_IFS) error(E_TOO_COMPLEX);
            }
            if(bas.t_token == T_ENDIF) {
                if(extra_depth-- == 0) error(E_NO_IF);
            }
            getToken();
        }
        bas.flags.no_str_consts = 0;
        bas.if_depth--;
    }
}


void cm_endif(void) {
    if(bas.if_depth == 0) error(E_NO_IF);
    bas.if_depth--;
}


void cm_wend(void) {
    if(bas.lpX == 0 || bas.loop[bas.lpX - 1].type != L_WHILE) error(E_NO_WHILE);
    bas.src = bas.loop[--bas.lpX].src;   // return back at the corresponding WHILE
}


void cm_repeat(void) {
    if(bas.lpX >= MAX_LOOPS) error(E_TOO_COMPLEX);
    bas.loop[bas.lpX].type = L_REPEAT;
    bas.loop[bas.lpX].src = bas.tk_src;
    bas.lpX++;
}


void cm_next(void) {
    if(bas.lpX == 0 || bas.loop[bas.lpX - 1].type != L_FOR) error(E_NO_FOR);
    bas.lpX--;
    uint8_t t;
    if(bas.loop[bas.lpX].v->data.type == TYPE_INT) t = (bas.loop[bas.lpX].v->data.i != bas.loop[bas.lpX].end_value.i);
    else t = (fabs(bas.loop[bas.lpX].v->data.f - bas.loop[bas.lpX].end_value.f) >= FPN_ACCURACY);
    getToken();
    if(bas.t_token == T_UNKNOWN) error(E_WRONG_ID);
    if(bas.t_token == T_IDENTIFIER) {   // an optional variable name is supplied
        if(bas.loop[bas.lpX].v != (var_t *) bas.s_token) error(E_WRONG_ID);
    }
    else revert_getToken(); // revert to the previous source position
    if(t) { // the end value is still different - add the step to the counter
        if(bas.loop[bas.lpX].v->data.type == TYPE_INT) {
            INT_T x = bas.loop[bas.lpX].v->data.i;
            bas.loop[bas.lpX].v->data.i = x + bas.loop[bas.lpX].step.i;
        }
        else {
            FPN_T x = bas.loop[bas.lpX].v->data.f;
            bas.loop[bas.lpX].v->data.f = x + bas.loop[bas.lpX].step.f;
        }
        bas.loop[bas.lpX].type = L_FOR;
        bas.src = bas.loop[bas.lpX].src; // return back at the corresponding FOR
        bas.lpX++;
    }
}


void cm_continue(void) {
    if(bas.lpX == 0) error(E_NO_LOOP);
    bas.src = bas.loop[bas.lpX - 1].src;
}


void cm_label(void) {
    getId();    // just skip the label name here
}


void cm_sub(void) {
    bas.flags.no_str_consts = 1;
    getToken();
    while(bas.t_token != T_ENDSUB) {    // skip to the following ENDSUB
        if(bas.t_token == T_ETX) error(E_NO_ENDSUB);
        if(bas.t_token == T_SUB) error(E_NESTING);
        if(bas.t_token == T_UNKNOWN) getId();
        getToken();
    }
    bas.flags.no_str_consts = 0;
}


void cm_goto(void) {
    getToken();
    if(bas.t_token != T_LABELID) error(E_WRONG_ID);
    bas.src = ((sub_t *) bas.s_token)->entry;
}


void cm_data(void) {
    bas.flags.no_str_consts = 1;
    while(bas.t_token != T_ETX) {
        getToken();
        if(bas.t_token == T_ETX || bas.t_token == T_EOL || bas.t_token == T_COLON) break;
        if(bas.t_token == T_UNKNOWN) getId();
    }
    bas.flags.no_str_consts = 0;
}


void cm_end(void) {
    error(E_OK);    // finish execution
}


void cm_clear(void) {
    getToken();
    if(bas.t_token == T_IDENTIFIER) {   // undefine specific variables
        do {
            char *parent = (bas.caX ? bas.cas[bas.caX - 1].cursub : NULL);
            var_t *vv = (var_t *) bas.s_token;      // this is the variable to be freed
            var_t *vn, *vp = NULL, *v = bas.vars;
            uint8_t success = 0;
            while(v) {
                vn = v->next;
                if((parent == v->parent || parent == NULL) && vv->nlen == v->nlen
                        && !istrncmp(vv->name, v->name, v->nlen)) {
                    if(v->ref == NULL) {    // referenced variables are only pointer to another variable
                        if(v->data.type == TYPE_STR) {
                            if(v->dim[0]) { // check if the variable is an array of strings
                                uint32_t t, total = 1;
                                for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++) total *= v->dim[t];
                                for(t = 0; t < total; t++) x_free((byte **) ((byte *) v->data.p + t * sizeof(PTR_T)));
                            }
                            else x_free((byte **) &v->data.s);      // single string
                        }
                        if(v->dim[0]) x_free((byte **) &v->data.p); // release arrays
                    }
                    x_free((byte **) &v);
                    if(vp) vp->next = vn;   // remove this variable from the chain
                    success = 1;
                    break;      // found and freed the specified variable, so finish now
                }
                else vp = v;    // store the previous not freed variable
                v = vn;
            }
            if(success == 0) error(E_UNKNOWN);  // not found
            getToken();
            if(bas.t_token == T_COMMA) {
                getToken();
                if(bas.t_token != T_IDENTIFIER) error(E_SYNTAX);
                continue;
            }
            if(bas.t_token != T_EOL && bas.t_token != T_COLON && bas.t_token != T_ETX) error(E_SYNTAX);
            revert_getToken();
        } while(0);
    }
    else releaseVars(NULL); // undefine all variables
}
