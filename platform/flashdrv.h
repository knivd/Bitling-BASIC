#ifndef FLASHDRV_H
#define FLASHDRV_H

#include "flash.h"

// flash file structure
typedef struct {
    unsigned long len;
    char name[FLASH_FILE_HEADER - sizeof(unsigned long)];
} flash_file_t;

// flash storage array pre-loaded with demo programs

// Hello World
__at(FLASH_START_ADDRESS)
const flash_file_t HEADER_1 = {  // 1 sector
    0x92, "Hello"
};
__at(FLASH_START_ADDRESS + FLASH_FILE_HEADER)
const char FILE_1[FLASH_WRITABLE_BYTES - FLASH_FILE_HEADER] = {
    "print \"Hello, World!\"" "\0"
    "print \"From Bitling BASIC\"" "\0"
    "var integer t" "\0"
    "for t=0 to 9" "\0"
    "  if t>0 then" "\0"
    "    print \" \";" "\0"
    "  endif" "\0"
    "  print t;" "\0"
    "next" "\0"
    "\x1f"
};

// Grainbench
__at(FLASH_START_ADDRESS + (1 * FLASH_WRITABLE_BYTES))
const flash_file_t HEADER_2 = {  // 3 sectors
    0x238, "Grainbench"
};
__at(FLASH_START_ADDRESS + FLASH_FILE_HEADER + (1 * FLASH_WRITABLE_BYTES))
const char FILE_2[(3 * FLASH_WRITABLE_BYTES) - FLASH_FILE_HEADER] = {
    "var integer t,i=0" "\0"
    "var float x[100],f=0.0" "\0"
    "var string s=\"\"" "\0"
    "print \"Grainbench 2016/22\"" "\0"
    "print \"Calculating...\"" "\0"
    "s=\"\":f=0.0:i=0" "\0"
    "TIMER(1)" "\0"
    "while TIMER(0)<=30000" "\0"
    "  i=i+2:f=f+2.0002" "\0"
    "  if (i // 2)=0 then" "\0"
    "    i=i*2:i=i/2" "\0"
    "    f=f*2.0002:f=f/2.0002" "\0"
    "  endif" "\0"
    "  i=i-1" "\0"
    "  for t=1 to 100" "\0"
    "    f=f-1.0001" "\0"
    "    if (f-trunc(f))>=0.5 then" "\0"
    "      f=sin(f*logd(i))" "\0"
    "      s=str$(f,6)" "\0"
    "    endif" "\0"
    "    f=(f-tan(i))*(RND(0)/i)" "\0"
    "    if instr(s,left$(str$(i,0),2),1)>0 then " "\0"
    "      s=s+\"0\"" "\0"
    "    endif" "\0"
    "  next" "\0"
    "  x[1+(i // 100)]=f" "\0"
    "wend" "\0"
    "print \"Performance:\"" "\0"
    "print str$((i*1024)/286,0)+\" grains\"" "\0"
    "\x1f"
};

