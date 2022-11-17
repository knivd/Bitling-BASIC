#if 0
/*

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
END IF  or  ENDIF

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
DATA    expression  { ,  expression ... }
READ    variable  { ,  variable ... }
REWIND                      ' return to the first DATA element
POP                         ' remove one return address from the stack
POKE    <addr, val
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
TDTDT   ()                  ' text display width
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

*/
#endif

#ifndef BASIC_H
#define BASIC_H

#include "common.h"

#define MAX_DIMENSIONS  5   // maximum number of dimensions in arrays
#define MAX_TERMS       25  // maximum terms in an expression
#define MAX_IFS         10  // maximum nested IFs
#define MAX_LOOPS       10  // maximum nested loops FOR/WHILE/REPEAT
#define MAX_CALLS       10  // maximum sub call nesting
#define MAX_PARAMETERS  10  // maximum sub parameters
#define MAX_FILES       3   // maximum number of open files

#define FPN_ACCURACY    0.000001

typedef uint16_t dim_t;     // specifies the size of dimension holders

// tokens
typedef enum {

    T_UNKNOWN = 0, T_ETX, T_EOL, T_NUMBER, T_STRING, T_IDENTIFIER, T_SUBID, T_LABELID,
    T_TYPE_INT, T_TYPE_REAL, T_TYPE_STRING,             // data types
    T_OP_PAREN, T_CL_PAREN, T_OP_BRACKET, T_CL_BRACKET, //  (  )  [  ]
    T_COMMA, T_SEMICOLON, T_COLON, T_HASH, T_AS, T_AT,  //  ,  ;  :  #  AS  @

    T_OPERATOR_,    // start of operators; must be before commands and functions
    // the operators below are sorted in rows by increasing priority
    T_ATTRIBUTION,                          //  =
    T_AND, T_OR, T_XOR,                     //  AND  OR  XOR
    T_NOT_EQUAL, T_EQUAL, T_GREATER_OR_EQUAL, T_SMALLER_OR_EQUAL, T_GREATER, T_SMALLER, //  <>  ==  >=  <=  >  <
    T_POWER,                                //  ^ (power)
    T_BITWISE_SHIFTR, T_BITWISE_SHIFTL,     //  >>  <<
    T_PLUS, T_MINUS,                        //  +  -
    T_MULTIPLICATION, T_MODULO, T_DIVISION, //  *  //  /
    T_UNARY_P, T_UNARY_M,                   //  + (unary)  - (unary)
    T_NOT,                                  //  NOT

    T_FUNCTION_,    // start of functions (return value); must be after operators and before commands
    T_RND, T_ABS, T_ROUND, T_TRUNC, T_FRACT, T_LOGN, T_LOGD, T_EXP, T_SQR, T_CBR,
    T_SIN, T_ASIN, T_HSIN, T_COS, T_ACOS, T_HCOS, T_TAN, T_ATAN, T_HTAN, T_COTAN,
    T_FREE, T_PEEK, T_ASC, T_VAL, T_STR, T_LEN, T_LEFT, T_RIGHT, T_MID, T_INSTR, T_CHR,
    T_TDRAM, T_TDWIDTH, T_TDHEIGHT, T_TIMER, T_AIN, T_DIN,

    T_COMMAND_,     // start of commands (not return value); must be after functions
    T_VAR, T_LET, T_END, T_IF, T_THEN, T_ELSE, T_ENDIF, T_CONTINUE, T_EXIT, T_FOR,
    T_TO, T_STEP, T_NEXT, T_WHILE, T_WEND, T_REPEAT, T_UNTIL, T_RETURN, T_GOTO, T_LABEL,
    T_GOSUB, T_SUB, T_ENDSUB, T_PRINT, T_QUESTION, T_DATA, T_READ, T_REWIND, T_POP,
    T_POKE, T_RESET, T_SLEEP, T_INKEY, T_GET, T_INPUT, T_CLS, T_HLOC, T_VLOC, T_PUTCH,
    T_DEFCH, T_SELCHT, T_CURSOR, T_SCROLL, T_TDREFRESH, T_WAIT, T_CLEAR, T_OPEN, T_CLOSE,
    T_DOUT

} token_t;

// keyword structure
typedef struct {
	int8_t priority;    // priority level (higher number = greater priority)
    uint8_t params;     // required parameters (0: none or processed internally)
	token_t token;      // associated token code
    char *name;         // keyword as text
    uint8_t nlen;       // keyword length in number of characters; needed for optimising the execution speed
    uint16_t code;      // instruction pseudo-code (code 0 is invalid and must not be used!)
    void (*func)(void); //function handler; parameter is the operator's own token code
} keyword_t;

// structure of a variable
typedef struct _var_t {
    data_t data;        // data container
    char *parent;       // pointer to parent function (or NULL)
    char *name;         // pointer to variable name
    uint8_t nlen;       // length of the variable name
    dim_t dim[MAX_DIMENSIONS];  // array dimensions
    struct _var_t *ref;     // pointer to referenced variable (accessed by address)
    struct _var_t *next;    // pointer to the next variable
} var_t;

