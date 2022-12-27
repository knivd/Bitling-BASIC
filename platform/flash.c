#include "../editor.h"  // only needed for the ETX value
#include "flash.h"

// these are the hardware-dependent functions
// ==================================================================================================

unsigned char flash_readByte(long page, long offset) {
    long addr = FLASH_START_ADDRESS + (page * FLASH_WRITABLE_BYTES) + offset;
    TBLPTR = (__uint24) addr;
    asm("TBLRD*");
    return (TABLAT);
}


void flash_readPage(long page, unsigned char *buffer) {
    long t;
    for(t = 0; t < FLASH_WRITABLE_BYTES; t++) *(buffer + t) = flash_readByte(page, t);
}


void flash_writePage(long page, unsigned char *buffer) {
    unsigned char GIEBitValue = INTCON0bits.GIE;            // save interrupt enable bit value
    memmove(system_buffer, buffer, FLASH_WRITABLE_BYTES);   // copy application buffer contents to the buffer RAM
    page *= FLASH_WRITABLE_BYTES;
    page += FLASH_START_ADDRESS;
    INTCON0bits.GIE = 0;        // disable interrupts
    #ifdef _NVMCON1_CMD_POSN    // PIC18 Q-series
        NVMADR = (__uint24) page;
        NVMCON1bits.CMD = 0b101;        // page write command
        NVMLOCK = 0x55; NVMLOCK = 0xAA; // unlock sequence
        NVMCON0bits.GO = 1;     // start operation
        while(NVMCON0bits.GO);  // wait for the operation to complete
        NVMCON1bits.CMD = 0x00;
    #else                       // PIC18 K-series
        TBLPTR = (__uint24) page;
        NVMCON1bits.REG0 = 0;   // point to program flash memory
        NVMCON1bits.REG1 = 1;   // access to program flash memory
        NVMCON1bits.WREN = 1;   // enable write to memory
        NVMCON1bits.FREE = 0;   // do not erase
        NVMCON2 = 0x55; NVMCON2 = 0xAA; // unlock sequence
        NVMCON1bits.WR = 1;     // write (CPU stalls)
        NOP();
        NVMCON1bits.WREN = 0;
    #endif
    INTCON0bits.GIE = GIEBitValue;
}


void flash_erasePage(long page) {
    unsigned char GIEBitValue = INTCON0bits.GIE;    // save interrupt enable bit value
    page *= FLASH_WRITABLE_BYTES;
    page += FLASH_START_ADDRESS;
    INTCON0bits.GIE = 0;        // disable interrupts
    #ifdef _NVMCON1_CMD_POSN    // PIC18 Q-series
        NVMADR = (__uint24) page;
        NVMCON1bits.CMD = 0b110;        // page erase command
        NVMLOCK = 0x55; NVMLOCK = 0xAA; // unlock sequence
        NVMCON0bits.GO = 1;     // start operation
        while(NVMCON0bits.GO);  // wait for the operation to complete
        NVMCON1bits.CMD = 0x00;
    #else                       // PIC18 K-series
        TBLPTR = (__uint24) page;
        NVMCON1bits.REG0 = 0;   // point to program flash memory
        NVMCON1bits.REG1 = 1;   // access to program flash memory
        NVMCON1bits.WREN = 1;   // enable write to memory
        NVMCON1bits.FREE = 1;   // enable block erase operation
        NVMCON2 = 0x55; NVMCON2 = 0xAA; // unlock sequence
        NVMCON1bits.WR = 1;     // start erase (CPU stalls)
        NOP();
        NVMCON1bits.WREN = 0;
    #endif
    INTCON0bits.GIE = GIEBitValue;
}

// ==================================================================================================


// convert file length in bytes into number of writable pages needed for it
long flash_b2p(long len) {
    if(len % FLASH_WRITABLE_BYTES) len += FLASH_WRITABLE_BYTES;
    len /= FLASH_WRITABLE_BYTES;
    return len;
}


