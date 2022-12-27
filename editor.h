#if 0
/*

Simple text editor


=== Keys

Enter                   - go to new line or split the existing line
Esc                     - toggle command mode
Arrow keys              - move cursor to direction
Tab / Shift-Tab         - tabulations forward/backward
Backspace               - delete and move left (also merge lines)
Shift-Backspace         - delete character under the cursor
Shift-Left/Right        - to start of line / to end of line
Shift-Up/Down           - page up / page down
Ctrl-Shift-Up/Down      - to start of text / to end of text
Ctrl-1                  - mini help
Ctrl-Y                  - erase all text in a line
Ctrl-Z                  - restore the last erased line of text


=== Commands

RUN       [ filename ]  - run program in memory or from a file
SAVE      [ filename ]  - save file
LOAD      [ filename ]  - load file
NEW       [ filename ]  - new file
RECEIVE   [ filename ]  - receive text through the XMODEM port and optionally give it a filename
SEND                    - send text through the XMODEM port
FILES                   - list the currently stored files
DELETE    <filename>    - delete file
ERASE ALL               - erase the entire storage and all files

*/
#endif

#ifndef EDITOR_H
#define	EDITOR_H

#include "common.h"

#define MAX_TEXT_LEN    ((MEMORY_SIZE_QKB * 256ul) - 192)   // maximum length of the entire text
#define MAX_LINE_LEN    127         // maximum length of a single line in the text

#define RUN_IN_PLACE    0           // enable "run without loading" function for files in the flash drive
                                    // NOTE: cannot be used with PIC18 but to 16-bit default pointer size
                                    //       (unless all relevant pointers in the code are made far type)

__persistent char *line_buffer;     // text line buffer
__persistent char *TEXT;            // array for the text editor
__persistent char *LINE;            // pointer to the current line in the text
__persistent uint32_t lines_TEXT;   // total number of lines in the text

#define FILENAME_LEN    (FLASH_FILE_HEADER - 3)
__persistent char FILENAME[FILENAME_LEN + 1];   // current filename

#define ETX             0x1F        // NOTE: this is NOT the standard ASCII ETX code

void editor(char *text);
void allocText(int8_t mode);

#endif