typedef struct _sub_t {
    char *entry;        // entry point in the source
    char *name;         // pointer to the sub name
    uint8_t nlen;       // length of the sub name
    uint8_t isLabel;    // flag indicating whether it is a SUB or LABEL
    struct _sub_t *next;    // pointer to next record
} sub_t;

typedef struct {
    int32_t hdr;        // (0) screen; (-1) COM1; (1...) (header_page+1) of a file
    int32_t pos;        // meaningful for files only; last position in the file
} file_t;

// error codes
typedef enum {
    E_OK = 0,
    E_BREAK,
    E_MEMORY,
    E_UNDEFINED,
    E_STRING,
    E_TOO_COMPLEX,
    E_SYNTAX,
    E_OP_PAREN,
    E_CL_PAREN,
    E_COMMA,
    E_TYPE,
    E_DIV_ZERO,
    E_PARAM,
    E_NUM_PARAMS,
    E_DUPLICATE,
    E_INDEX,
    E_NO_IF,
    E_NO_THEN,
    E_NO_ENDIF,
    E_NO_WHILE,
    E_NO_WEND,
    E_NO_REPEAT,
    E_NO_LOOP,
    E_NO_SUB,
    E_NO_ENDSUB,
    E_NO_FOR,
    E_NO_TO,
    E_NO_AT,
    E_NO_VAR,
    E_WRONG_ID,
    E_NESTING,
    E_UNKNOWN,
    E_NO_DATA,
    E_FILE,
    E_CHANNEL
} error_code_t;

// error structure
typedef struct {
    error_code_t e_code;
    const char *message;
} error_t;

// main interpreter structure
typedef struct {
    char *entry;                // stored entry point of the source during execution
    char *src;                  // global source pointer
    char *tk_src;               // source location of the last taken token; modified by getToken()
    char *id_src;               // source location of the last taken identifier; modified by getId()
    uint8_t id_len;             // length of the last taken identifier; modified by getId()
    token_t t_token;            // last taken token code
    void *s_token;              // last token structure
    struct {
        uint8_t no_str_consts   : 1;    // if set, memory allocation for constant strings is not performed
        uint8_t index_base      : 1;    // index base for arrays: 0 or 1
        uint8_t use_codes       : 1;    // flag to enable using pseudo-codes
        uint8_t check_break     : 1;    // enable check for Ctrl-C break
        uint8_t syntax_style    : 2;    // 0/1:all uppercase; 2:all lowercase; 3:lowercase with capital first
        uint8_t                 : 2;
    } flags;
    jmp_buf err_env;            // stores the environment return point in case of abnormal exit due to errors
    error_code_t error_code;    // exit error code
    int8_t acX;                 // data stack index
    data_t acc[MAX_TERMS];      // data stack for expression evaluations
    int8_t opX;                 // operations stack index
    keyword_t *ops[MAX_TERMS];  // operations stack for expression evaluations
    int8_t fuX;                 // functions stack index
    struct {                    // functions stack
        const keyword_t *k;     // pointer to the keyword structure
        uint8_t depth;          // bracket depth at the time of calling
        int8_t acx;             // data stack depth at the time of calling
        void *p;                // generic pointer (used when reading arrays or calling subs)
    } fun[MAX_TERMS];
    data_t Z, S, A, B;          // virtual data registers (S is reserved only for string operations)
    size_x str_const_len;       // length of the last string constant returned by getToken()
    var_t *vars;                // variables
    dim_t index[MAX_DIMENSIONS]; // temporary array index values
    uint8_t if_depth;           // nested 'if' depth
    struct {                    // loops stack
        enum { L_FOR, L_WHILE, L_REPEAT } type;
        char *src;              // source location to return to
        var_t *v;               // variable-counter
        data_t end_value;       // end value
        data_t step;            // step of change
    } loop[MAX_LOOPS];
    uint8_t lpX;                // loops stack index
    struct {
        char *cursub;           // currently executed sub (or NULL)
        char *retadr;           // return address
        var_t *var;             // assignment variable
        dim_t index[MAX_DIMENSIONS]; // index for assignment variable
        data_type_t rettype;    // return data type
    } cas[MAX_CALLS];
    uint8_t caX;                // call stack index
    sub_t *subs;                // list of subs in the program subs
    char *data_entry;           // first recorded DATA occurrence
    char *data_current;         // pointer to the current data element to be read
    const keyword_t *kindex[27]; // search acceleration table - 26 letters plus one entry for non-alpha elements
    char str_buf[25];           // number to string conversion buffer
    char str_buf2[25];          // number to string conversion buffer (for fractions)
    file_t file[MAX_FILES];     // files
} basic_t;

basic_t bas;    // main interpreter data structure

#define revert_getToken() { \
    if(bas.t_token == T_STRING) x_free((byte **) &bas.s_token); \
    bas.src = bas.tk_src;   \
}

error_code_t runBasic(char **source, char use_codes);

// internal functions
void skip(uint32_t forced);
void error(error_code_t err);
void releaseVars(char *parent);
void pushAcc(data_t *a);
void popAcc(data_t *a, data_type_t dt);
void fifoAcc(data_t *a, data_type_t dt);
data_t *peekAcc(void);
void promote(void);
void convert(data_t *a, data_type_t t);
void getToken(void);
token_t expression(data_t *a, int8_t base_aX);
void statement(void);
void getId(void);

#endif
