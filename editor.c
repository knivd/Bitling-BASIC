#include "editor.h"
#include "basic.h"  // for the runBasic() function
#include "xmodem/xmodem.h"

#define skipSpacesCmd() { while(*cmd == ' ') cmd++; }

typedef struct {
    uint16_t hwin;
    uint32_t vwin;
    uint8_t hcur;   // cursor within the window
    uint8_t vcur;   // line within the window
    enum { CHG_NONE = 0, CHG_REDRAW, CHG_STORE, CHG_CMDMODE, CHG_SPLIT, CHG_MERGE } chgv;
} editor_vars_t;
char *pTEXT;

const char *helpk = "  Esc; #Bs; ^Y; ^Z\0"
                    "#Up/#Dn (page Up/Dn)\0"
                    "#Lf/#Rt (line B/End)\0"
                    "#^Up/#^Dn (tx B/End)\0"
;

const char *helpc = "  NEW/LOAD/SAVE [fn]\0"
                    "DELETE fn; ERASE ALL\0"
                    "SEND; RECEIVE [fn]\0"
                    "FILES; RUN [fn]\0"
;

// store single received xmodem block
// this function is required by XmodemReceive()
int storeBlock(void *funcCtx, void *xmodemBuffer, int xmodemSize) {
    char *p = xmodemBuffer;
    int c = xmodemSize;
    while(c) {
        if(*p == '\r' || *p == '\n') {
            *(pTEXT++) = 0; // replace line endings with 0 (works with either single '\r' or '\n', or double '\r\n' or '\r\n'
            if(c && ((*p == '\r' || *(p + 1) == '\n') || (*p == '\n' || *(p + 1) == '\r'))) { p++; c--; }
        }
        else if(*p < ' ' && *p != '\0' && *p != '\t') *(pTEXT++) = ETX; // ignore unsupported non-printable codes
        else *(pTEXT++) = *p;
        p++; c--;
    }
    return xmodemSize;
}


// fetch single xmodem block to send
// this function is required by XmodemTransmit()
void fetchBlock(void *funcCtx, void *xmodemBuffer, int xmodemSize) {
    //static uint8_t was_r = 0;
    char *p = xmodemBuffer;
    if(sys_flags.was_r) {   // the previous block was incomplete
        *(p++) = '\n'; xmodemSize--;
        sys_flags.was_r = 0;
    }
    while(xmodemSize) {
        sys_flags.was_r = 0;
        if(*pTEXT == 0) {
            sys_flags.was_r = 1;
            *(p++) = '\r'; xmodemSize--;
            if(xmodemSize == 0) break;
            *(p++) = '\n'; pTEXT++;
            sys_flags.was_r = 0;
        }
        else *(p++) = *(pTEXT++);
        xmodemSize--;
    }
}


// print status after executing XMODEM receive or transmit
static void xmodem_status(int r) {
    if(r >= 0) printf("OK");
    else switch(r) {
        case -1: printf("Cancelled"); break;
        case -2: printf("Sync Error"); break;
        case -3: printf("Failed Retry"); break;
        case -4: printf("Storage Error"); break;
        default: printf("ERROR"); break;
    }
}


// return the length of text from certain point onwards
static size_t textLen(char *s) {
    size_t r;
    for(r = 0; *s != ETX && r < MAX_TEXT_LEN; s++, r++);
    return r;
}


// allocate memory for the text editor
// mode 0 = allocate full memory and reset
// mode 1 = re-allocate full memory and don't reset
// mode 2 = re-allocate only used to free up memory for program execution
void allocText(int8_t mode) {
    char *p = NULL;
    x_defrag(); // force full memory optimisation
    if(mode < 2) {
        if(mode == 0) {
            x_free((byte **) &TEXT); TEXT = NULL;
        }
        p = x_malloc((byte **) &TEXT, (size_x) (MAX_TEXT_LEN + 1));     // allocate memory for the text
    }
    else {  // mode 2
        p = x_malloc((byte **) &TEXT, (size_x) (textLen(TEXT) + 1));    // adjust memory size to free up memory for execution
    }
    line_buffer = (char *) &system_buffer[sizeof(system_buffer)] - (MAX_LINE_LEN + 1);  // reuse buffer instead of allocating separate block
    if(p == NULL || line_buffer == NULL) { // the memory allocation has failed - this is a major failure
        WDTCON0bits.SEN = 0;        // disable the WDT
        printf("\r\nFATAL: No memory");
        x_list_alloc();
        uint8_t t = 100;
        while(t--) mSec(100);   // 10 seconds here and then turn off
        Reset();
    }
    if(mode == 0) {
        memset(TEXT, ETX, (MAX_TEXT_LEN + 1));
        lines_TEXT = *TEXT = 0;
        memset(FILENAME, 0, (FILENAME_LEN + 1));
        LINE = NULL;
    }
    memset(line_buffer, 0, (MAX_LINE_LEN + 1));
}