long flash_writeFile(char *name, long len, void *data, char opt) {
    #if FLASH_SIZE_SECTORS > 0
        long avlp = 0, b = -1;
        while(avlp < FLASH_NUMBER_PAGES && b) { // find the first available page in (avlp)
            flash_getHeader(avlp, &b, NULL);
            if(b == 0 || b == -1) { b = 0; break; }
            avlp += flash_b2p(FLASH_FILE_HEADER + b);
        }
        long cl = 0;    // current file length in bytes
        long cp = flash_findFile(name); // current file length in pages
        long p = avlp;  // page from where will start writing
        if(cp >= 0) {   // this file already exists so it will be an append operation
            char *n;
            flash_getHeader(cp, &cl, &n);   // get the file length
            p = cp;
            cp = flash_b2p(FLASH_FILE_HEADER + cl);
        }
        else cp = 0;
        if(opt == 'A' || opt == 'a') len += cl;         // add the current file length
        else if(opt != 'W' && opt != 'w') return -1;    // unknown operation
        long np = flash_b2p(FLASH_FILE_HEADER + len);               // calculate the number of needed pages
        if(b || (p + (np - cp)) > FLASH_NUMBER_PAGES) return -1;    // not enough free space
        long diff = flash_b2p(FLASH_FILE_HEADER + len) - flash_b2p(FLASH_FILE_HEADER + cl); // difference in pages
        long wp;
        if(cp && diff > 0) {        // file is expanding
            for(wp = avlp - 1; wp >= p; wp--) {
                flash_readPage(wp, system_buffer);
                flash_erasePage(wp + diff);
                flash_writePage(wp + diff, system_buffer);
            }
        }
        else if(cp && diff < 0) {   // file is shrinking
            for(wp = p; wp < avlp; wp++) {
                flash_readPage(wp, system_buffer);
                flash_erasePage(wp + diff);
                flash_writePage(wp + diff, system_buffer);
            }
        }
        unsigned char *dp = (unsigned char *) data;
        b = FLASH_FILE_HEADER;      // start inserting data after the file header
        wp = p; diff = 0;
        do {
            flash_readPage(wp, system_buffer);
            flash_erasePage(wp);    // NOTE: this will only work with equal size of an erasable page and a writable page
            if(diff == 0) {         // create file header in the beginning of the first page of the file
                memset(system_buffer, 0, FLASH_WRITABLE_BYTES);
                strncpy((char *) system_buffer + 4, name, (FLASH_FILE_HEADER - 5));
                *(system_buffer + 3) = (unsigned char) (len >> 24);
                *(system_buffer + 2) = (unsigned char) (len >> 16);
                *(system_buffer + 1) = (unsigned char) (len >> 8);
                *system_buffer = (unsigned char) len;
            }
            for( ; len && b < FLASH_WRITABLE_BYTES; len--, diff++) {
                if(diff >= cl || opt == 'W' || opt == 'w') *(system_buffer + b++) = *(dp++);
            }
            if(len == 0) {
                if(b < FLASH_WRITABLE_BYTES) *(system_buffer + b++) = 0;    // fill the remainder of the last page
                while(b < FLASH_WRITABLE_BYTES) *(system_buffer + b++) = ETX;
            }
            flash_writePage(wp++, system_buffer);
            memset(system_buffer, 0, FLASH_WRITABLE_BYTES);
            b = 0;
        } while(len > 0);
        return p;       // return the starting page of the file
    #else
        return -1;
    #endif
}


long flash_readFile(long p, long *offs, long maxlen, void *buffer) {
    #if FLASH_SIZE_SECTORS > 0
        if(buffer == NULL) return 0;
        flash_readPage(p, system_buffer);
        long b = ((long) flash_readByte(p, 3) << 24) + ((long) flash_readByte(p, 2) << 16) +
                 ((long) flash_readByte(p, 1) << 8) + flash_readByte(p, 0);
        if(maxlen == 0 || maxlen > b) maxlen = b;
        unsigned char *dp = (unsigned char *) buffer;
        if(*offs < 0 || *offs > b) *offs = b;
        b = FLASH_FILE_HEADER + *offs;  // start reading data after the file header and add the offset
        if(b >= FLASH_WRITABLE_BYTES) { // jump to the needed page
            while(b >= FLASH_WRITABLE_BYTES) { b -= FLASH_WRITABLE_BYTES; p++; }
            flash_readPage(p, system_buffer);
        }
        long read = 0;
        maxlen -= *offs;
        while(maxlen) {
            for( ; maxlen && b < FLASH_WRITABLE_BYTES; maxlen--) {
                *(dp++) = *(system_buffer + b++);
                read++;
            }
            if(maxlen) {    // read the next page
                flash_readPage(++p, system_buffer);
                b = 0;
            }
        }
        return read;
    #else
        return -1;
    #endif
}


