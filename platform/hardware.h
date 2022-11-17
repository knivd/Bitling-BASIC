#ifndef HARDWARE_H
#define	HARDWARE_H

#include <xc.h>
#include "../common.h"

// global RAM buffer for flash and other operations
// * IMPORTANT: the size of the buffer MUST be at least as much as the size of erasable flash sector
// * IMPORTANT: the address is hardware-dependent and fixed in PIC18 !!
__at(0x3700)
uint8_t system_buffer[256];

// default CPU frequency in MHz when the system is working
#define _DEF_FREQ_MHZ   8

// UART parameters for transmit/receive operations in the editor
#define TX_PIN      B,3
#define RX_PIN      B,4
#define RX_PPS      0x0C    // PPS code for the RX_PIN line (A:00..07, B:08..0F, C:10..17, ...)
#define XM_BAUDRATE 9600    // default baudrate for XMODEM transfers

void systemInit(void);
void systemSleep(void);
void openXMPort(uint32_t baudrate);
void closeXMPort(void);
int _inbyte(unsigned short timeout_ms);
void _outbyte(int c);
void setTimer(uint32_t val);
uint32_t getTimer(void);
uint16_t adcRead(uint8_t ch);

uint8_t sysMhz;
void setSysFreq(uint8_t freqMhz);

#define uSec(us) __delay_us(us)
#define mSec(ms) __delay_ms(ms)

// port directions for direction (TRIS) registers
#define DIR_INPUT   1
#define DIR_OUTPUT  0

// additional macros
#define UNLOCK_PPS()    { PPSLOCK = 0x55; PPSLOCK = 0xAA; PPSLOCKbits.PPSLOCKED = 0; } // unlock PPS
#define LOCK_PPS()      { PPSLOCKbits.PPSLOCKED = 1; } // lock PPS

// default CPU frequency at start
#define _XTAL_FREQ      (_DEF_FREQ_MHZ * 1000000)

// MCU configuration bits
#pragma config FEXTOSC  = OFF
#pragma config RSTOSC   = HFINTOSC_64MHZ
#pragma config CLKOUTEN = OFF
#pragma config PR1WAY   = OFF
#pragma config CSWEN    = ON
#pragma config JTAGEN   = OFF
#pragma config FCMEN    = OFF
#pragma config FCMENP   = OFF
#pragma config FCMENS   = OFF
#pragma config MCLRE    = EXTMCLR
#pragma config PWRTS    = PWRT_16
#pragma config MVECEN   = OFF
#pragma config IVT1WAY  = OFF
#pragma config LPBOREN  = ON
#pragma config BOREN    = NOSLP
#pragma config BORV     = VBOR_2P45
#pragma config ZCD      = OFF
#pragma config PPS1WAY  = OFF
#pragma config STVREN   = ON
#pragma config LVP      = OFF
#pragma config XINST    = OFF
#pragma config WDTCPS   = WDTCPS_5
#pragma config WDTE     = SWDTEN
#pragma config WDTCWS   = WDTCWS_7
#pragma config WDTCCS   = LFINTOSC
#pragma config BBSIZE   = BBSIZE_1024
#pragma config BBEN     = OFF
#pragma config SAFEN    = OFF
#pragma config WRTB     = OFF
#pragma config WRTC     = OFF
#pragma config WRTD     = OFF
#pragma config WRTSAF   = OFF
#pragma config WRTAPP   = OFF
#pragma config BOOTPINSEL=RC4
#pragma config BPEN     = OFF
#pragma config ODCON    = OFF
#pragma config CP       = OFF
#pragma config BOOTSCEN = OFF
#pragma config BOOTCOE  = CONTINUE
#pragma config APPSCEN  = OFF
#pragma config SAFSCEN  = OFF
#pragma config DATASCEN = OFF
#pragma config CFGSCEN  = OFF
#pragma config COE      = CONTINUE
#pragma config BOOTPOR  = OFF
#pragma config BCRCPOLT = h00
#pragma config BCRCPOLU = h00
#pragma config BCRCPOLH = h00
#pragma config BCRCPOLL = h00
#pragma config BCRCSEEDT= h00
#pragma config BCRCSEEDU= h00
#pragma config BCRCSEEDH= h00
#pragma config BCRCSEEDL= h00
#pragma config BCRCEREST= h00
#pragma config BCRCERESU= h00
#pragma config BCRCERESH= h00
#pragma config BCRCERESL= h00
#pragma config CRCPOLT  = h00
#pragma config CRCPOLU  = h00
#pragma config CRCPOLH  = h00
#pragma config CRCPOLL  = h00
#pragma config CRCSEEDT = h00
#pragma config CRCSEEDU = h00
#pragma config CRCSEEDH = h00
#pragma config CRCSEEDL = h00
#pragma config CRCEREST = h00
#pragma config CRCERESU = h00
#pragma config CRCERESH = h00
#pragma config CRCERESL = h00

#endif