// count all lines and go to line number (start from 0)
// will return *ETX if end of text is reached
static char *gotoLine(uint32_t lnum) {
    lines_TEXT = 0;
    char *s = TEXT, *r = NULL;
    while(*s != ETX) {
        if(lnum == lines_TEXT++) r = s; // record the start of the target line
        s += (strlen(s) + 1);   // include the trailing zero
    }
    return (r ? r : s);         // if the targeted line number is too high return pointer to the end of the text
}


// print the current text window
// (l) is the current cursor line within the window
// (h) is the horizontal window offset
// (bf) is a flag to tell the function to ignore the text buffer and only use the input string
static void textWindow(char *s, uint8_t l, uint16_t h, char bf) {
    uint16_t lb = strlen(line_buffer);
    uint8_t t, r;
    for(r = 0; r < LCD_HEIGHT; r++) {
        uint16_t ls = ((s && *s != ETX) ? strlen(s) : 0);
        for(t = 0; t < LCD_WIDTH; t++) {
            lcdSetPos(t, r);
            if(r != l || bf) lcdDrawChar(((h + t) < ls) ? s[h + t] : ' ');
            else lcdDrawChar(((h + t) < lb) ? line_buffer[h + t] : ' ');
        }
        if(s && *s != ETX) s += (strlen(s) + 1);
    }
}


// display error/info message and wait for key pressed
// with parameter NULL will only clear the info line
// with parameter (keyf) will wait for key pressed
static int message(const char *msg, uint8_t keyf) {
    uint8_t t;
    for(t = 0; t < LCD_WIDTH; t++) {
        lcdSetPos(t, (LCD_HEIGHT - 1));
        if(msg && t < strlen(msg)) lcdDrawChar(msg[t]);
        else lcdDrawChar(' ');
    }
    lcdSetPos((LCD_WIDTH - 1), (LCD_HEIGHT - 1));
    if(keyf) {
        mSec(1000);
        return getchar();
    }
    else return 0;
}


// run the source in the editor
static void run(editor_vars_t *ev, __uint24 source_addr) {
    allocText(2);   // free up memory for execution
    //x_malloc((byte **) &bas, (size_x) sizeof(basic_t));   // allocate memory for the internal interpreter data
    if((void *) &bas) {
        bas.src = (char *) source_addr; // load the global source pointer
        error_code_t e = runBasic(&bas.src,
                !!(source_addr >= (__uint24) (intptr_t) MEMORY && source_addr < (__uint24) (intptr_t) (MEMORY + xmem_bytes)));
        if(e > E_OK) {  // go to the error location
            if(source_addr >= (__uint24) (intptr_t) MEMORY && source_addr < (__uint24) (intptr_t) (MEMORY + xmem_bytes)) {
                ev->hwin = ev->hcur = ev->vwin = ev->vcur = 0;
                char *c;
                for(c = TEXT; *c != ETX && *(c + 1) != ETX && c < bas.src; c++) {
                    if(*c) {
                        if(ev->hcur < (LCD_WIDTH - 1)) ev->hcur++; else ev->hwin++;
                    }
                    else {
                        ev->hwin = ev->hcur = 0;
                        if(ev->vcur < (LCD_HEIGHT - 1)) ev->vcur++; else ev->vwin++;
                    }
                }
            }
            else { message("[!] No file err pos", 0); mSec(1000); }
        }
        //x_free((byte **) &bas); bas = NULL;
    }
    else message("RUN ERR: No memory", 1);
    allocText(1);   // grab again the full memory
}


