// functions to maintain the keyboard
// the keyboard is organised as 12x4 array, in which the columns are output and rows are input
// columns are counted from left to right, and rows are counted from top to bottom
// counting of columns and rows starts from 1
// three keys are independent
//      Alt   (equivalent to COL=1, ROW=2)
//      Shift (equivalent to COL=1, ROW=3)
//      Ctrl  (equivalent to COL=12, ROW=1)
#ifndef KEYBD_H
#define	KEYBD_H

#include "../common.h"

// these are the codes set in the keyboard decoding table
#define CODE_LEFT       0x11
#define CODE_RIGHT      0x12
#define CODE_UP         0x13
#define CODE_DOWN       0x14
#define CODE_HOME       0x02
#define CODE_END        0x03
#define CODE_PGUP       0x0B
#define CODE_PGDN       0x0C
#define CODE_ESC        0x1B

// keyboard matrix size
#define KBD_COLUMNS     12
#define KBD_ROWS        4

// keyboard lines
#define KBD_COL1        A,4
#define KBD_COL2        A,5
#define KBD_COL3        E,0
#define KBD_COL4        E,1
#define KBD_COL5        E,2
#define KBD_COL6        A,7
#define KBD_COL7        A,6
#define KBD_COL8        C,6
#define KBD_COL9        C,5
#define KBD_COL10       C,7
#define KBD_COL11       D,4
#define KBD_COL12       D,5

#define KBD_ROW1        A,3
#define KBD_ROW2        A,2
#define KBD_ROW3        A,1
#define KBD_ROW4        A,0

#define KBD_ALT         D,6
#define KBD_SHIFT       D,7
#define KBD_CTRL        B,5

// ADC channel numbers
// A:00..07, B:08..0F, C:10..17, D:18..1F, E:20..27
#define KBD_COL1_AN     0x04
#define KBD_COL2_AN     0x05
#define KBD_COL3_AN     0x20
#define KBD_COL4_AN     0x21
#define KBD_COL5_AN     0x22
#define KBD_COL6_AN     0x07
#define KBD_COL7_AN     0x06
#define KBD_COL8_AN     0x16
#define KBD_COL9_AN     0x15
#define KBD_COL10_AN    0x17
#define KBD_COL11_AN    0x1C
#define KBD_COL12_AN    0x1D

#define KBD_ROW1_AN     0x03
#define KBD_ROW2_AN     0x02
#define KBD_ROW3_AN     0x01
#define KBD_ROW4_AN     0x00

#define KBD_ALT_AN      0x1E
#define KBD_SHIFT_AN    0x1F
#define KBD_CTRL_AN     0x0D

// bit flags for kbdGetScanCode()
#define KBD_FLAG_SHIFT  BIT(0)
#define KBD_FLAG_ALT    BIT(1)
#define KBD_FLAG_CTRL   BIT(2)

#define KB_FLAGS_CLRMS  2000    // timeout after which sticky keys (Shift / Alt / Ctrl) will be released

uint32_t kb_f_clrcnt;   // counter to clear special flags after a period
uint8_t kbd_flags;      // bitmap with keyboard flags (see KBD_FLAG_xxx constants)

void kbdCheckFlags(uint8_t *kbflags, uint8_t KBD_CAPACITIVE);   // mainly internal but can be called from outside as well
uint8_t kbdGetScanCode(uint8_t *kbflags, uint8_t full); // parameter can be NULL if the flags are not needed
uint8_t kbdGet(void);   // doesn't wait

#define lcdCursorOn()   lcdCtrl(DISPLAY | CURSOR | BLINK)
#define lcdCursorOff()  lcdCtrl(DISPLAY)

// internal constants
#define KBD_ADC_TRIM    1       // number of LSB bits to trim from the ADC reading
#define KEY_THRESHOLD_C 2027    // capacitive ADC threshold value change to consider key pressed (between 1 and 255)
#define KEY_THRESHOLD_R 1536    // resistive ADC threshold value change to consider key pressed (between 1 and 255)

// standard library functions
char *fgets(char *str, int num, FILE *stream);

#endif
