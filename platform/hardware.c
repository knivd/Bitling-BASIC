#include "hardware.h"

uint16_t tmr_value;


void systemSleep(void) {
    di();
    LCD_ON_D = LCD_E_D = LCD_RWn_D = LCD_RS_D = LCD_RSTn_D = DIR_OUTPUT;
    LCD_ON = LCD_E = LCD_RWn = LCD_RS = LCD_RSTn = 0;
    WDTCON0bits.SEN = 0;        // disable the WDT
    PMD0 = PMD1 = PMD2 = PMD3 = PMD4 = PMD5 = PMD6 = PMD7 = PMD8 = 0xFF;    // disable all peripherals
    VREGCONbits.VREGPM = 0b10;  // ULP mode
    while(1) Sleep();           // sleep forever (until reset)
}


// set system frequency
void setSysFreq(uint8_t freqMhz) {
    OSCCON1bits.NOSC = 0b110;   // HFINTOSC
    OSCCON1bits.NDIV = 0b0000;  // div=1
    try_again:;
    switch(freqMhz) {
        case 64: OSCFRQbits.HFFRQ = 0b1000; break;
        case 48: OSCFRQbits.HFFRQ = 0b0111; break;
        case 32: OSCFRQbits.HFFRQ = 0b0110; break;
        case 24: OSCFRQbits.HFFRQ = 0b0111; OSCCON1bits.NDIV = 0b0001; break;   // clk=48, div=2
        case 16: OSCFRQbits.HFFRQ = 0b0101; break;
        case 12: OSCFRQbits.HFFRQ = 0b0100; break;
        case 8:  OSCFRQbits.HFFRQ = 0b0011; break;
        case 6:  OSCFRQbits.HFFRQ = 0b0100; OSCCON1bits.NDIV = 0b0001; break;   // clk=12, div=2
        case 4:  OSCFRQbits.HFFRQ = 0b0010; break;
        case 3:  OSCFRQbits.HFFRQ = 0b0100; OSCCON1bits.NDIV = 0b0010; break;   // clk=12, div=4
        case 2:  OSCFRQbits.HFFRQ = 0b0001; break;
        case 1:  OSCFRQbits.HFFRQ = 0b0000; break;
        default: freqMhz--; goto try_again;
    }
    sysMhz = freqMhz;
    while(OSCCON3bits.NOSCR);   // wait until the clock switching is complete
}


void systemInit(void) {
    di();
    setSysFreq(_DEF_FREQ_MHZ);
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = 0;

    T0CON1bits.T0CS = 0b0100;   // LFINTOSC clock 31250 Hz for Timer0
    T0CON1bits.T0CKPS = 0b0001; // 2:1 pre-scaler
    T0CON0bits.T0OUTPS = 0b0000;    // 1:1 post-scaler
    T0CON0bits.MD16 = 1;        // 16-bit mode
    tmr_value = 65536 - (31250 / 2 / 25);   // timer clocks to rollover = (clk / pre / Hz)
    TMR0H = (uint8_t) (tmr_value >> 8); // the internal timer counter is clocked at 15625 Hz
    TMR0L = (uint8_t) tmr_value;        // 40 milliseconds (25 Hz) are 625 clocks to rollover
    PIE3bits.TMR0IE = 1;        // enable interrupt from Timer0
    T0CON0bits.EN = 1;          // enable Timer0

    T1CLK = 0b00101;            // MFINTOSC (500 kHz) for Timer1
    T1CONbits.CKPS = 0b00;      // 1:1 pre-scaler
    T1CONbits.RD16 = 1;         // 16-bit
    T1CONbits.ON = 1;           // enable Timer1 (will be used for microsecond delays)

    lcdInit();                  // initialise the display
    kbd_flags = 0;              // clear the keyboard flags
    sys_flags.scrlock = 0;
    sys_flags.was_r = 0;
    sys_flags.timer = 0;
    ei();                       // enable interrupts
}


// system interrupts come here
void __interrupt() ISR(void) {
    if(PIR3bits.TMR0IF) {           // interrupt from Timer 0
        sys_flags.timer += 40;
        TMR0H = (uint8_t) (tmr_value >> 8);
        TMR0L = (uint8_t) tmr_value;
        PIR3bits.TMR0IF = 0;        // clear the interrupt flag
    }
}


// initialise and enable UART1 for XMODEM data transfers
void openXMPort(uint32_t baudrate) {
    U1BRG = (uint16_t) (_XTAL_FREQ / (16ul * baudrate)) - 1;    // calculate the BRG value and load the baud rate register
    U1CON0bits.MODE = 0b0000;   // 8-bit UART with no parity
    U1CON0bits.BRGS = 0;        // normal baud rate
    U1CON0bits.TXEN = 1;        // enable transmit
    U1CON0bits.RXEN = 1;        // enable receive
    U1CON2bits.RUNOVF = 1;      // continue after overflow
    U1CON1bits.ON = 1;          // enable EUSART1
    REG(TRIS, TX_PIN) = DIR_OUTPUT;
    REG_PPS(TX_PIN) = 0x20;     // route TX1 to the TX pin (TX232)
    REG(TRIS, RX_PIN) = DIR_INPUT;
    U1RXPPS = RX_PPS;           // route RX1 to the RX pin (RX232)
}


// close UART1
void closeXMPort(void) {
    REG_PPS(TX_PIN) = 0x00;
    REG(TRIS, TX_PIN) = DIR_INPUT;
    REG_PPS(RX_PIN) = 0x00;
    REG(TRIS, RX_PIN) = DIR_INPUT;
    U1CON1bits.ON = 0;
}


// needed by XMODEM functions
int _inbyte(unsigned short timeout_ms) {
	int ch = -1;
    uint32_t t = timeout_ms;
    while(t--) {
        if(PIR4bits.U1RXIF) {       // received character from UART1
            ch = (int) U1RXB;       // get the received byte
            uint8_t rf = U1ERRIR;	// get the reception flags
            if((rf & _U1ERRIR_FERIF_MASK) == 0) break;  // no errors
		}
        else mSec(1);
    }
	return ch;
}


// needed by XMODEM function
void _outbyte(int c) {
    while(U1ERRIRbits.TXMTIF == 0);
    U1TXB = (uint8_t) c;
}


void setTimer(uint32_t val) {
    T0CON0bits.EN = 0;      // disable the timer when setting a new value
    sys_flags.timer = val / 40;
    val = ((val % 40) * 15625) / 1000;
    TMR0H = (uint8_t) ((val >> 8) & 0xFF);
    TMR0L = (uint8_t) (val & 0xFF);
    T0CON0bits.EN = 1;
}


uint32_t getTimer(void) {
    uint32_t v = ((uint16_t) TMR0H << 8) + TMR0L;
    v = (v * 1000) / 15625; // convert the raw reading into milliseconds
    return sys_flags.timer + v;
}


// switch to AN channel and get a reading from the ADC
uint16_t adcRead(uint8_t ch) {
    ADCON2bits.ACLR = 1;
    ADPCH = ch;
    ADCON0bits.GO = 1;
    uint8_t t = 25;
    while(ADCON0bits.GO && --t) uSec(1);
    return (t ? (uint16_t) ADRES : (uint16_t) -1);
}