// command mode for the text editor
static void editor_commands(editor_vars_t *ev) {
    uint8_t t, r, cmdw = LCD_WIDTH; // width of the command editing window
    uint8_t sw = 0, sc = 0;         // window, cursor
    memset(line_buffer, 0, (MAX_LINE_LEN + 1));
    {   // initial indications
        unsigned long tl = textLen(TEXT);
        lcdCls();
        printf("File: %s\r\n%5lu byte%s\r\n%5lu free",
                FILENAME, tl, ((tl != 1) ? "s" : ""), (unsigned long) MAX_TEXT_LEN - tl);
    }
    while(1) {

        //update the information
        for(t = 0; t < LCD_WIDTH; t++) {
            lcdSetPos(t, LCD_HEIGHT - 1);
            lcdDrawChar(' ');
        }
        lcdSetPos(0, LCD_HEIGHT - 1);
        #if (LCD_WIDTH >= 16)   // display some basic information if there is enough room
            cmdw = (uint8_t) (LCD_WIDTH - printf("%u:%lu",
                                                    (unsigned int) (ev->hwin + ev->hcur + 1),
                                                    (unsigned long) (ev->vwin + ev->vcur + 1)));
        #endif
        #if (LCD_WIDTH >= 20)   // display extra prompt if there is enough room
            cmdw -= (uint8_t) printf("/%lu", (unsigned long) lines_TEXT);
        #endif
        cmdw -= (uint8_t) printf(">");
        r = (uint8_t) strlen(line_buffer);
        for(t = 0; t < cmdw; t++) { // print the command string
            lcdSetPos(LCD_WIDTH - cmdw + t, LCD_HEIGHT - 1);
            if(sw + t < r) lcdDrawChar(*(line_buffer + sw + t));
            else lcdDrawChar(' ');
        }
        lcdSetPos(LCD_WIDTH - cmdw + sc, LCD_HEIGHT - 1);

        // edit the command string
        int ch = getchar();
        if(ch == CODE_ESC) break;   // exit command mode
        if(ch == '\b') {    // backspace
            if(*line_buffer) {
                if(sw > 0) sw--;
                else if(sc > 0) sc--;
                *(line_buffer + strlen(line_buffer) - 1) = 0;
            }
        }
        if(ch >= ' ') {     // printable characters
            if(strlen(line_buffer) < MAX_LINE_LEN) {
                if((sc + 1) < cmdw) sc++;
                else if((sw + sc + 1) < MAX_LINE_LEN) sw++;
                *(line_buffer + strlen(line_buffer) + 1) = 0;
                *(line_buffer + strlen(line_buffer)) = (char) ch;
            }
        }
        if(ch != '\r') continue;   // repeat until Enter

        // process command(s) in line_buffer[] after Enter key
        message(NULL, 0);
        char *cmd = line_buffer;
        skipSpacesCmd();    // skip leading spaces
        char *cmde = cmd + strlen(cmd) - 1;
        while(cmde >= cmd && *cmde == ' ') *(cmde--) = 0;   // trim trailing spaces
        if(*cmd == 0) continue;

        // new file or receive text over serial line
        if((!istrncmp(line_buffer, "new", 3) && (*(line_buffer + 3) == 0 || *(line_buffer + 3) == ' ')) ||
                (!istrncmp(line_buffer, "receive", 7) && (*(line_buffer + 7) == 0 || *(line_buffer + 7) == ' '))) {
            uint8_t n = (toupper(*line_buffer) == 'N' ? 3 : 7);
            cmd += n; skipSpacesCmd();
            if(*cmd) {  // specified a new filename so overwrite the existing one
                memset(FILENAME, 0, (FILENAME_LEN + 1));
                strncpy(FILENAME, cmd, FILENAME_LEN);
            }
            memset(TEXT, ETX, (MAX_TEXT_LEN + 1));
            lines_TEXT = *TEXT = 0;
            memset((uint8_t *) ev, 0, sizeof(editor_vars_t));
            memset(line_buffer, 0, (MAX_LINE_LEN + 1));
            if(n == 7) {    // receive new text
                message("[XMODEM] Receiving", 0);
                printf("\r\n");
                pTEXT = TEXT;
                openXMPort(XM_BAUDRATE);
                xmodem_status(XmodemReceive(storeBlock, NULL, MAX_TEXT_LEN, 0, 0));
                closeXMPort();
                printf("\r\n%lu bytes", (unsigned long) textLen(TEXT));
                getchar();
            }
            LINE = gotoLine(0);
            break;
        }

        // send the text over serial line
        if(!istrncmp(line_buffer, "send", 8) && *(line_buffer + 8) == 0) {
            cmd += 8; skipSpacesCmd();
            message("[XMODEM] Sending", 0);
            printf("\r\n");
            size_t tl = 0;
            pTEXT = TEXT;
            while(*pTEXT != ETX) {  // calculate the length of text with '\r\n' line endings instead of '\0'
                if(*(pTEXT++) == 0) tl++;
                tl++;
            }
            pTEXT = TEXT;
            openXMPort(XM_BAUDRATE);
            xmodem_status(XmodemTransmit(fetchBlock, NULL, (int) tl, 0, 0));
            closeXMPort();
            printf("\r\n%lu bytes", (unsigned long) textLen(TEXT));
            getchar();
            break;
        }

        // save file
        if(!istrncmp(line_buffer, "save", 4) && (*(line_buffer + 4) == 0 || *(line_buffer + 4) == ' ')) {
            cmd += 4; skipSpacesCmd();
            #if 0
            if(*FILENAME == 0) {    // no filename exists so create one automatically with random name
                do {
                    strcpy(FILENAME, "file");
                    sprintf(&FILENAME[strlen(FILENAME)], "%04u", (rand() % 10000));
                } while(flash_findFile(FILENAME) >= 0);
            }
            #endif
            if(*cmd) {  // specified a new filename so overwrite the existing one
                memset(FILENAME, 0, (FILENAME_LEN + 1));
                strncpy(FILENAME, cmd, FILENAME_LEN);
            }
            if(*FILENAME) {
                lcdSetPos(0, (LCD_HEIGHT - 1));
                printf("Saving file...");
                long p = flash_writeFile(FILENAME, textLen(TEXT), TEXT, 'W');
                if(p >= 0) {
                    flash_getHeader(p, &p, &cmd);   // length will come in bytes here
                    p = flash_b2p(FLASH_FILE_HEADER + p);
                    lcdSetPos(0, (LCD_HEIGHT - 1));
                    printf("Saved %li sector%s", (long) p, ((p != 1) ? "s" : ""));
                    mSec(1000);
                }
                else message("ERR: saving failed", 1);
            }
            else message("ERR: name required", 1);
            break;
        }

        #if RUN_IN_PLACE > 0
            // run the file in the editor or from flash (without loading)
            if(!istrncmp(line_buffer, "run", 3) && (*(line_buffer + 3) == 0 || *(line_buffer + 3) == ' ')) {
                cmd += 3; skipSpacesCmd();
                if(*cmd == 0) { run(ev, (__uint24) (intptr_t) TEXT); break; }   // run the file in the editor
                skipSpacesCmd();
                long p = flash_findFile(cmd);
                if(p >= 0) {
                    p = FLASH_START_ADDRESS + (p * FLASH_WRITABLE_BYTES);
                    run(ev, (__uint24) p);
                }
                else message("ERR: loading failed", 1);
                break;
            }
        #endif

        // load file or load and run file or run the content in memory
        if((!istrncmp(line_buffer, "load", 4) && (*(line_buffer + 4) == 0 || *(line_buffer + 4) == ' '))
                #if RUN_IN_PLACE == 0
                    || (!istrncmp(line_buffer, "run", 3) && (*(line_buffer + 3) == 0 || *(line_buffer + 3) == ' '))
                #endif
                ) {
            #if RUN_IN_PLACE == 0
                uint8_t runf = 0;   // run flag
                if(!istrncmp(line_buffer, "load", 4)) cmd += 4;
                else if(!istrncmp(line_buffer, "run", 3)) {
                    cmd += 3; skipSpacesCmd();
                    if(*cmd == 0) { run(ev, (__uint24) (intptr_t) TEXT); break; }   // disregard any loading and run the content in memory
                    else runf = 1;  // run file after loading
                }
            #else
                cmd += 4;
            #endif
            skipSpacesCmd();
            if(*cmd) {  // a new filename is supplied
                memset(FILENAME, 0, (FILENAME_LEN + 1));
                strncpy(FILENAME, cmd, FILENAME_LEN);
            }
            long p = flash_findFile(FILENAME);
            if(p >= 0) {
                lcdSetPos(0, (LCD_HEIGHT - 1));
                printf("Loading file...");
                memset(TEXT, ETX, (MAX_TEXT_LEN + 1));
                lines_TEXT = *TEXT = 0;
                memset((uint8_t *) ev, 0, sizeof(editor_vars_t));
                long offs = 0;
                flash_readFile(p, &offs, MAX_TEXT_LEN, TEXT);
                memset(line_buffer, 0, (MAX_LINE_LEN + 1));
                LINE = gotoLine(0);
                #if RUN_IN_PLACE == 0
                    if(runf) run(ev, (__uint24) (intptr_t) TEXT);
                #endif
            }
            else message("ERR: loading failed", 1);
            break;
        }

        // delete file
        if(!istrncmp(line_buffer, "delete", 6) && (*(line_buffer + 6) == 0 || *(line_buffer + 6) == ' ')) {
            cmd += 6; skipSpacesCmd();
            if(*cmd) {
                long p = flash_findFile(cmd);
                if(p >= 0) {
                    message("Deleting file...", 0);
                    flash_deleteFile(p);
                    message("File deleted", 0);
                    mSec(1000);
                }
                else message("ERR: delete failed", 1);
            }
            else message("ERR: name required", 1);
            break;
        }

        // list stored filenames
        if(!istrncmp(line_buffer, "files", 5) && (*(line_buffer + 5) == 0 || *(line_buffer + 5) == ' ')) {
            cmd += 5; skipSpacesCmd();

            // TODO: insert name mask support here?

            lcdCls();
            char *fn;
            int32_t flen;
            int8_t lc = LCD_HEIGHT;
            uint32_t total_len = 0, files = 0, p = 0;
            while(p < FLASH_NUMBER_PAGES) {
                flash_getHeader((long) p, &flen, &fn);   // length will come in bytes here
                if(flen == 0 || flen == -1) break;
                flen = flash_b2p(FLASH_FILE_HEADER + flen);
                if(--lc <= 0) {
                    lc = LCD_HEIGHT - 1;
                    if(message("Press key...", 1) == CODE_ESC) break;
                    message(NULL, 0);
                    lcdSetPos(0, (LCD_HEIGHT - 1));
                }
                printf("%4li %s\r\n", flen, fn);
                files++; total_len += (uint32_t) flen;
                p += (uint32_t) flen;
            }
            total_len = FLASH_SIZE_SECTORS - total_len;
            lcdSetPos(0, (LCD_HEIGHT - 1));
            printf("%lu file%s %lu free", files, ((files != 1) ? "s" : ""), total_len);
            if(getchar() == CODE_ESC) { *line_buffer = sw = sc = 0; continue; }
            break;
        }

        // erase the entire file storage
        if(!istrncmp(line_buffer, "erase all", 9) && *(line_buffer + 9) == 0) {
            cmd += 9; skipSpacesCmd();
            message("Erasing storage...", 0);
            flash_eraseAll();
            message("Storage erased", 0);
            mSec(1000);
            break;
        }

        // mini help (commands)
        if(!istrncmp(line_buffer, "help", 4) && *(line_buffer + 4) == 0) {
            cmd += 4; skipSpacesCmd();
            textWindow((char *) helpc, 0, 0, 1);
            getchar();
            break;
        }

    }
}


