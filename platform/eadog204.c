#include "eadog204.h"

#define LCD_WAIT_US 20
#define E_PULSE()   { LCD_E = 1; uSec(LCD_WAIT_US / 2); LCD_E = 0; }


// reset initialise the LCD
void lcdInit(void) {
    LCD_PORT_D &= ~(0b1111 << LCD_OFFSET);
    LCD_PORT_D |= ((0b1111 * DIR_OUTPUT) << LCD_OFFSET);    // set the data port as output
    LCD_ON_D = LCD_E_D = LCD_RWn_D = LCD_RS_D = LCD_RSTn_D = DIR_OUTPUT;

    LCD_RSTn = 1;
    LCD_E = LCD_RWn = LCD_RS = 0;
    mSec(20);
    LCD_ON = 1;   mSec(10);
    LCD_RSTn = 0; mSec(5);
    LCD_RSTn = 1; mSec(10);

    // establish 4-bit data mode
    lcdNibble(0x03);
    uSec(LCD_WAIT_US);
    lcdNibble(0x03);
    uSec(LCD_WAIT_US);
    lcdNibble(0x03);
    uSec(LCD_WAIT_US);
    lcdNibble(0x02);
    mSec(5);

    // initialise
    lcdCmd(0x2A);   // 4-bit data length extension Bit RE=1; REV=0
    lcdCmd(0x09);   // 4-line display
    lcdCmd(0x05);   // top view
    lcdCmd(0x1E);   // bias setting BS1=1
    lcdCmd(0x29);   // 4-bit data length extension Bit RE=0; IS=1
    lcdCmd(0x1B);   // BS0=1 -> Bias=1/6
    lcdCmd(0x6E);   // divider on and set value
    lcdCmd(0x57);   // booster on and set contrast (BB1=C5, DB0=C4)
    lcdCmd(0x72);   // set contrast (DB3-DB0=C3-C0)
    lcdCmd(0x28);   // 4-bit data length extension Bit RE=0; IS=0

    lcdCls();
    lcdCtrl(DISPLAY /* | CURSOR | BLINK */);
    lcdSelectROM(ROM_B);
}


// wait while the LCD is busy
void lcdWait(void) {
    uint8_t r, t;
    LCD_PORT_D &= ~(0b1111 << LCD_OFFSET);
    LCD_PORT_D |= ((0b1111 * DIR_INPUT) << LCD_OFFSET); // set the data port as input
    LCD_RS = 0;
    LCD_RWn = 1;
    uSec(LCD_WAIT_US);
    do {
        E_PULSE();
        r = (LCD_PORT >> LCD_OFFSET) & 0b1111;
        uSec(LCD_WAIT_US);
        E_PULSE();
        t = (LCD_PORT >> LCD_OFFSET) & 0b1111;
        uSec(LCD_WAIT_US);
        r = (uint8_t) (r << 4) + t;
    } while(r & 0b10000000);    //check for set busy flag
    LCD_PORT_D &= ~(0b1111 << LCD_OFFSET);
    LCD_PORT_D |= ((0b1111 * DIR_OUTPUT) << LCD_OFFSET);    // set the data port as output
}


// write a nibble
void lcdNibble(uint8_t data4) {
    LCD_LAT &= ~(0b1111 << LCD_OFFSET);
    LCD_LAT |= ((data4 & 0b1111) << LCD_OFFSET);
    E_PULSE();
}


// write data
void lcdData(uint8_t data) {
    lcdWait();
    LCD_RS = 1;
    LCD_RWn = 0;
    lcdNibble(data >> 4);   // high nibble
    uSec(LCD_WAIT_US);
    lcdNibble(data);        // low nibble
    uSec(LCD_WAIT_US);
}


// write command
void lcdCmd(uint8_t data) {
    lcdWait();
    LCD_RS = 0;
    LCD_RWn = 0;
    lcdNibble(data >> 4);   // high nibble
    uSec(LCD_WAIT_US);
    lcdNibble(data);        // low nibble
    uSec(LCD_WAIT_US);
}


// select ROM table 0..3
void lcdSelectROM(uint8_t n) {
    if(n > 3) return;   // invalid ROM number
    lcdCmd(0x2A);
    lcdCmd(0x72);
    lcdData((uint8_t) (n << 2));
    lcdCmd(0x28);
}


void lcdContrast(uint8_t contr) {
    lcdCmd(0x29);
    lcdCmd(0x54 | (contr >> 4));
    lcdCmd(0x70 | (contr & 0x0F));
    lcdCmd(0x28);
}


// clear screen and set the cursor at home position
void lcdCls(void) {
    lcdCmd(0x01);
	mSec(10);
	memset(DMEM, ' ', sizeof(DMEM));
	lcdSetPos(0, 0);
}


void lcdCtrl(uint8_t param) {
    lcdCmd(0x08 + param);
}


