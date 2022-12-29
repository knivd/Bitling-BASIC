#include "common.h"
#include "editor.h"


// main entry point
void main(void) {
    sys_flags.sleep ^= 1;   // toggle between active and sleep mode on reset
    if(sys_flags.sleep) systemSleep();  // enter in lowest power mode from which it can be brought back on the next reset
    systemInit();   // initialise the hardware
    printf("Bitling BASIC @KnivD\r\n");
    x_meminit();    // initialise the dynamic memory manager
    TEXT = line_buffer = NULL;
    allocText(0);   // allocate memory and reset the editor
    mSec(1000);
    while(1) editor(TEXT);  // main loop
    Reset();        // double assurance; execution should never reach this line
}
