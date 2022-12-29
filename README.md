# ELLO-LC1
ELLO LC1 firmware with Bitling BASIC
Platform: PIC18F47Q83
```

Bitling BASIC interpreter

' comment until the end of the line


=== data types

INT     (also INTEGER for compatibility)
REAL    (also FLOAT for compatibility)
STRING


=== operators

+   -   *   /
//  or  MOD     ' modulo
^               ' power
<<              ' shift left
>>              ' shift right
=               ' (in statements) attribution
==  or  =       ' (in expressions) compare for equal
<>              ' compare for not equal
<   <=   =>   >
AND   OR   XOR


=== variables

VAR  <type>  <name>  { [dimension [, dimension...] ] }  { , <name> ... }
' DIM is alias to VAR for backward compatibility

examples:
var int a, b[3,3]
var real x, z[10,10,10]
var string s[5]     ' array of five zero-terminated strings
' () are also supported in variable indexes instead of [] for backward compatibility


=== control structures

IF  <expression>  THEN
{ ELSE }
END IF  or  ENDIF  or  !

FOR  <variable>  =  <expression>  TO  <expression>  { STEP <expression> }
NEXT  { <variable> }

WHILE  <expression>
END WHILE  or  WEND

REPEAT
UNTIL  <expression>

CONTINUE            ' valid for FOR, WHILE, and REPEAT structures
EXIT                ' valid for FOR, WHILE, and REPEAT structures

LABEL  <label_name>
GOTO   <label_name>

END
' STOP is also supported and act the same way as END


=== subroutines

SUB     <name> {  (  <type>  {  @  }  <variable>  { ,  <type>  ...  }  )  }
FUNC    <name> {  (  <type>  {  @  }  <variable>  { ,  <type>  ...  }  )  }  {  AS  <type>  }
RETURN  {  value  }
END SUB  or  ENDSUB
END FUNC  or  ENDFUNC
GOSUB   <sub_name(...)>      ' optional calling format for subs


examples:
sub test1
func average(real a, real b) as real
sub save(int x, int y, int @arr)


=== commands

PRINT   [ # fileN ]  { expression  { ,  expression ... } }
INPUT   [ # fileN ]  variable  { ,  variable ... }
GET     [ # fileN ]  variable  { ,  variable ... }
INKEY   [ # fileN ]  variable
OPEN    <str>   AS   # fileN
CLOSE   # fileN
DELETE  <filename>
RENAME  <old_fname>, <new_fname>
DATA    expression  { ,  expression ... }
READ    variable  { ,  variable ... }
REWIND                      ' return to the first DATA element
POP                         ' remove one return address from the stack
POKE    <addr, val>
RESET                       ' restart the system
SLEEP                       ' put the system to sleep
CLS
HLOC    <column>
VLOC    <row>
PUTCH   <ascii>
DEFCH   <chrcode>  <byte1>  <byte2>  <byte3>  <byte4>  <byte5>  <byte6>  <byte7>  <byte8>
SELCHT  < 1, 2, or 3 >      ' select character table (see DOGM204 datasheet)
CURSOR  < bit.0 blink, bit.1 enable >
SCROLL  < bit.0 up, bit.1 down, bit.2 left, bit.3 right >
TDREF                       ' refresh the text display (used after direct writes into TD RAM)
WAIT    <milliseconds>
CLEAR   [ var [ , ... ] ]   ' undefine all or specific variables
DOUT    (channel, bit)      ' supported channels are 0..7


=== functions

RND     (val)               ' parameter other than 0 first seeds the generator with the value
ABS     (val)
ROUND   (val)
TRUNC   (val)
FRACT   (val)
LOGN    (val)
LOGD    (val)
EXP     (val)
SQR     (val)               ' square root
CBR     (val)               ' cube root
SIN     (val)
ASIN    (val)
HSIN    (val)
COS     (val)
ACOS    (val)
HCOS    (val)
TAN     (val)
ATAN    (val)
HTAN    (val)
COTAN   (val)
FREE    ()                  ' return the largest free block in memory
TDRAM   ()                  ' return the text display RAM address
TDWDT   ()                  ' text display width
TDHGT   ()                  ' text display height
PEEK    (addr)              ' 0 and positive numbers: RAM address, negative numbers: ROM address-1
ASC     (str)               ' character (first in string) to ASCII
CHR$    (val)               ' ASCII to character (one-character string)
VAL     (str)               ' string to number
STR$    (val, d)            ' number to string; "d" specifies the number of digits after decimal point
LEN$    (str)
LEFT$   (str, count)
RIGHT$  (str, count)
MID$    (str, start, count)
INSTR   (substr, str, start)
TIMER   (value)             ' read system millisecond incremental counter; any value different than 0 first loads the counter
AIN     (channel)           ' supported channels are 0..7
DIN     (channel)           ' supported channels are 0..7
EXIST   <filename>          ' will return 1 if true or 0 if not

```