// set cursor position
void lcdSetPos(uint8_t px, uint8_t py) {
    posX = px; posY = py;
	if(posX >= LCD_WIDTH || posY >= LCD_HEIGHT) return;
    uint8_t addr;
	switch(posY) {
		default: return;	// undefined address
		case 0: addr = LCD_HOME_L1; break;
        case 1: addr = LCD_HOME_L2; break;
        case 2: addr = LCD_HOME_L3; break;
        case 3: addr = LCD_HOME_L4; break;
	}
	lcdCmd(LCD_DDADR | (addr + posX));
}


void lcdDrawChar(uint16_t chr) {
    if(posX >= LCD_WIDTH || posY >= LCD_HEIGHT) return;
    if(chr > 0xFF) lcdSelectROM(chr >> 8);
    DMEM[posY][posX] = (uint8_t) chr;
    lcdSetPos(posX, posY);
    lcdData((uint8_t) chr);
}


void lcdDefChar(uint8_t pcode, uint8_t *data) {
    uint8_t t = 8;
    lcdCmd(0x40 + (t * pcode));
    while(t--) lcdData(*(data++));
}


void lcdUpdateScreen(void) {
	uint8_t t, r;
	for(r = 0; r < LCD_HEIGHT; r++) {
		for(t = 0; t < LCD_WIDTH; t++) {
			lcdSetPos(t, r);
			lcdData(DMEM[r][t]);
		}
	}
}


void lcdScrollUp(void) {
	uint8_t r, x = posX, y = posY;
	for(r = 0; r < (LCD_HEIGHT - 1); r++) memcpy(DMEM[r], DMEM[r + 1], LCD_WIDTH);
	memset(DMEM[LCD_HEIGHT - 1], ' ', LCD_WIDTH);
	lcdUpdateScreen();
	lcdSetPos(x, y);
}


void lcdScrollDown(void) {
	uint8_t r, x = posX, y = posY;
	for(r = (LCD_HEIGHT - 1); r; r--) memcpy(DMEM[r - 1], DMEM[r], LCD_WIDTH);
	memset(DMEM[0], ' ', LCD_WIDTH);
	lcdUpdateScreen();
	lcdSetPos(x, y);
}


void lcdScrollLeft(void) {
	uint8_t t, r, x = posX, y = posY;
	for(r=0; r<LCD_HEIGHT; r++) {
		for(t = 0; t < (LCD_WIDTH - 1); t++) DMEM[r][t] = DMEM[r][t + 1];
	}
	for(r = 0; r < LCD_HEIGHT; r++) DMEM[r][LCD_WIDTH - 1] = ' ';
	lcdUpdateScreen();
	lcdSetPos(x, y);
}


void lcdScrollRight(void) {
	uint8_t t, r, x = posX, y = posY;
	for(r=0; r<LCD_HEIGHT; r++) {
		for(t = (LCD_WIDTH - 1); t; t--) DMEM[r][t] = DMEM[r][t - 1];
	}
	for(r = 0; r < LCD_HEIGHT; r++) DMEM[r][0] = ' ';
	lcdUpdateScreen();
	lcdSetPos(x, y);
}


void lcdPutch(uint16_t ch) {
    if(ch > 0) lcdDrawChar((uint16_t) ch);  // character 0 is 'transparent' - cursor just moves next
    if((++posX) < LCD_WIDTH) lcdSetPos(posX, posY);
    else {
        if(++posY >= LCD_HEIGHT) {
            if(sys_flags.scrlock == 0) {
                posY = LCD_HEIGHT - 1;
                lcdScrollUp();
            }
        }
        lcdSetPos(0, posY);
    }
}


// putch() hook
__attribute__ ((used))
void putch(char ch) {
	if(posX >= LCD_WIDTH || posY >= LCD_HEIGHT) return;
	uint8_t t;
	switch(ch) {

		case '\b':  // backspace
			if(posX) lcdSetPos(posX - 1, posY);
			lcdDrawChar(' ');
			break;

		case '\t':  // tab
            do {
                lcdPutch(' ');
            } while(posX % 2);  // a tab is two columns
			break;

		case '\r':  // carriage return
			lcdSetPos(0, posY);
			break;

		case '\n':  // new line (scroll up after the most bottom line)
			if(++posY >= LCD_HEIGHT) {
				posY = LCD_HEIGHT - 1;
				if(sys_flags.scrlock == 0) lcdScrollUp();
			}
			lcdSetPos(posX, posY);
			break;

		case '\f':  // form feed
			lcdCls();   // clear screen and set cursor at position 0,0
			break;

		case '\x1A':    // clear to the end of the current line
			for(t = posX; t < LCD_WIDTH; t++) DMEM[posY][t] = ' ';
            lcdUpdateScreen();
			break;

		default:
			lcdPutch(ch);
			break;
	}
}


// puts() hook
__attribute__ ((used))
int puts(const char *str) {
    int n = 0;
    while(str && *str) { lcdPutch(*(str++)); n++; }
    return n;
}