// microscopic text editor
void editor(char *text) {
    if(text == NULL) return;
    lcdCursorOn();
    //bas = NULL;
    editor_vars_t ev;
    memset((uint8_t *) &ev, 0, sizeof(editor_vars_t));
    ev.chgv = CHG_REDRAW;
    char *sw = NULL;
    while(1) {

        // refresh the window
        if(ev.chgv != CHG_NONE) {

            if((ev.chgv != CHG_REDRAW) && LINE && *LINE != ETX) {
                int32_t sr = (int32_t) strlen(LINE) - (int32_t) strlen(line_buffer);
                if(sr < 0) memmove((LINE - sr), LINE, textLen(LINE) + 1);   // old text is shorter
                else if(sr > 0) memmove(LINE, (LINE + sr), textLen(LINE + sr) + 1); // old text is longer
                memmove(LINE, line_buffer, strlen(line_buffer) + 1);        // store the line buffer including the trailing zero
            }

            memset(line_buffer, 0, (MAX_LINE_LEN + 1));
            LINE = gotoLine(ev.vwin + ev.vcur);
            if(LINE && *LINE != ETX) strncpy(line_buffer, LINE, MAX_LINE_LEN);

            if((ev.hwin + ev.hcur) > strlen(line_buffer)) { // adjust the horizontal position to the length of the new line
                ev.hwin = ev.hcur = 0;
                while((ev.hwin + ev.hcur) < strlen(line_buffer)) {
                    if(ev.hcur < (LCD_WIDTH - 1)) ev.hcur++;
                    else ev.hwin++;
                }
            }

            if(ev.chgv == CHG_MERGE && LINE && *LINE != ETX) {      // merge lines
                memmove((LINE + strlen(LINE)), (LINE + strlen(LINE) + 1), textLen(LINE + strlen(LINE) + 1));
                if(textLen(LINE) > 1) *(LINE + textLen(LINE) - 1) = ETX;
                if(*LINE != ETX && strlen(LINE) > MAX_LINE_LEN) {   // if the merged line is longer than possible split again
                    memmove((LINE + MAX_LINE_LEN + 1), (LINE + MAX_LINE_LEN), textLen(LINE + MAX_LINE_LEN));
                    *(LINE + MAX_LINE_LEN) = 0;
                    ev.hwin = MAX_LINE_LEN; ev.hcur = 0;
                }
                ev.chgv = CHG_REDRAW;
                continue;
            }

            if(ev.chgv == CHG_SPLIT) {          // split lines
                memmove((LINE + ev.hwin + ev.hcur + 1), (LINE + ev.hwin + ev.hcur), textLen(LINE + ev.hwin + ev.hcur) + 1);
                *(LINE + ev.hwin + ev.hcur) = 0;
                ev.hwin = ev.hcur = 0;
                if(ev.vcur < (LCD_HEIGHT - 1)) ev.vcur++;
                else ev.vwin++;
                ev.chgv = CHG_REDRAW;
                continue;
            }

            sw = gotoLine(ev.vwin);
        }

        textWindow((sw ? sw : text), ev.vcur, ev.hwin, 0);
        if(LINE == NULL) LINE = text;
        if(ev.chgv == CHG_CMDMODE) {
            editor_commands(&ev);   // command mode
            ev.chgv = CHG_REDRAW;   // update the main screen and reload the line buffer
            continue;
        }
        ev.chgv = CHG_NONE;
        lcdSetPos(ev.hcur, ev.vcur);
        int ch = getchar();

        // Esc - enter command mode
        if(ch == CODE_ESC) {
            srand((unsigned int) (textLen(TEXT) + ev.hwin + ev.hcur + ev.vwin + ev.vcur));
            ev.chgv = CHG_CMDMODE;
            continue;
        }

        // Enter will split line and insert a new line
        if(ch == '\r') {
            if(textLen(text) < MAX_TEXT_LEN) ev.chgv = CHG_SPLIT;
            continue;
        }

        // backspace
        if(ch == '\b') {
            if((ev.hwin + ev.hcur) > 0) {
                if(ev.hwin > 0) ev.hwin--;
                else if(ev.hcur > 0) ev.hcur--;
                memmove((line_buffer + ev.hwin + ev.hcur),
                        (line_buffer + ev.hwin + ev.hcur + 1),
                        (strlen(line_buffer + ev.hwin + ev.hcur + 1) + 1));
            }
            else if((ev.vwin + ev.vcur) > 0) {  // merge with the previous line
                if(ev.vcur > 0) ev.vcur--;
                else ev.vwin--;
                ev.hwin = MAX_LINE_LEN; ev.hcur = 0;
                ev.chgv = CHG_MERGE;
            }
            continue;
        }

        //delete
        if(ch == '\x7F') {
            if(*(line_buffer + ev.hwin + ev.hcur))
                memmove((line_buffer + ev.hwin + ev.hcur),
                        (line_buffer + ev.hwin + ev.hcur + 1),
                        (strlen(line_buffer + ev.hwin + ev.hcur + 1) + 1));
            continue;
        }

        // Ctrl-Y to erase a full line
        if((kbd_flags & KBD_FLAG_CTRL) && toupper(ch) == 'Y') {
            strcpy((char *) system_buffer, line_buffer);
            *line_buffer = 0;
            ev.hwin = ev.hcur = 0;
            continue;
        }

        // Ctrl-Z to restore erased line
        if((kbd_flags & KBD_FLAG_CTRL) && toupper(ch) == 'Z') {
            strcpy(line_buffer, (char *) system_buffer);
            ev.hwin = ev.hcur = 0;
            continue;
        }

        // Ctrl-1 for mini help (keys)
        if((kbd_flags & KBD_FLAG_CTRL) && toupper(ch) == '1') {
            textWindow((char *) helpk, 0, 0, 1);
            getchar();
            continue;
        }

        // printable characters
        if(ch >= ' ') {
            if(strlen(line_buffer) < MAX_LINE_LEN) {
                memmove((line_buffer + ev.hwin + ev.hcur + 1), (line_buffer + ev.hwin + ev.hcur), (strlen(line_buffer) + 1));
                *(line_buffer + ev.hwin + ev.hcur) = (char) ch;
                ch = CODE_RIGHT;
            }
            else continue;
        }

        // basic control keys
        if(ch == CODE_RIGHT || ch == CODE_END) {
            uint8_t t = ((ch == CODE_RIGHT) ? 1 : MAX_LINE_LEN);
            while(t-- && (ev.hwin + ev.hcur) < strlen(line_buffer)) {
                if(ev.hcur < (LCD_WIDTH - 1)) ev.hcur++;
                else ev.hwin++;
            }
        }
        else if(ch == CODE_LEFT || ch == CODE_HOME) {
            uint8_t t = ((ch == CODE_LEFT) ? 1 : MAX_LINE_LEN);
            while(t-- && (ev.hwin + ev.hcur) > 0) {
                if(ev.hcur > 0) ev.hcur--;
                else ev.hwin--;
            }
        }
        else if(ch == CODE_UP || ch == CODE_PGUP) {
            uint32_t t = ((ch == CODE_UP) ? 1 : LCD_HEIGHT);
            if(kbd_flags & KBD_FLAG_CTRL) { // in combination with Ctrl will go to the very beginning
                ev.hwin = ev.hcur = 0;
                t = lines_TEXT;
            }
            while(t-- && (ev.vwin + ev.vcur) > 0) {
                if(ev.vcur > 0) ev.vcur--;
                else ev.vwin--;
            }
            if((ev.hwin + ev.hcur) == strlen(line_buffer)) { ev.hwin = MAX_LINE_LEN; ev.hcur = 0; }
            ev.chgv = CHG_STORE;
        }
        else if(ch == CODE_DOWN || ch == CODE_PGDN) {
            uint32_t t = ((ch == CODE_DOWN) ? 1 : LCD_HEIGHT);
            if(kbd_flags & KBD_FLAG_CTRL) { // in combination with Ctrl will go to the very end
                ev.hwin = MAX_LINE_LEN; ev.hcur = 0;
                t = lines_TEXT;
            }
            while(t-- && (ev.vwin + ev.vcur + 1) < lines_TEXT) {
                if(ev.vcur < (LCD_HEIGHT - 1)) ev.vcur++;
                else ev.vwin++;
            }
            if((ev.hwin + ev.hcur) == strlen(line_buffer)) { ev.hwin = MAX_LINE_LEN; ev.hcur = 0; }
            ev.chgv = CHG_STORE;
        }

    }
}
