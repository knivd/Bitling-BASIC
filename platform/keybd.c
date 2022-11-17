#include "keybd.h"

// keyboard codes
const uint8_t kbd_map[4][KBD_ROWS][KBD_COLUMNS] = {

{ // normal state (no Shift, no Alt)
{ '\x1B',   '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '\b'    },
{ '\t',     'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',    'o',    'p',    0       },
{ 0,        'a',    's',    'd',    'f',    'g',    'h',    'j',    'k',    'l',    '\x13', '\r'    },
{ 0,        'z',    'x',    'c',    'v',    'b',    'n',    'm',    ' ',    '\x11', '\x14', '\x12'  },
},

{ // Shift state
{ '`',      '!',    '@',    '#',    '$',    '%',    '^',    '&',    '*',    '(',    ')',    '\b'    },
{ '\t',     'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',    'O',    'P',    0       },
{ 0,        'A',    'S',    'D',    'F',    'G',    'H',    'J',    'K',    'L',    '\x0B', '\r'    },
{ 0,        'Z',    'X',    'C',    'V',    'B',    'N',    'M',    '/',    '\x02', '\x0C', '\x03'  },
},

{ // Alt state
{ '~',      0,      0,      0,      0,      0,      0,      0,      0,      '-',    '=',    '\x7F'  },
{ 0,        0,      0,      0,      0,      0,      0,      0,      '[',    ']',    '\\',   0       },
{ 0,        0,      0,      0,      0,      0,      0,      0,      ';',    '\'',   0,      0       },
{ 0,        0,      0,      0,      0,      0,      ',',    '.',    '?',    0,      0,      0       },
},

{ // Shift+Alt state
{ 0,        0,      0,      0,      0,      0,      0,      0,      0,      '_',    '+',    '\x7F'  },
{ 0,        0,      0,      0,      0,      0,      0,      0,      '{',    '}',    '|',    0       },
{ 0,        0,      0,      0,      0,      0,      0,      0,      ':',    '\"',   0,      0       },
{ 0,        0,      0,      0,      0,      0,      '<',    '>',    ' ',    0,      0,      0       },
}

};


// initialise the keyboard
static void kbdInit(uint8_t KBD_CAPACITIVE) {

    // initialise the port lines
    if(KBD_CAPACITIVE) {
        REG(TRIS, KBD_COL1) = DIR_INPUT; REG(ANSEL, KBD_COL1) = 1; //REG(WPU, KBD_COL1) = 1;
        REG(TRIS, KBD_COL2) = DIR_INPUT; REG(ANSEL, KBD_COL2) = 1; //REG(WPU, KBD_COL2) = 1;
        REG(TRIS, KBD_COL3) = DIR_INPUT; REG(ANSEL, KBD_COL3) = 1; //REG(WPU, KBD_COL3) = 1;
        REG(TRIS, KBD_COL4) = DIR_INPUT; REG(ANSEL, KBD_COL4) = 1; //REG(WPU, KBD_COL4) = 1;
        REG(TRIS, KBD_COL5) = DIR_INPUT; REG(ANSEL, KBD_COL5) = 1; //REG(WPU, KBD_COL5) = 1;
        REG(TRIS, KBD_COL6) = DIR_INPUT; REG(ANSEL, KBD_COL6) = 1; //REG(WPU, KBD_COL6) = 1;
        REG(TRIS, KBD_COL7) = DIR_INPUT; REG(ANSEL, KBD_COL7) = 1; //REG(WPU, KBD_COL7) = 1;
        REG(TRIS, KBD_COL8) = DIR_INPUT; REG(ANSEL, KBD_COL8) = 1; //REG(WPU, KBD_COL8) = 1;
        REG(TRIS, KBD_COL9) = DIR_INPUT; REG(ANSEL, KBD_COL9) = 1; //REG(WPU, KBD_COL9) = 1;
        REG(TRIS, KBD_COL10) = DIR_INPUT; REG(ANSEL, KBD_COL10) = 1; //REG(WPU, KBD_COL10) = 1;
        REG(TRIS, KBD_COL11) = DIR_INPUT; REG(ANSEL, KBD_COL11) = 1; //REG(WPU, KBD_COL11) = 1;
        REG(TRIS, KBD_COL12) = DIR_INPUT; REG(ANSEL, KBD_COL12) = 1; //REG(WPU, KBD_COL12) = 1;
    }
    else {
        REG(TRIS, KBD_COL1) = DIR_OUTPUT; REG(LAT, KBD_COL1) = 1;
        REG(TRIS, KBD_COL2) = DIR_OUTPUT; REG(LAT, KBD_COL2) = 1;
        REG(TRIS, KBD_COL3) = DIR_OUTPUT; REG(LAT, KBD_COL3) = 1;
        REG(TRIS, KBD_COL4) = DIR_OUTPUT; REG(LAT, KBD_COL4) = 1;
        REG(TRIS, KBD_COL5) = DIR_OUTPUT; REG(LAT, KBD_COL5) = 1;
        REG(TRIS, KBD_COL6) = DIR_OUTPUT; REG(LAT, KBD_COL6) = 1;
        REG(TRIS, KBD_COL7) = DIR_OUTPUT; REG(LAT, KBD_COL7) = 1;
        REG(TRIS, KBD_COL8) = DIR_OUTPUT; REG(LAT, KBD_COL8) = 1;
        REG(TRIS, KBD_COL9) = DIR_OUTPUT; REG(LAT, KBD_COL9) = 1;
        REG(TRIS, KBD_COL10) = DIR_OUTPUT; REG(LAT, KBD_COL10) = 1;
        REG(TRIS, KBD_COL11) = DIR_OUTPUT; REG(LAT, KBD_COL11) = 1;
        REG(TRIS, KBD_COL12) = DIR_OUTPUT; REG(LAT, KBD_COL12) = 1;
        REG(WPU, KBD_ROW1) = 1;
        REG(WPU, KBD_ROW2) = 1;
        REG(WPU, KBD_ROW3) = 1;
        REG(WPU, KBD_ROW4) = 1;
    }

    REG(TRIS, KBD_ROW1) = DIR_INPUT; REG(ANSEL, KBD_ROW1) = 1; //REG(WPU, KBD_ROW1) = 1;
    REG(TRIS, KBD_ROW2) = DIR_INPUT; REG(ANSEL, KBD_ROW2) = 1; //REG(WPU, KBD_ROW2) = 1;
    REG(TRIS, KBD_ROW3) = DIR_INPUT; REG(ANSEL, KBD_ROW3) = 1; //REG(WPU, KBD_ROW3) = 1;
    REG(TRIS, KBD_ROW4) = DIR_INPUT; REG(ANSEL, KBD_ROW4) = 1; //REG(WPU, KBD_ROW4) = 1;

    REG(TRIS, KBD_SHIFT) = DIR_INPUT; REG(ANSEL, KBD_SHIFT) = 1; REG(WPU, KBD_SHIFT) = 1;
    REG(TRIS, KBD_ALT) = DIR_INPUT; REG(ANSEL, KBD_ALT) = 1; REG(WPU, KBD_ALT) = 1;
    REG(TRIS, KBD_CTRL) = DIR_INPUT; REG(ANSEL, KBD_CTRL) = 1; REG(WPU, KBD_CTRL) = 1;

    // initialise some ADC parameters
    ADCON2bits.MD = 0b010;  // average mode
    ADCON0bits.CS = 1;      // ADC clock
    ADCON0bits.FM = 1;      // right justified result
    ADCON0bits.ON = 1;      // enable the ADC
    ADCON1bits.DSEN = 1;    // double sampling enabled
}


// this function is now in hardware.c
#if 0
// switch to AN channel and get a reading from the ADC
static uint16_t adcRead(uint8_t ch) {
    ADCON2bits.ACLR = 1;
    ADPCH = ch;
    ADCON0bits.GO = 1;
    uint8_t t = 25;
    while(ADCON0bits.GO && --t) uSec(1);
    return (t ? (uint16_t) ADRES : (uint16_t) -1);
}
#endif


// INTERNAL
// read the main keyboard matrix
static uint8_t kbdGetScanCode_(uint8_t KBD_CAPACITIVE) {
    uint8_t row, col, code = 0;
    uint16_t ar = (uint16_t) -1;
    for(row = KBD_ROWS; code == 0 && row; row--) {

        if(KBD_CAPACITIVE) {
            switch(row) {
                default: ar = (uint16_t) -1; break;
                case 1:
                    REG(WPU, KBD_ROW1) = 0;
                    ar = adcRead(KBD_ROW1_AN) >> KBD_ADC_TRIM;
                    REG(WPU, KBD_ROW1) = 1;
                    break;
                case 2:
                    REG(WPU, KBD_ROW2) = 0;
                    ar = adcRead(KBD_ROW2_AN) >> KBD_ADC_TRIM;
                    REG(WPU, KBD_ROW2) = 1;
                    break;
                case 3:
                    REG(WPU, KBD_ROW3) = 0;
                    ar = adcRead(KBD_ROW3_AN) >> KBD_ADC_TRIM;
                    REG(WPU, KBD_ROW3) = 1;
                    break;
                case 4:
                    REG(WPU, KBD_ROW4) = 0;
                    ar = adcRead(KBD_ROW4_AN) >> KBD_ADC_TRIM;
                    REG(WPU, KBD_ROW4) = 1;
                    break;
            }
            uint16_t ac = (uint16_t) -1;
            for(col = KBD_COLUMNS; code == 0 && col; col--) {
                switch(col) {
                    default: ac = (uint16_t) -1; break;
                    case 1:
                        REG(WPU, KBD_COL1) = 0;
                        ac = adcRead(KBD_COL1_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL1) = 1;
                        break;
                    case 2:
                        REG(WPU, KBD_COL2) = 0;
                        ac = adcRead(KBD_COL2_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL2) = 1;
                        break;
                    case 3:
                        REG(WPU, KBD_COL3) = 0;
                        ac = adcRead(KBD_COL3_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL3) = 1;
                        break;
                    case 4:
                        REG(WPU, KBD_COL4) = 0;
                        ac = adcRead(KBD_COL4_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL4) = 1;
                        break;
                    case 5:
                        REG(WPU, KBD_COL5) = 0;
                        ac = adcRead(KBD_COL5_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL5) = 1;
                        break;
                    case 6:
                        REG(WPU, KBD_COL6) = 0;
                        ac = adcRead(KBD_COL6_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL6) = 1;
                        break;
                    case 7:
                        REG(WPU, KBD_COL7) = 0;
                        ac = adcRead(KBD_COL7_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL7) = 1;
                        break;
                    case 8:
                        REG(WPU, KBD_COL8) = 0;
                        ac = adcRead(KBD_COL8_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL8) = 1;
                        break;
                    case 9:
                        REG(WPU, KBD_COL9) = 0;
                        ac = adcRead(KBD_COL9_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL9) = 1;
                        break;
                    case 10:
                        REG(WPU, KBD_COL10) = 0;
                        ac = adcRead(KBD_COL10_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL10) = 1;
                        break;
                    case 11:
                        REG(WPU, KBD_COL11) = 0;
                        ac = adcRead(KBD_COL11_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL11) = 1;
                        break;
                    case 12:
                        REG(WPU, KBD_COL12) = 0;
                        ac = adcRead(KBD_COL12_AN) >> KBD_ADC_TRIM;
                        REG(WPU, KBD_COL12) = 1;
                        break;
                }
                if(ar < KEY_THRESHOLD_C && ac < KEY_THRESHOLD_C) code = (uint8_t) (row << 5) + col;
            }
        }

        else {  // "standard" resistive matrix model
            for(col = KBD_COLUMNS; code == 0 && col; col--) {
                switch(col) {
                    default: break;
                    case 1: REG(LAT, KBD_COL1) = 0; break;
                    case 2: REG(LAT, KBD_COL2) = 0; break;
                    case 3: REG(LAT, KBD_COL3) = 0; break;
                    case 4: REG(LAT, KBD_COL4) = 0; break;
                    case 5: REG(LAT, KBD_COL5) = 0; break;
                    case 6: REG(LAT, KBD_COL6) = 0; break;
                    case 7: REG(LAT, KBD_COL7) = 0; break;
                    case 8: REG(LAT, KBD_COL8) = 0; break;
                    case 9: REG(LAT, KBD_COL9) = 0; break;
                    case 10: REG(LAT, KBD_COL10) = 0; break;
                    case 11: REG(LAT, KBD_COL11) = 0; break;
                    case 12: REG(LAT, KBD_COL12) = 0; break;
                }
                switch(row) {
                    default: ar = (uint16_t) -1; break;
                    case 1: ar = adcRead(KBD_ROW1_AN) >> KBD_ADC_TRIM; break;
                    case 2: ar = adcRead(KBD_ROW2_AN) >> KBD_ADC_TRIM; break;
                    case 3: ar = adcRead(KBD_ROW3_AN) >> KBD_ADC_TRIM; break;
                    case 4: ar = adcRead(KBD_ROW4_AN) >> KBD_ADC_TRIM; break;
                }
                switch(col) {
                    default: break;
                    case 1: REG(LAT, KBD_COL1) = 1; break;
                    case 2: REG(LAT, KBD_COL2) = 1; break;
                    case 3: REG(LAT, KBD_COL3) = 1; break;
                    case 4: REG(LAT, KBD_COL4) = 1; break;
                    case 5: REG(LAT, KBD_COL5) = 1; break;
                    case 6: REG(LAT, KBD_COL6) = 1; break;
                    case 7: REG(LAT, KBD_COL7) = 1; break;
                    case 8: REG(LAT, KBD_COL8) = 1; break;
                    case 9: REG(LAT, KBD_COL9) = 1; break;
                    case 10: REG(LAT, KBD_COL10) = 1; break;
                    case 11: REG(LAT, KBD_COL11) = 1; break;
                    case 12: REG(LAT, KBD_COL12) = 1; break;
                }
                if(ar < KEY_THRESHOLD_R) code = (uint8_t) (row << 5) + col;
            }
        }

    }
    return code;
}


// check the special key modifiers
// flags must be cleared externally before calling this function
void kbdCheckFlags(uint8_t *kbflags, uint8_t KBD_CAPACITIVE) {
    if(kbflags == NULL) return;
    uint16_t ar;
    uint8_t t;

    // ALT key
    for(ar = t = 0; t < 5; t++) {
        if(KBD_CAPACITIVE) { REG(WPU, KBD_ALT) = 0; uSec(1); }
        ar += (adcRead(KBD_ALT_AN) >> KBD_ADC_TRIM);
        REG(WPU, KBD_ALT) = 1;
    }
    ar /= t;
    if(KBD_CAPACITIVE) {
        if(ar < KEY_THRESHOLD_C) { *kbflags |= KBD_FLAG_ALT; kb_f_clrcnt = sys_flags.timer; }
    }
    else {
        if(ar < KEY_THRESHOLD_R) { *kbflags |= KBD_FLAG_ALT; kb_f_clrcnt = sys_flags.timer; }
    }

    // CTRL key
    for(ar = t = 0; t < 5; t++) {
        if(KBD_CAPACITIVE) { REG(WPU, KBD_CTRL) = 0; uSec(1); }
        ar += (adcRead(KBD_CTRL_AN) >> KBD_ADC_TRIM);
        REG(WPU, KBD_CTRL) = 1;
    }
    ar /= t;
    if(KBD_CAPACITIVE) {
        if(ar < KEY_THRESHOLD_C) { *kbflags |= KBD_FLAG_CTRL; kb_f_clrcnt = sys_flags.timer; }
    }
    else {
        if(ar < KEY_THRESHOLD_R) { *kbflags |= KBD_FLAG_CTRL; kb_f_clrcnt = sys_flags.timer; }
    }

    // SHIFT key
    for(ar = t = 0; t < 5; t++) {
        if(KBD_CAPACITIVE) { REG(WPU, KBD_SHIFT) = 0; uSec(1); }
        ar += (adcRead(KBD_SHIFT_AN) >> KBD_ADC_TRIM);
        REG(WPU, KBD_SHIFT) = 1;
    }
    ar /= t;
    if(KBD_CAPACITIVE) {
        if(ar < KEY_THRESHOLD_C) { *kbflags |= KBD_FLAG_SHIFT; kb_f_clrcnt = sys_flags.timer; }
    }
    else {
        if(ar < KEY_THRESHOLD_R) { *kbflags |= KBD_FLAG_SHIFT; kb_f_clrcnt = sys_flags.timer; }
    }

}


// scancode is returned as ((ROW << 5) + COL)
// where both the ROW and COL values start from 1
// returns 0 if there is no key pressed
uint8_t kbdGetScanCode(uint8_t *kbflags, uint8_t full) {
    uint8_t t, ch = 0;
    for(t = 0; t <= 1 && ch == 0; t++) {
        kbdInit((uint8_t) t);
        if(kbflags) kbdCheckFlags(kbflags, (uint8_t) t);    // check the special modifiers
        if(full) ch = kbdGetScanCode_((uint8_t) t);         // first try resistive and then capacitive method
    }
    return ch;
}


// get a translated code from the keyboard
// doesn't wait, and returns 0 if no key is pressed
uint8_t kbdGet(void) {
    static uint8_t kbflags = 0;
    uint8_t z = kbdGetScanCode(&kbflags, 1);
    kbd_flags = kbflags;    // make the last keyboard flags public
    if((sys_flags.timer - kb_f_clrcnt) > KB_FLAGS_CLRMS) { kbflags = 0; kb_f_clrcnt = sys_flags.timer; }
    if(z == 0) return 0;    // no pressed key
    uint8_t r = (z >> 5) - 1;
    uint8_t c = (z & 0b11111) - 1;
    z = (uint8_t) (!!(kbflags & KBD_FLAG_ALT) << 1) + !!(kbflags & KBD_FLAG_SHIFT);
    kbflags = 0; kb_f_clrcnt = sys_flags.timer;     // clear the special flags
    return kbd_map[z][r][c];
}


// getch() hook
__attribute__ ((used))
int getchar(void) {
    int r, ch;
    for(r = 0; r < 6; r++) {    // make sure all keys are released (this also determines the autorepeat rate)
        mSec(5);
        if(kbdGetScanCode(NULL, 1) > 0) r = 0;
    }
    for(r = 0; r < 2; r++) {    // get a confirmed reading
        mSec(5);
        if((ch = kbdGet()) == 0) r = 0;
    }
    return ch;
}


// gets() hook
__attribute__ ((used))
char *fgets(char *str, int num, FILE *stream) {
    if(str == NULL || num <= 0) return NULL;
    int ch, len = 0;
    while(len < num) {
        ch = getchar();
        if(ch == '\r' || ch == '\n') { str[len++] = '\n'; printf("\r\n"); break; }  // end of string
        else if(ch == '\b' && len) { len--; printf("%c", ch); }     // backspace
        else if(ch == '\t') {       // tab key (two spaces)
            for(ch = 2; ch > 0 && len < num; ch--) { str[len++] = ' '; printf(" "); }
        }
        else if(ch == '\x7F') {}    // delete key (TODO)
        else if(ch == '\x11') {}    // left arrow (TODO)
        else if(ch == '\x12') {}    // right arrow (TODO)
        else if(ch >= ' ') { str[len++] = (char) ch; printf("%c", ch); }    // printable codes
    }
    str[len] = 0;
    return str;
}
