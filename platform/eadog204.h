// functions to control LCD in 4-bit interface
#ifndef EADOG204_H
#define	EADOG204_H

#include "../common.h"

// LCD control lines (directions must be set externally)
#define LCD_ON      LATCbits.LATC4  // (output) enable power to the LCD
#define LCD_E       LATCbits.LATC3  // (output) E signal
#define LCD_RWn     LATCbits.LATC2  // (output) R/W# signal
#define LCD_RS      LATCbits.LATC1  // (output) RS signal
#define LCD_RSTn    LATCbits.LATC0  // (output) RESET# signal

// direction registers
#define LCD_ON_D    TRISCbits.TRISC4
#define LCD_E_D     TRISCbits.TRISC3
#define LCD_RWn_D   TRISCbits.TRISC2
#define LCD_RS_D    TRISCbits.TRISC1
#define LCD_RSTn_D  TRISCbits.TRISC0

#define LCD_LAT     LATD    // writing here
#define LCD_PORT    PORTD   // reading from here
#define LCD_PORT_D  TRISD   // direction port
#define LCD_OFFSET  0       // bit offset for the 4-bit data interface

// LCD line addresses
#define LCD_HOME_L1	0x00
/* HD44780
#define LINE2       (LCD_HOME_L1 + 0x40)
#define LINE3       (LCD_HOME_L1 + 0x14)
#define	LINE4       (LCD_HOME_L1 + 0x14)
*/
// KS0073/SSD1803(A)
#define LCD_HOME_L2 (LCD_HOME_L1 + 0x20)
#define LCD_HOME_L3 (LCD_HOME_L1 + 0x40)
#define	LCD_HOME_L4 (LCD_HOME_L1 + 0x60)

#define LCD_CGADR   0b01000000  // CGRAM address
#define LCD_DDADR   0b10000000  // DDRAM address

#define LCD_WIDTH   20      // LCD width in number of characters
#define LCD_HEIGHT  4       // LCD height in number of characters

uint8_t DMEM[LCD_HEIGHT][LCD_WIDTH];    // screen buffer
uint8_t posX, posY;         // current cursor coordinates

// some notable control parameters for lcdCtrl()
// parameters can be OR-ed; 1=ON, 0=OFF
#define DISPLAY     BIT(2)
#define CURSOR      BIT(1)
#define BLINK       BIT(0)

// ROM table codes for lcdSelectROM()
#define ROM_A       0b00
#define ROM_B       0b01
#define ROM_C       0b10

// high-level functions
void lcdInit(void);
void lcdCls(void);
void lcdSetPos(uint8_t px, uint8_t py);
void lcdSelectROM(uint8_t n);   // 1,2,3
void lcdContrast(uint8_t contr);
void lcdDefChar(uint8_t pcode, uint8_t *data);
void lcdDrawChar(uint16_t chr); // high byte if not zero specifies ROM table
void lcdPutch(uint16_t ch);

// low-level functions
void lcdWait(void);
void lcdNibble(uint8_t data4);
void lcdData(uint8_t data);
void lcdCmd(uint8_t data);
void lcdUpdateScreen(void);
void lcdScrollUp(void);
void lcdScrollDown(void);
void lcdScrollLeft(void);
void lcdScrollRight(void);
void lcdCtrl(uint8_t param);

// standard library functions
void putch(char ch);
int puts(const char *str);

#endif