// Maze
__at(FLASH_START_ADDRESS + (4 * FLASH_WRITABLE_BYTES))
const flash_file_t HEADER_3 = {  // 12 sectors
    0xBF9, "Maze"
};
__at(FLASH_START_ADDRESS + FLASH_FILE_HEADER + (4 * FLASH_WRITABLE_BYTES))
const char FILE_4[(12 * FLASH_WRITABLE_BYTES) - FLASH_FILE_HEADER] = {
    "selcht 1:cls:cursor 0" "\0"
    "print \" FIND YOUR WAY HOME\"" "\0"
    "print \" Up arrow to walk\"" "\0"
    "print \" Left/Right to turn\"" "\0"
    "print \" Press key to start\";" "\0"
    "var int k" "\0"
    "get k:print ' discarded" "\0"
    "var string m[29]" "\0"
    "'Maze from dCode.fr" "\0"
    "m[ 1]=\"#######################################\"" "\0"
    "m[ 2]=\"#       #           #   #   #     #   #\"" "\0"
    "m[ 3]=\"# # # ### ##### ####### # ### # ### # #\"" "\0"
    "m[ 4]=\"# # #     # #       #     # # #   # # #\"" "\0"
    "m[ 5]=\"# # # ##### # # ##### # # # ### ##### #\"" "\0"
    "m[ 6]=\"# # #     # # #     # # #   #     #   #\"" "\0"
    "m[ 7]=\"##### ##### # ### ##### # ### ##### # #\"" "\0"
    "m[ 8]=\"# #   #       #     #   #           # #\"" "\0"
    "m[ 9]=\"# # ####### ### ##### ####### ### #####\"" "\0"
    "m[10]=\"#     # #   #             #   #   #   #\"" "\0"
    "m[11]=\"##### # # # # ### ##### ##### ####### #\"" "\0"
    "m[12]=\"#   # #   # # #     # #     # #       #\"" "\0"
    "m[13]=\"# ##### ### ### ##### # # # ##### # # #\"" "\0"
    "m[14]=\"#       # #   # # #     # # #   # # # #\"" "\0"
    "m[15]=\"# ####### ##### # # ### ##### ### #####\"" "\0"
    "m[16]=\"#   #       #     # #                 #\"" "\0"
    "m[17]=\"# ### # ##### ########### # # ### ### #\"" "\0"
    "m[18]=\"#     # #           # # # # #   #   # #\"" "\0"
    "m[19]=\"##### # # ########### # ####### ### ###\"" "\0"
    "m[20]=\"# #   # # # #   # # #     # # # # #   #\"" "\0"
    "m[21]=\"# ####### # # # # # ### ### # # # ### #\"" "\0"
    "m[22]=\"#   # #   # # # #       #     # #   # #\"" "\0"
    "m[23]=\"### # ### # ### ### ##### ### # # #####\"" "\0"
    "m[24]=\"# #   # #       #       #   #   #     #\"" "\0"
    "m[25]=\"# # ### ##### ### ### # ##### # # ### #\"" "\0"
    "m[26]=\"#       # # #   #   # # # # # #   # # #\"" "\0"
    "m[27]=\"####### # # ### # # # # # # # # # # # #\"" "\0"
    "m[28]=\"#                 # # #   #   # #   # $\"" "\0"
    "m[29]=\"#######################################\"" "\0"
    "var int steps=0   ' number of steps taken" "\0"
    "var int heading=1 ' 0:N, 1:E, 2:S, 3:W" "\0"
    "var int x=2,y=2   ' coordinates within the maze" "\0"
    "var int dx=0,dy=0" "\0"
    "var int dxa,dya" "\0"
    "var string pts = \"   \"" "\0"
    "var string wls = chr$(0xD6)+chr$(0xD6)+chr$(0xD6)" "\0"
    "var string exs = \" \"+chr$(0xDD)+\" \"" "\0"
    "var string z" "\0"
    "print \"Dir.L.Ahead.R.Steps\"" "\0"
    "while 1" "\0"
    "  ' what is on the left" "\0"
    "  if heading==0 then print \"N.\";:dx=-1:dy=0!" "\0"
    "  if heading==1 then print \"E.\";:dx=0:dy=-1!" "\0"
    "  if heading==2 then print \"S.\";:dx=1:dy=0!" "\0"
    "  if heading==3 then print \"W.\";:dx=0:dy=1!" "\0"
    "  update()" "\0"
    "  ' what is ahead" "\0"
    "  if heading==0 then dx=0:dy=-1!" "\0"
    "  if heading==1 then dx=1:dy=0!" "\0"
    "  if heading==2 then dx=0:dy=1!" "\0"
    "  if heading==3 then dx=-1:dy=0!" "\0"
    "  update()" "\0"
    "  dxa=dx:dya=dy ' store the character ahead" "\0"
    "  ' what is on the right" "\0"
    "  if heading==0 then dx=1:dy=0!" "\0"
    "  if heading==1 then dx=0:dy=1!" "\0"
    "  if heading==2 then dx=-1:dy=0!" "\0"
    "  if heading==3 then dx=0:dy=-1!" "\0"
    "  update()" "\0"
    "  print steps;" "\0"
    "  ' act on user input" "\0"
    "  get k" "\0"
    "  if k==0x11 then heading=(heading-1) and 3! ' Left arrow key" "\0"
    "  if k==0x12 then heading=(heading+1) and 3! ' Right arrow key" "\0"
    "  if k==0x13 and mid$(m[y+dya],x+dxa,1)<>\"#\" then ' Up arrow key" "\0"
    "    x=x+dxa:y=y+dya" "\0"
    "    steps=steps+1" "\0"
    "    if mid$(m[y],x,1)==\"$\" then exit!" "\0"
    "  endif" "\0"
    "  print" "\0"
    "wend" "\0"
    "?:print \"CONGRATULATIONS!\"" "\0"
    "print \"You made it home\"" "\0"
    "print \"in \";steps;\" steps\"" "\0"
    "end" "\0"

    "sub update" "\0"
    "  z=mid$(m[y+dy],x+dx,1)" "\0"
    "  if z==\" \" then ? pts;:!" "\0"
    "  if z==\"#\" then ? wls;:!" "\0"
    "  if z==\"$\" then ? exs;:!" "\0"
    "  print \".\";" "\0"
    "endsub" "\0"

    "\x1f"
};

__at(FLASH_START_ADDRESS + (16 * FLASH_WRITABLE_BYTES))
const uint8_t FREE[FLASH_SIZE_SECTORS - 16][FLASH_ERASABLE_BYTES] =
        { [0 ... ((FLASH_SIZE_SECTORS - 16) - 1)][0 ... ((FLASH_ERASABLE_BYTES) - 1)] = 0xFF };

#endif