void flash_renameFile(long p, char *name) {
    #if FLASH_SIZE_SECTORS > 0
        if(name == NULL) return;
        long t, sectp = (p / FLASH_ERASABLE_SECTOR) * FLASH_ERASABLE_SECTOR;    // start from the beginning of the sector
        for(t = 0; t < FLASH_ERASABLE_SECTOR; t++) {    // read the entire erasable sector
            flash_readPage((sectp + t), (system_buffer + (t * FLASH_WRITABLE_BYTES)));
        }
        memset((system_buffer + ((p % FLASH_ERASABLE_SECTOR) * FLASH_WRITABLE_BYTES) + 4), 0, (FLASH_FILE_HEADER - 4));  // clear the filename field
        strncpy((char *) (system_buffer + ((p % FLASH_ERASABLE_SECTOR) * FLASH_WRITABLE_BYTES) + 4), name, (FLASH_FILE_HEADER - 5)); // copy the new name
        flash_erasePage(sectp);
        for(t = 0; t < FLASH_ERASABLE_SECTOR; t++) {    // restore the sector
            flash_writePage((sectp + t), (system_buffer + (t * FLASH_WRITABLE_BYTES)));
        }
    #endif
}


void flash_deleteFile(long p) {
    #if FLASH_SIZE_SECTORS > 0
        long flen;  // file length in pages
        flash_getHeader(p, &flen, NULL);    // (flen) comes in bytes here
        if(flen == 0 || flen == -1) return; // invalid header
        flen = flash_b2p(flen);
        memset(system_buffer, 0, FLASH_ERASABLE_BYTES);
        long sectp = (p / FLASH_ERASABLE_SECTOR) * FLASH_ERASABLE_SECTOR;   // start from the beginning of the sector
        long page = sectp;
        long pcnt = 0;
        while(page < FLASH_NUMBER_PAGES) {
            if(page < p || page >= (p + flen)) {    // skip the pages occupied by the file
                flash_readPage(page, (system_buffer + (pcnt * FLASH_WRITABLE_BYTES)));
                if(++pcnt >= FLASH_ERASABLE_SECTOR) {
                    flash_erasePage(sectp);
                    for(pcnt = 0; pcnt < FLASH_ERASABLE_SECTOR; pcnt++, sectp++) {
                        flash_writePage(sectp, (system_buffer + (pcnt * FLASH_WRITABLE_BYTES)));
                    }
                    pcnt = 0;
                    memset(system_buffer, 0, FLASH_ERASABLE_BYTES);
                }
            }
            page++;
        }
        if(pcnt) {  // flush the buffer
            flash_erasePage(sectp);
            for(pcnt = 0; pcnt < FLASH_ERASABLE_SECTOR; pcnt++, sectp++) {
                flash_writePage(sectp, (system_buffer + (pcnt * FLASH_WRITABLE_BYTES)));
            }
        }
    #endif
}


void flash_getHeader(long p, long *len, char **name) {
    #if FLASH_SIZE_SECTORS > 0
        if(len) *len = ((long) flash_readByte(p, 3) << 24) + ((long) flash_readByte(p, 2) << 16) +
                       ((long) flash_readByte(p, 1) << 8) + flash_readByte(p, 0);
        if(name) {
            *name = (char *) system_buffer;
            memset(*name, 0, FLASH_FILE_HEADER);
            uint8_t t;
            for(t = 0; t < FLASH_FILE_HEADER - 4; t++) *(*name + t) = flash_readByte(p, t + 4);
        }
    #endif
}


long flash_findFile(char *name) {
    #if FLASH_SIZE_SECTORS > 0
        if(name == NULL || *name == 0) return -1;
        long fl, p = 0, r = -1;
        char *fn;
        while(p < FLASH_NUMBER_PAGES && r == -1) {
            flash_getHeader(p, &fl, &fn);
            if(fl == 0 || fl == -1) break;
            if(!istrncmp(fn, name, (FLASH_FILE_HEADER - 4))) r = p; // found it (not case-sensitive)
            p += flash_b2p(FLASH_FILE_HEADER + fl);
        }
    #endif
    return r;
}


void flash_eraseAll(void) {
    long s;
    for(s = 0; s < FLASH_SIZE_SECTORS; s++) flash_erasePage(s);
}
