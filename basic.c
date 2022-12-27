#include "basic.h"
#include "basiclib.h"
#include "editor.h"

// error messages
const error_t errors[] = {
    { E_OK,         "OK" },
    { E_BREAK,      "Break" },
    { E_MEMORY,     "Out of memory" },
    { E_UNDEFINED,  "Undefined error" },
    { E_STRING,     "String constant" },
    { E_TOO_COMPLEX,"Too complex" },
    { E_SYNTAX,     "Bad syntax" },
    { E_OP_PAREN,   "Missing \'(\'" },
    { E_CL_PAREN,   "Missing \')\'" },
    { E_COMMA,      "Missing \',\'" },
    { E_TYPE,       "Type promotion" },
    { E_DIV_ZERO,   "Division by 0" },
    { E_PARAM,      "Missing parameter" },
    { E_NUM_PARAMS, "Parameter count" },
    { E_DUPLICATE,  "Duplicated name" },
    { E_INDEX,      "Invalid index" },
    { E_NO_IF,      "Missing IF" },
    { E_NO_THEN,    "Missing THEN" },
    { E_NO_ENDIF,   "Missing ENDIF" },
    { E_NO_WHILE,   "Missing WHILE" },
    { E_NO_WEND,    "Missing WEND" },
    { E_NO_REPEAT,  "Missing REPEAT" },
    { E_NO_LOOP,    "Not in a loop" },
    { E_NO_SUB,     "Missing SUB" },
    { E_NO_ENDSUB,  "Missing ENDSUB" },
    { E_NO_FOR,     "Missing FOR" },
    { E_NO_TO,      "Missing TO" },
    { E_NO_AT,      "Missing @" },
    { E_NO_VAR,     "Missing variable" },
    { E_WRONG_ID,   "Wrong identifier" },
    { E_NESTING,    "Excessive nesting" },
    { E_UNKNOWN,    "Unknown id" },
    { E_NO_DATA,    "No more data" },
    { E_FILE,       "File #" },
    { E_CHANNEL,    "Invalid channel" },
    { E_WRITE,      "File write/append" },
    { 0, 0 }    // must be always last in this list
};

// language keywords
const keyword_t keywords[] = {

    // single byte pseudo-codes are using their own ASCII code
    // if a pseudo-code is 0x01 or 0x02, then it is the high byte of a two-byte code
    // code 0x03 is reserved for compiled commentaries (followed by one byte number of bytes to skip)

    // non-alphabetic elements (0x100)
	{ 6, 0, T_PLUS,             "+",        1, '+',    op_add },
    { 8, 0, T_UNARY_P,          "+",        1, '+',    op_unary_plus },     // unary '+'; MUST be immediately after 'add'
	{ 6, 0, T_MINUS,            "-",        1, '-',    op_sub },
    { 8, 0, T_UNARY_M,          "-",        1, '-',    op_unary_minus },    // unary '-'; MUST be immediately after 'sub'
	{ 7, 0, T_MULTIPLICATION,   "*",        1, '*',    op_mul },
    { 7, 0, T_MODULO,           "//",       2, 0x0101, op_mod },
	{ 7, 0, T_DIVISION,         "/",        1, '/',    op_div },
	{ 3, 0, T_EQUAL,            "==",       2, 0x0102, op_equal },
    { 1, 0, T_ATTRIBUTION,      "=",        1, '=',    op_equal },          //  MUST be immediately after '==' in this list
	{ 3, 0, T_NOT_EQUAL,        "<>",       2, 0x0103, op_not_equal },
	{ 3, 0, T_SMALLER_OR_EQUAL, "<=",       2, 0x0104, op_sm_equal },
    { 3, 0, T_GREATER_OR_EQUAL, ">=",       2, 0x0106, op_gr_equal },
    { 5, 0, T_BITWISE_SHIFTL,   "<<",       2, 0x0105, op_shiftl },
    { 5, 0, T_BITWISE_SHIFTR,   ">>",       2, 0x0107, op_shiftr },
    { 3, 0, T_SMALLER,          "<",        1, '<',    op_smaller },
    { 3, 0, T_GREATER,          ">",        1, '>',    op_greater },
    { 4, 0, T_POWER,            "^",        1, '^',    op_power },
    { 0, 0, T_OP_PAREN,         "(",        1, '(',    __ },
    { 0, 0, T_CL_PAREN,         ")",        1, ')',    __ },
    { 0, 0, T_OP_BRACKET,       "[",        1, '[',    __ },                // array index
    { 0, 0, T_CL_BRACKET,       "]",        1, ']',    __ },                // array index
    { 0, 0, T_COMMA,            ",",        1, ',',    __ },                // parameter separator
    { 0, 0, T_COLON,            ":",        1, ':',    __ },                // statement separator
    { 0, 0, T_SEMICOLON,        ";",        1, ';',    __ },                // print value separator
    { 0, 0, T_AT,               "@",        1, '@',    __ },                // pointer to variable
    { 0, 0, T_HASH,             "#",        1, '#',    __ },                // file handler

    // non-alphabetic aliases to commands
    { 0, 0, T_ENDIF,            "!",        1, '!',    cm_endif },          // ENDIF
    {-1, 0, T_PRINT,            "?",        1, '?',    __ },                // PRINT

    // A (0x160)
    { 2, 0, T_AND,              "AND",      3, 0x0160, op_and },
    { 0, 1, T_ABS,              "ABS",      3, 0x0161, fn_abs },
    { 0, 1, T_ASIN,             "ASIN",     4, 0x0162, fn_asin },
    { 0, 1, T_ACOS,             "ACOS",     4, 0x0163, fn_acos },
    { 0, 1, T_ATAN,             "ATAN",     4, 0x0164, fn_atan },
    { 0, 1, T_ASC,              "ASC",      3, 0x0165, fn_asc },
    { 0, 1, T_AIN,              "AIN",      3, 0x0167, fn_ain },
    { 0, 0, T_AS,               "AS",       2, 0x0166, __ },

    // B (0x170)

    // C (0x180)
    { 0, 1, T_CBR,              "CBR",      3, 0x0180, fn_cbr },
    { 0, 1, T_COS,              "COS",      3, 0x0181, fn_cos },
    { 0, 1, T_COTAN,            "COTAN",    5, 0x0182, fn_cotan },
    { 0, 0, T_CONTINUE,         "CONTINUE", 8, 0x0183, cm_continue },
    { 0, 1, T_CHR,              "CHR$",     4, 0x0184, fn_chr },
    { 0, 1, T_CURSOR,           "CURSOR",   6, 0x0185, cm_cursor },
    { 0, 0, T_CLS,              "CLS",      3, 0x0186, cm_cls },
    { 0, 0, T_CLEAR,            "CLEAR",    5, 0x0187, cm_clear },
    {-1, 0, T_CLOSE,            "CLOSE",    5, 0x0188, __ },

    // D (0x190)
    {-1, 0, T_VAR,              "DIM",      3, 0x0190, __ },        // alias to VAR for backward compatibility
    { 0, 0, T_DATA,             "DATA",     4, 0x0191, cm_data },
    { 0, 9, T_DEFCH,            "DEFCH",    5, 0x0192, cm_defch },
    { 0, 1, T_DIN,              "DIN",      3, 0x0193, fn_din },
    { 0, 2, T_DOUT,             "DOUT",     4, 0x0194, cm_dout },
    { 0, 1, T_DELETE,           "DELETE",   6, 0x0195, cm_delete },

    // E (0x1A0)
    { 0, 0, T_ELSE,             "ELSE",     4, 0x01A0, cm_else },   // code is followed by a two-byte offset to matching ENDIF
    { 0, 0, T_ENDIF,            "ENDIF",    5, 0x01A1, cm_endif },
    { 0, 0, T_ENDIF,            "END IF",   6, 0x01A2, cm_endif },
    { 0, 0, T_WEND,             "END WHILE",9, 0x01A3, cm_wend },
    { 0, 1, T_EXP,              "EXP",      3, 0x01A4, fn_exp },
    {-1, 0, T_EXIT,             "EXIT",     4, 0x01A5, __ },
    {-1, 0, T_ENDSUB,           "ENDSUB",   6, 0x01A6, __ },
    {-1, 0, T_ENDSUB,           "END SUB",  7, 0x01A7, __ },
    {-1, 0, T_ENDSUB,           "ENDFUNC",  7, 0x01A8, __ },        // alias to ENDSUB
    {-1, 0, T_ENDSUB,           "END FUNC", 8, 0x01A9, __ },        // alias to ENDSUB
    { 0, 1, T_EXIST,            "EXIST",    5, 0x01AA, fn_exist },
    { 0, 0, T_END,              "END",      3, 0x01AB, cm_end },

    // F (0x1B0)
    {-1, 0, T_FOR,              "FOR",      3, 0x01B0, __ },
    { 0, 0, T_TYPE_REAL,        "FLOAT",    5, 0x01B1, __ },        // alias to REAL
    { 0, 1, T_FRACT,            "FRACT",    5, 0x01B3, fn_fract },
    { 0, 0, T_SUB,              "FUNC",     4, 0x01B2, cm_sub },    // alias to SUB
    { 0, 0, T_FREE,             "FREE",     4, 0x01B4, fn_free },

    // G (0x1C0)
    { 0, 0, T_GOTO,             "GOTO",     4, 0x01C0, cm_goto },
    {-1, 0, T_GOSUB,            "GOSUB",    5, 0x01C1, cm_gosub },  // optional/obsolete; must be followed by a sub name
    {-1, 0, T_GET,              "GET",      3, 0x01C2, __ },

    // H (0x1D0)
    { 0, 1, T_HSIN,             "HSIN",     4, 0x01D0, fn_hsin },
    { 0, 1, T_HCOS,             "HCOS",     4, 0x01D1, fn_hcos },
    { 0, 1, T_HTAN,             "HTAN",     4, 0x01D2, fn_htan },
    { 0, 1, T_HLOC,             "HLOC",     4, 0x01D3, cm_hloc },

    // I (0x1E0)
    {-1, 0, T_IF,               "IF",       2, 0x01E0, __ },
    { 0, 0, T_TYPE_INT,         "INT",      3, 0x01E1, __ },
    { 0, 0, T_TYPE_INT,         "INTEGER",  7, 0x01E2, __ },        // alias to INT
    { 0, 3, T_INSTR,            "INSTR",    5, 0x01E3, fn_instr },
    {-1, 0, T_INKEY,            "INKEY",    5, 0x01E4, __ },
    {-1, 0, T_INPUT,            "INPUT",    5, 0x01E5, __ },

    // J (0x1F0)

    // K (0x200)

    // L (0x210)
    { 0, 1, T_LOGN,             "LOGN",     4, 0x0210, fn_logn },
    { 0, 1, T_LOGD,             "LOGD",     4, 0x0211, fn_logd },   // alias to LOGD for backward compatibility
    { 0, 1, T_LOGD,             "LOG",      3, 0x0212, fn_logd },
    {-1, 0, T_LET,              "LET",      3, 0x0213, cm_let },    // optional/obsolete; must be followed by an identifier
    { 0, 1, T_LEN,              "LEN$",     4, 0x0214, fn_len },
    { 0, 2, T_LEFT,             "LEFT$",    5, 0x0215, fn_left },
    { 0, 0, T_LABEL,            "LABEL",    5, 0x0216, cm_label },

    // M (0x220)
    { 7, 0, T_MODULO,           "MOD",      3, 0x0220, op_mod },    // same as '//'
    { 0, 3, T_MID,              "MID$",     4, 0x0221, fn_mid },

    // N (0x230)
    { 8, 0, T_NOT,              "NOT",      3, 0x0230, op_not },
    { 0, 0, T_NEXT,             "NEXT",     4, 0x0231, cm_next },

    // O (0x240)
    { 2, 0, T_OR,               "OR",       2, 0x0240, op_or },
    {-1, 0, T_OPEN,             "OPEN",     4, 0x0241, __ },

    // P (0x250)
    { 0, 2, T_POKE,             "POKE",     4, 0x0250, cm_poke },
    { 0, 1, T_PEEK,             "PEEK",     4, 0x0251, fn_peek },
    { 0, 1, T_PUTCH,            "PUTCH",    5, 0x0252, cm_putch },
    {-1, 0, T_PRINT,            "PRINT",    5, 0x0253, __ },
    { 0, 0, T_POP,              "POP",      3, 0x0254, cm_pop },

    // Q (0x260)

    // R (0x270)
    { 0, 0, T_REPEAT,           "REPEAT",   6, 0x0270, cm_repeat },
    { 0, 0, T_TYPE_REAL,        "REAL",     4, 0x0271, __ },
    { 0, 1, T_ROUND,            "ROUND",    5, 0x0272, fn_round },
    { 0, 1, T_RND,              "RND",      3, 0x0273, fn_rnd },
    {-1, 0, T_RETURN,           "RETURN",   6, 0x0274, __ },
    { 0, 2, T_RIGHT,            "RIGHT$",   6, 0x0275, fn_right },
    {-1, 0, T_READ,             "READ",     4, 0x0276, __ },
    { 0, 0, T_REWIND,           "REWIND",   6, 0x0277, cm_rewind },
    { 0, 2, T_RENAME,           "RENAME",   6, 0x0278, cm_rename },
    { 0, 0, T_RESET,            "RESET",    5, 0x0279, cm_reset },

    // S (0x280)
    { 0, 1, T_SQR,              "SQR",      3, 0x0280, fn_sqr },
    { 0, 1, T_SIN,              "SIN",      3, 0x0281, fn_sin },
    { 0, 0, T_STEP,             "STEP",     4, 0x0282, __ },
    { 0, 0, T_SUB,              "SUB",      3, 0x0283, cm_sub },
    { 0, 0, T_TYPE_STRING,      "STRING",   6, 0x0284, __ },
    { 0, 2, T_STR,              "STR$",     4, 0x0285, fn_str },
    { 0, 1, T_SCROLL,           "SCROLL",   6, 0x0286, cm_scroll },
    { 0, 1, T_SELCHT,           "SELCHT",   6, 0x0287, cm_selcht },
    { 0, 0, T_END,              "STOP",     4, 0x0288, cm_end },    // alias to END
    { 0, 0, T_SLEEP,            "SLEEP",    5, 0x0289, cm_sleep },

    // T (0x290)
    { 0, 0, T_THEN,             "THEN",     4, 0x0290, __ },        // code is followed by a two-byte offset to matching ELSE or ENDIF
    { 0, 0, T_TO,               "TO",       2, 0x0291, __ },
    { 0, 1, T_TAN,              "TAN",      3, 0x0292, fn_tan },
    { 0, 1, T_TRUNC,            "TRUNC",    5, 0x0293, fn_trunc },
    { 0, 1, T_TIMER,            "TIMER",    5, 0x0294, fn_timer },
    { 0, 0, T_TDREFRESH,        "TDREF",    5, 0x0295, cm_tdref },
    { 0, 0, T_TDWIDTH,          "TDWDT",    5, 0x0296, fn_tdwdt },
    { 0, 0, T_TDHEIGHT,         "TDHGT",    5, 0x0297, fn_tdhgt },
    { 0, 0, T_TDRAM,            "TDRAM",    5, 0x0298, fn_tdram },

    // U (0x2A0)
    {-1, 0, T_UNTIL,            "UNTIL",    5, 0x02A0, __ },

    // V (0x2B0)
    {-1, 0, T_VAR,              "VAR",      3, 0x02B0, __ },
    { 0, 1, T_VAL,              "VAL",      3, 0x02B1, fn_val },
    { 0, 1, T_VLOC,             "VLOC",     4, 0x02B2, cm_vloc },

    // W (0x2C0)
    {-1, 0, T_WHILE,            "WHILE",    5, 0x02C0, __ },
    { 0, 0, T_WEND,             "WEND",     4, 0x02C1, cm_wend },
    { 0, 1, T_WAIT,             "WAIT",     4, 0x02C2, cm_wait },

    // X (0x2D0)
    { 2, 0, T_XOR,              "XOR",      3, 0x02D0, op_xor },

    // Y (0x2E0)

    // Z (0x2F0)

    // must be always last in this list
    { 0, 0, 0, 0, 0, 0, 0 }
};

const keyword_t k_OP_PAREN = { 0, 0, T_OP_PAREN, "(", 1, 0x0000, __ };
const keyword_t k_IDENTIFIER = { -1, 0, T_IDENTIFIER, "", 0, 0x0000, __ };
const keyword_t k_SUBID = { -1, 0, T_SUBID, "", 0, 0x0000, __ };
char *nulstr = "";

void getFromArray(var_t *v);


// release variables for given parent sub
void releaseVars(char *parent) {
    var_t *vn, *vp = NULL, *v = bas.vars;
    while(v) {  // release variables
        vn = v->next;
        if(parent == v->parent || parent == NULL) {
            if(v->ref == NULL) {    // referenced variables are only pointer to another variable
                if(v->data.type == TYPE_STR) {
                    if(v->dim[0]) { // check if the variable is an array of strings
                        uint32_t t, total = 1;
                        for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++) total *= v->dim[t];
                        for(t = 0; t < total; t++) x_free((byte **) ((byte *) v->data.p + t * sizeof(PTR_T)));
                    }
                    else x_free((byte **) &v->data.s);      // single string
                }
                if(v->dim[0]) x_free((byte **) &v->data.p); // release arrays
            }
            x_free((byte **) &v);
            if(vp) vp->next = vn;   // remove this variable from the chain
        }
        else vp = v;    // store the previous not freed variable
        v = vn;
    }
}


// print an error message and exit
void error(error_code_t err) {
    if(bas.error_code != E_OK) return;
    bas.error_code = err;
    for( ; bas.acX >= 0; bas.acX--) {
        if(bas.acc[bas.acX].type == TYPE_STR) x_free((byte **) &bas.acc[bas.acX].s);
    }
    if(bas.A.type == TYPE_STR) x_free((byte **) &bas.A.s);
    if(bas.B.type == TYPE_STR) x_free((byte **) &bas.B.s);
    if(bas.Z.type == TYPE_STR) x_free((byte **) &bas.Z.s);
    if(bas.S.type == TYPE_STR) x_free((byte **) &bas.S.s);
    releaseVars(NULL);  // release all variables
    while(bas.subs) {   // release all subs
        sub_t *sn = bas.subs->next;
        x_free((byte **) &bas.subs);
        bas.subs = sn;
    }
    const error_t *e = errors;
    while(e->message && e->e_code != err) e++;
    if(e->message) {    // found the code
        uint8_t t, r, y = posY;
        if(posX) printf("\r\n");
        for(y = r = posY; r < LCD_HEIGHT; r++)  // clear to the end of the screen
            for(t = 0; t < LCD_WIDTH; t++) {
                lcdSetPos(t, r);
                lcdDrawChar(' ');
            }
        lcdSetPos(0, y);
        if(err > E_BREAK) printf("E: ");
        printf("%s", e->message);
        while(toupper(getchar()) == 'C');   // wait for a key pressed to return to the editor (ignore Ctrl-C here)
    }
    longjmp(bas.err_env, (int) bas.error_code);
}


void pushAcc(data_t *a) {
    if(bas.acX >= MAX_TERMS) error(E_TOO_COMPLEX);
    memcpy(&bas.acc[bas.acX++], a, sizeof(data_t));
    a->s = NULL;
}


void popAcc(data_t *a, data_type_t dt) {
    if(bas.acX == 0) error(E_PARAM);
    memcpy(a, &bas.acc[--bas.acX], sizeof(data_t));
    if(dt != TYPE_NONE) convert(a, dt);
}


void fifoAcc(data_t *a, data_type_t dt) {
    if(bas.acX == 0) error(E_PARAM);
    memcpy(a, &bas.acc[0], sizeof(data_t));
    if(dt != TYPE_NONE) convert(a, dt);
    bas.acX--;
    uint8_t t;
    for(t = 0; t < bas.acX; t++) memcpy(&bas.acc[t], &bas.acc[t + 1], sizeof(data_t));
}


data_t *peekAcc(void) {
    if(bas.acX == 0) error(E_PARAM);
    return &bas.acc[bas.acX - 1];
}


// promote A to B or B to A in order to make them of equal type
void promote(void) {
    if(bas.A.type != bas.B.type) {
        if(bas.A.type == TYPE_STR || bas.B.type == TYPE_STR) error(E_TYPE);
        if(bas.A.type == TYPE_FPN) {   // A is real, B is int to be promoted to real
            bas.B.f = (FPN_T) bas.B.i;
            bas.B.type = TYPE_FPN;
        }
        else {                          // B is real, A is int to be promoted to real
            bas.A.f = (FPN_T) bas.A.i;
            bas.A.type = TYPE_FPN;
        }
    }
}


// convert data container to specified type
void convert(data_t *a, data_type_t t) {
    if(a->type != t) {
        if(a->type != TYPE_STR) {
            if(t == TYPE_FPN) a->f = (FPN_T) a->i;
            else if(t == TYPE_INT) a->i = (INT_T) a->f;
            else error(E_TYPE); // trying to convert int/real into string
            a->type = t;
        }
        else error(E_TYPE);     // trying to convert string into int/real
    }
}


// skip whitespaces
void skip(uint32_t forced) {
    while(forced-- && *bas.src != ETX) bas.src++;
    new_skip:;
    while(*bas.src <= ' ' && *bas.src > 0x03 && *bas.src != '\n' && *bas.src != ETX) bas.src++; // not skipping 0x01 .. 0x03
    if(*bas.src == 0x03) {  // compiled commentaries
        bas.src += (2 + (uint8_t) *(bas.src + 1));
        goto new_skip;
    }
    else if(*bas.src == '\'') bas.src += strlen(bas.src);   // skip source commentaries
}


// get identifier name
// (*bas.id_src) and (id_len) contain the result on exit
void getId(void) {
    skip(0);
    for(bas.id_len = 0, bas.id_src = bas.src; isidchr1C(*bas.src); bas.id_len++, bas.src++);
    for( ; bas.id_len && isidchrC(*bas.src); bas.id_len++, bas.src++);
    if(bas.id_len == 0) error(E_WRONG_ID);
}


// find a keyword pointed by (s)
const keyword_t *findIt(char *s) {
    char c = (char) toupper(*s);
    uint8_t af = isalpha(c);
    const keyword_t *k = (af ? bas.kindex[c - 'A' + 1] : bas.kindex[0]);
    for( ; k->code && ((af && *k->name == c) || !isalpha(*k->name)); k++) {
        if(!istrncmp(s, k->name, k->nlen)) {
            if(isidchr1C(*(s + k->nlen - 1))) {
                if(!isidchr1C(*(s + k->nlen))) return k;
            }
            else return k;
        }
    }
    return NULL;
}


// get the next token from the source
// (t_token) and (*s_token) are updated on exit
void getToken(void) {
    bas.s_token = NULL;
    skip(0);    // commentaries are handled here
    bas.tk_src = bas.src;
    if(*bas.src == 0 || *bas.src == '\n') { bas.t_token = T_EOL; bas.src++; return; }   // end of line token
    if(*bas.src == ETX) { bas.t_token = T_ETX; return; }    // end of text token

    // check if it is a number and if so get it in bas.acc[bas.acX]
    if(!getNumber(&bas.src, &bas.acc[bas.acX])) {   // getNumber() also sets the type
        bas.s_token = (void *) &bas.acc[bas.acX];
        bas.t_token = T_NUMBER;
        return;
    }

    // get string constants
    if(*bas.src == '\"') {
        bas.s_token = (void *) ++bas.src;           // record the start of the string
        for(bas.str_const_len = 0; *bas.src && *bas.src != ETX && *bas.src != '\"'; bas.str_const_len++, bas.src++);
        if(*(bas.src++) != '\"') error(E_STRING);   // the string is not properly terminated
        bas.t_token = T_STRING;
        return;
    }

    // search in the keywords
    const keyword_t *k;
    if(bas.flags.use_codes && *bas.src < 0x03) {    // scan in pre-processed pseudo-codes
        uint16_t code = (uint16_t) *bas.src;
        if(code < 0x03) code = (code << 8) + *(bas.src + 1);    // two-byte code
        if(code >= 0x160) k = bas.kindex[((code - 0x160) >> 4) + 1];
        else k = bas.kindex[0];
        for( ; k->code && k->code != code; k++);    // search this code
    }
    else k = findIt(bas.src);   // normal scanning of text source
    if(k->code) {   // found it in the keywords
        bas.src += k->nlen;
        bas.s_token = (void *) k;
        bas.t_token = k->token;
        return;
    }

    // search in the subs and variables
    if(isidchr1C(*bas.src)) {
        var_t *v = bas.vars;
        while(v) {
            if(!istrncmp(bas.src, v->name, v->nlen) && !isidchrC(*(bas.src + v->nlen)) &&
                (v->parent == NULL || (bas.caX && v->parent == bas.cas[bas.caX - 1].cursub))) break;
            v = v->next;
        }
        if(v) { // found it in the variables
            bas.src += v->nlen;
            if(v->ref != NULL) v = v->ref;
            bas.s_token = (void *) v;
            bas.t_token = T_IDENTIFIER;
            return;
        }
        sub_t *s = bas.subs;
        while(s) {
            if(!istrncmp(bas.src, s->name, s->nlen) && !isidchrC(*(bas.src + s->nlen))) break;
            s = s->next;
        }
        if(s) { // found it in the subs
            bas.src += s->nlen;
            bas.s_token = (void *) s;
            bas.t_token = (s->isLabel ? T_LABELID : T_SUBID);
            return;
        }
    }

    bas.t_token = T_UNKNOWN;
}


// pop indexes from the data stack, get data from an array and push it into the data stack
void getFromArray(var_t *v) {
    uint8_t t, r;
    for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++);   // count the number of dimensions into (t)
    while(t--) {    // pop the indexes from the stack in reverse order
        popAcc(&bas.A, TYPE_INT);
        if(bas.A.i < bas.flags.index_base || bas.A.i >= (v->dim[t] + bas.flags.index_base)) error(E_INDEX);
        bas.index[t] = (dim_t) (bas.A.i - bas.flags.index_base);
    }
    uint32_t x = 0; // calculated linear index
    for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++) {  // calculate the linear index in the array
        uint32_t xt = 1;
        for(r = t + 1; v->dim[r] && r < MAX_DIMENSIONS; r++) xt *= v->dim[r];
        x += (bas.index[t] * xt);
    }
    if(v->data.type == TYPE_INT)
        memcpy(&bas.A.i, ((byte *) v->data.p + x * sizeof(INT_T)), sizeof(INT_T));
    else if(v->data.type == TYPE_STR) {
        byte **ps = (byte **) ((byte *) v->data.p + x * sizeof(STR_T));
        size_t sl = strlen((char *) *ps) + 1;
        if(x_malloc((byte **) &bas.S.s, (size_x) sl) == NULL) error(E_MEMORY);
        memmove(bas.S.s, *ps, sl);
        pushAcc(&bas.S);
        return;
    }
    else memcpy(&bas.A.f, ((byte *) v->data.p + x * sizeof(FPN_T)), sizeof(FPN_T));
    bas.A.type = v->data.type;
    pushAcc(&bas.A);
}


// get array variable index into (bas.index[])
void getIndex(var_t *v) {
    getToken();
    if(bas.t_token != T_OP_BRACKET && bas.t_token != T_OP_PAREN) error(E_INDEX);
    uint8_t t;
    for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++) {
        expression(&bas.Z, 0);
        if(bas.Z.type != TYPE_INT) error(E_INDEX);
        if(bas.Z.i < bas.flags.index_base || bas.Z.i >= (v->dim[t] + bas.flags.index_base)) error(E_INDEX);
        if((t + 1) < MAX_DIMENSIONS && v->dim[t + 1] && bas.t_token != T_COMMA) error(E_INDEX);
        bas.index[t] = (dim_t) (bas.Z.i - bas.flags.index_base);
    }
    if(bas.t_token != T_CL_BRACKET && bas.t_token != T_CL_PAREN) error(E_INDEX);
}


// call a user sub currently pointed by *(bas.src)
void callSub(sub_t *sp) {
    if(bas.caX >= MAX_CALLS) error(E_NESTING);
    bas.cas[bas.caX].cursub = sp->entry;
    char *sub_src = bas.cas[bas.caX].cursub;        // sub definition
    char *par_src = bas.src;                        // source parameters
    bas.src = sub_src;                              // ======= switch to the sub side
    getToken();
    if(bas.t_token == T_OP_PAREN) { // an opening bracket is found - expect parameters
        sub_src = bas.src; bas.src = par_src;       // ======= switch to the parameters side
        getToken();
        if(bas.t_token != T_OP_PAREN) error(E_OP_PAREN);
        par_src = bas.src; bas.src = sub_src;       // ======= switch to the sub side
        while(bas.t_token != T_CL_PAREN) {          // declare the parameters as local variables
            getToken();
            if(bas.t_token == T_CL_PAREN) break;
            if(bas.t_token == T_ETX) error(E_SYNTAX);
            if(bas.t_token != T_TYPE_INT && bas.t_token != T_TYPE_REAL && bas.t_token != T_TYPE_STRING) error(E_SYNTAX);
            if(bas.t_token == T_TYPE_INT) bas.Z.type = TYPE_INT;
            else if(bas.t_token == T_TYPE_STRING) bas.Z.type = TYPE_STR;
            else bas.Z.type = TYPE_FPN;
            getToken(); // expecting T_UNKNOWN here (*src will not be moved in such case)
            if(bas.t_token == T_IDENTIFIER) error(E_DUPLICATE);
            if(bas.t_token != T_UNKNOWN && bas.t_token != T_AT) error(E_SYNTAX);
            uint8_t ptrf = (bas.t_token == T_AT);
            getId();    // name of the local variable
            var_t *v = bas.vars, *newv = NULL;
            while(v && v->next) v = v->next;        // go to the last variable in the list
            if(x_malloc((byte **) &newv, (size_x) sizeof(var_t)) == NULL) error(E_MEMORY); // memory for the new variable
            x_setname((byte *) newv, bas.id_src);
            newv->data.type = bas.Z.type;
            newv->name = bas.id_src;
            newv->nlen = bas.id_len;
            newv->ref = NULL;
            newv->next = NULL;
            newv->parent = bas.cas[bas.caX].cursub;
            if(v) v->next = newv; else bas.vars = newv; // attach the new variable to the variables list
            sub_src = bas.src; bas.src = par_src;   // ======= switch to the parameters side
            token_t tt;
            if(ptrf) {  // variable accessed via address
                getToken();
                if(bas.t_token != T_AT) error(E_NO_AT);
                getToken();
                if(bas.t_token != T_IDENTIFIER) error(E_NO_VAR);
                var_t *va = (var_t *) bas.s_token;
                getToken();
                tt = bas.t_token;
                convert(&newv->data, va->data.type);
                newv->ref = va;
            }
            else {      // variable accessed via value
                expression(&bas.Z, 0);
                tt = bas.t_token;
                convert(&bas.Z, newv->data.type);
                memcpy(&newv->data, &bas.Z, sizeof(data_t));
            }
            par_src = bas.src; bas.src = sub_src;   // ======= switch to the sub side
            getToken();
            if(bas.t_token != T_COMMA && bas.t_token != T_CL_PAREN) error(E_CL_PAREN);
            sub_src = bas.src; bas.src = par_src;   // ======= switch to the parameters side
            if(bas.t_token != tt) {                 // the sub side takes priority in what should be expected
                if(bas.t_token == T_CL_PAREN) error(E_CL_PAREN);
                else if(bas.t_token == T_COMMA) error(E_COMMA);
                else error(E_SYNTAX);
            }
            par_src = bas.src; bas.src = sub_src;   // ======= switch to the sub side
        }
    }
    else {  // no parameters are expected
        revert_getToken();     // revert the last getToken()
        sub_src = bas.src; bas.src = par_src;       // ======= switch to the parameters side
        getToken();
        if(bas.t_token == T_OP_PAREN) { // check for ()
            getToken();
            if(bas.t_token != T_CL_PAREN) error(E_CL_PAREN);
        }
        else revert_getToken(); // revert the last getToken()
        par_src = bas.src; bas.src = sub_src;       // ======= switch to the sub side
    }
    bas.cas[bas.caX].rettype = TYPE_NONE;
    getToken();
    if(bas.t_token == T_AS) {   // check if a result should be returned
        getToken();
        if(bas.t_token == T_ETX) error(E_SYNTAX);
        if(bas.t_token != T_TYPE_INT && bas.t_token != T_TYPE_REAL && bas.t_token != T_TYPE_STRING) error(E_SYNTAX);
        if(bas.t_token == T_TYPE_INT) bas.cas[bas.caX].rettype = TYPE_INT;
        else if(bas.t_token == T_TYPE_STRING) bas.cas[bas.caX].rettype = TYPE_STR;
        else bas.cas[bas.caX].rettype = TYPE_FPN;
    }
    bas.cas[bas.caX].var = NULL;
    bas.cas[bas.caX].retadr = par_src;  // store the point of return
    bas.caX++;
}


// calculate an expression and push return value into (*a)
// the final result is returned in (*a)
token_t expression(data_t *a, int8_t base_aX) {
    uint8_t depth = 0;  // bracket depth level
    token_t tk, tk_prev;
    bas.fuX = bas.opX = 0;
    bas.acX = base_aX;
    do {
        for(tk_prev = T_OP_PAREN, getToken(); ; tk_prev = tk, getToken()) {
            tk = bas.t_token;  // get the last taken token code
            if(tk > T_COMMAND_) break;

            if(tk > T_FUNCTION_) {
                if(bas.fuX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.fun[bas.fuX].acx = bas.acX;
                bas.fun[bas.fuX].depth = depth++;
                bas.fun[bas.fuX].k = (keyword_t *) bas.s_token;
                bas.fuX++;
                getToken();
                if(bas.t_token != T_OP_PAREN) error(E_OP_PAREN);
                if(bas.opX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.ops[bas.opX++] = (keyword_t *) bas.s_token;    // push the opening bracket
                continue;
            }

            if(tk > T_OPERATOR_) {
                keyword_t *k = (keyword_t *) bas.s_token;
                if(tk == T_ATTRIBUTION) {
                    tk = T_EQUAL;
                    k--;    // '=' becomes '==' when used in expressions
                }
                else if(tk == T_PLUS && (tk_prev > T_OPERATOR_ || tk_prev == T_OP_PAREN)) {
                    tk = T_UNARY_P;
                    k++;
                }
                else if(tk == T_MINUS && (tk_prev > T_OPERATOR_ || tk_prev == T_OP_PAREN)) {
                    tk = T_UNARY_M;
                    k++;
                }
                while(bas.opX && bas.ops[bas.opX - 1]->priority >= k->priority) (bas.ops[--bas.opX]->func)();
                if(bas.opX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.ops[bas.opX++] = k; // push the new operator
                continue;
            }

            if(tk == T_NUMBER) {    // constants are pushed straight into the data stack
                if(++bas.acX >= MAX_TERMS) error(E_TOO_COMPLEX);
                continue;
            }

            if(tk == T_STRING) {    // constants are pushed straight into the data stack
                if(bas.flags.no_str_consts == 0) {
                    const char *s = (const char *) bas.s_token;
                    if(x_malloc((byte **) &bas.S.s, (size_x) (bas.str_const_len + 1)) == NULL) error(E_MEMORY);    // HERE!
                    memcpy(bas.S.s, s, bas.str_const_len);
                    *(bas.S.s + bas.str_const_len) = 0; // ensure proper string termination
                    bas.acc[bas.acX].s = bas.S.s;
                    bas.acc[bas.acX].type = TYPE_STR;
                    bas.s_token = (void *) &bas.acc[bas.acX];
                    bas.S.s = NULL;
                }
                if(++bas.acX >= MAX_TERMS) error(E_TOO_COMPLEX);
                continue;
            }

            if(tk == T_IDENTIFIER) {
                if(bas.acX >= MAX_TERMS) error(E_TOO_COMPLEX);
                var_t *v = (var_t *) bas.s_token;
                if(v->dim[0]) { // the variable is an array so this is the toughest case to avoid recursion
                    if(bas.fuX >= MAX_TERMS) error(E_TOO_COMPLEX);
                    bas.fun[bas.fuX].acx = bas.acX;     // treating the case as call to a special function
                    bas.fun[bas.fuX].depth = depth++;
                    bas.fun[bas.fuX].k = &k_IDENTIFIER; // special "function" handle
                    bas.fun[bas.fuX].p = (void *) v;    // pass pointer to the core variable
                    bas.fuX++;
                    getToken();
                    if(bas.t_token != T_OP_BRACKET && bas.t_token != T_OP_PAREN) error(E_INDEX);
                    if(bas.opX >= MAX_TERMS) error(E_TOO_COMPLEX);
                    bas.ops[bas.opX++] = (void *) &k_OP_PAREN;  // push the opening bracket
                }
                else if(v->data.type == TYPE_STR) {     // get single string
                    size_t sl = strlen(v->data.s) + 1;
                    if(x_malloc((byte **) &bas.S.s, (size_x) sl) == NULL) error(E_MEMORY);
                    memmove(bas.S.s, v->data.s, sl);
                    pushAcc(&bas.S);
                }
                else {
                    memcpy(&bas.acc[bas.acX], &v->data, sizeof(data_t));    // get data from single variables
                    bas.acX++;
                }
                continue;
            }

            if(bas.t_token == T_SUBID) {    // call a sub with this name
                sub_t *s = (sub_t *) bas.s_token;
                if(bas.caX >= MAX_CALLS) error(E_NESTING);
                if(bas.fuX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.fun[bas.fuX].acx = bas.acX;     // treating the case as call to a special function
                bas.fun[bas.fuX].depth = depth++;
                bas.fun[bas.fuX].k = &k_SUBID;      // special "function" handle
                bas.fun[bas.fuX].p = (void *) s;    // pass pointer to the sub structure
                bas.fuX++;
                getToken();
                if(bas.t_token != T_OP_PAREN) error(E_INDEX);
                if(bas.opX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.ops[bas.opX++] = (void *) &k_OP_PAREN;  // push the opening bracket
                continue;
            }

            if(tk == T_OP_PAREN) {
                depth++;
                if(bas.opX >= MAX_TERMS) error(E_TOO_COMPLEX);
                bas.ops[bas.opX++] = (keyword_t *) bas.s_token; // push the opening bracket
                continue;
            }

            if(tk == T_CL_PAREN || tk == T_CL_BRACKET) {
                while(bas.opX && bas.ops[bas.opX - 1]->token != T_OP_PAREN) (bas.ops[--bas.opX]->func)();
                if(depth == 0 || bas.opX == 0 || bas.ops[bas.opX - 1]->token != T_OP_PAREN) break;  // the opening is outside
                bas.opX--;  depth--;    // pop the opening bracket as well
                if(bas.fuX && bas.fun[bas.fuX - 1].depth == depth) {
                    bas.fuX--;
                    if((bas.acX - bas.fun[bas.fuX].acx) < bas.fun[bas.fuX].k->params) error(E_NUM_PARAMS);
                    if(bas.fun[bas.fuX].k->token == T_IDENTIFIER) { // get data from an array
                        getFromArray((var_t *) bas.fun[bas.fuX].p);
                    }
                    else if(bas.fun[bas.fuX].k->token == T_SUBID) { // get data from user subs
                        // parameters are not in the stack (in reverse order) and return value must go back there
                        // TODO: not supported (for now) as it is very difficult to achieve without recursion!!
                        error(E_SYNTAX);
                    }
                    else {  // call function if this was part of its parameters
                        if(tk != T_CL_PAREN) error(E_CL_PAREN);
                        (bas.fun[bas.fuX].k->func)();
                    }
                }
                continue;
            }

            if(tk == T_COMMA) {
                if(bas.opX == 0) break;
                while(bas.opX && bas.ops[bas.opX - 1]->token != T_OP_PAREN) (bas.ops[--bas.opX]->func)();
                if(bas.opX) continue; else break;
            }

            break;      // something else (to be processed outside)
        }
        if(tk == T_COMMA && bas.opX == 0) break;
        while(bas.opX) {
            (bas.ops[--bas.opX]->func)();   // pop and execute all remaining operators
            if(bas.ops[bas.opX]->token == T_OP_PAREN) error(E_CL_PAREN);
        }
    } while(tk == T_COMMA);
    memcpy(a, &bas.acc[base_aX], sizeof(data_t));
    if(bas.t_token == T_ENDIF) {    // special case for the short ENDIF form
        revert_getToken();
        bas.t_token = T_EOL;
    }
    return tk;
}


// set variable (v) with index ix[] with value (d)
void setVar(var_t *v, dim_t *ix, data_t *d) {
    convert(d, v->data.type);
    if(v->dim[0]) {
        uint8_t t, r;
        uint32_t x = 0; // calculated linear index
        for(t = 0; t < MAX_DIMENSIONS && v->dim[t]; t++) {  // calculate the linear index in the array
            uint32_t xt = 1;
            for(r = t + 1; v->dim[r] && r < MAX_DIMENSIONS; r++) xt *= v->dim[r];
            x += (ix[t] * xt);
        }
        if(v->data.type == TYPE_INT) memcpy(((byte *) v->data.p + x * sizeof(INT_T)), &d->i, sizeof(INT_T));
        else if(v->data.type == TYPE_STR) {     // element in a string array
            if(d->s == NULL) d->s = nulstr;
            byte **ps = (byte **) ((byte *) v->data.p + x * sizeof(STR_T));
            x_free((byte **) *ps);
            *ps = (byte *) d->s; d->s = NULL;
        }
        else memcpy(((byte *) v->data.p + x * sizeof(FPN_T)), &d->f, sizeof(FPN_T));
    }
    else if(v->data.type == TYPE_STR) {
        if(d->s == NULL) d->s = nulstr;
        x_free((byte **) &v->data.s);
        v->data.s = d->s; d->s = NULL;
    }
    else memcpy(&v->data, d, sizeof(data_t));   // store data into single variables
}


// execute statements
void statement(void) {
    while(bas.t_token != T_ETX) {
        getToken();

        if(bas.t_token == T_EOL || bas.t_token == T_COLON) {
            if(bas.flags.check_break) {
                kbdGetScanCode(&kbd_flags, 0);
                if(kbd_flags & KBD_FLAG_CTRL) { // check for Ctrl-C to break the execution
                    uint8_t ch = kbdGet();
                    if(toupper(ch) == 'C') {
                        revert_getToken();
                        error(E_BREAK);
                    }
                }
            }
            continue;   // statement separators just continue with the next statement
        }

        if(bas.t_token == T_IDENTIFIER) {  // statements starting with an identifier are attributions
            var_t *v = (var_t *) bas.s_token;
            if(v->dim[0]) getIndex(v);      // the variable is an array so must get the index as well
            getToken();
            if(bas.t_token != T_ATTRIBUTION) error(E_SYNTAX);
            getToken();
            if(bas.t_token == T_SUBID) {    // so this is a call to function
                callSub((sub_t *) bas.s_token);
                bas.cas[bas.caX - 1].var = v;
                memcpy(&bas.cas[bas.caX - 1].index, &bas.index, sizeof(bas.index));
                continue;
            }
            revert_getToken();  // not a sub so revert the last getToken()
            expression(&bas.Z, 0);
            setVar(v, (dim_t *) bas.index, &bas.Z);
            continue;
        }

        if(bas.t_token == T_SUBID) {    // call a sub with this name
            callSub((sub_t *) bas.s_token);
            continue;
        }

        // note: getToken() non-keyword returned structures must be before this point

        // custom implemented keywords need to have priority -1
        if(((keyword_t *) bas.s_token)->priority == -1) {

            if(bas.t_token == T_IF) {
                expression(&bas.Z, 0);
                if(bas.if_depth++ >= MAX_IFS) error(E_TOO_COMPLEX);
                if(bas.t_token != T_THEN) getToken();   // check again - must be THEN
                if(bas.t_token != T_THEN) error(E_NO_THEN);
                if((bas.Z.type == TYPE_FPN && bas.Z.f == 0.0) ||
                        (bas.Z.type == TYPE_INT && bas.Z.i == 0) ||
                        (bas.Z.type == TYPE_STR && *bas.Z.s == 0)) {    // unsatisfied condition - search for ELSE or ENDIF
                    if(*bas.tk_src < 0x03) {    // pre-calculated offset jump
                        uint16_t offs = ((uint16_t) *(bas.tk_src + 2)) << 8;
                        offs += (uint8_t) *(bas.tk_src + 3);
                        bas.src = bas.tk_src + offs;
                    }
                    else {  // text form - not using pre-calculated offset jump
                        uint8_t extra_depth = 0;
                        bas.flags.no_str_consts = 1;
                        while((bas.t_token != T_ELSE && bas.t_token != T_ENDIF)
                                || extra_depth) {
                            if(bas.t_token == T_ETX) error(E_NO_ENDIF);
                            if(bas.t_token == T_IF) {
                                if(++extra_depth >= MAX_IFS) error(E_TOO_COMPLEX);
                            }
                            if(bas.t_token == T_ENDIF) {
                                if(extra_depth-- == 0) error(E_NO_IF);
                            }
                            getToken();
                        }
                        bas.flags.no_str_consts = 0;
                        if(bas.t_token == T_ENDIF) bas.if_depth--;  // case closed
                    }
                }
                continue;
            }

            if(bas.t_token == T_WHILE) {
                if(bas.lpX >= MAX_LOOPS) error(E_TOO_COMPLEX);
                bas.loop[bas.lpX].type = L_WHILE;
                bas.loop[bas.lpX].src = bas.tk_src;
                bas.lpX++;
                expression(&bas.Z, 0);
                if((bas.Z.type == TYPE_FPN && bas.Z.f == 0.0) ||
                        (bas.Z.type == TYPE_INT && bas.Z.i == 0) ||
                        (bas.Z.type == TYPE_STR && *bas.Z.s == 0)) {    // unsatisfied condition - jump after the WEND
                    getToken();
                    uint8_t extra_depth = 0;
                    bas.flags.no_str_consts = 1;
                    while(bas.t_token != T_WEND || extra_depth) {   // skip to the following WEND
                        if(bas.t_token == T_ETX) error(E_NO_WEND);
                        if(bas.t_token == T_WHILE) {
                            if(++extra_depth >= MAX_LOOPS) error(E_TOO_COMPLEX);
                        }
                        if(bas.t_token == T_WEND) {
                            if(extra_depth-- == 0) error(E_NO_WHILE);
                        }
                        getToken();
                    }
                    bas.flags.no_str_consts = 0;
                    bas.lpX--;
                }
                continue;
            }

            if(bas.t_token == T_UNTIL) {
                if(bas.lpX == 0 || bas.loop[bas.lpX - 1].type != L_REPEAT) error(E_NO_REPEAT);
                expression(&bas.Z, 0);
                if((bas.Z.type == TYPE_FPN && bas.Z.f == 0.0) ||
                        (bas.Z.type == TYPE_INT && bas.Z.i == 0) ||
                        (bas.Z.type == TYPE_STR && *bas.Z.s == 0)) {    // unsatisfied condition - return back to the REPEAT
                    bas.src = bas.loop[--bas.lpX].src;
                }
                continue;
            }

            if(bas.t_token == T_FOR) {
                if(bas.lpX >= MAX_LOOPS) error(E_TOO_COMPLEX);
                getToken();
                if(bas.t_token != T_IDENTIFIER) error(E_NO_VAR);    // this needs to be the variable-counter
                bas.loop[bas.lpX].v = (var_t *) bas.s_token;
                if(bas.loop[bas.lpX].v->data.type == TYPE_STR) error(E_TYPE);
                getToken();
                if(bas.t_token != T_ATTRIBUTION) error(E_SYNTAX);   // this needs to be '='
                expression(&bas.Z, 0);
                convert(&bas.Z, bas.loop[bas.lpX].v->data.type);
                memcpy(&bas.loop[bas.lpX].v->data, &bas.Z, sizeof(data_t));  // set the initial value
                if(bas.t_token != T_TO) getToken();
                if(bas.t_token != T_TO) error(E_NO_TO);             // this needs to be TO
                expression(&bas.Z, 0);
                convert(&bas.Z, bas.loop[bas.lpX].v->data.type);
                memcpy(&bas.loop[bas.lpX].end_value, &bas.Z, sizeof(data_t));    // set the end value
                if(bas.t_token != T_STEP) { // missing STEP - set automatic step 1
                    bas.loop[bas.lpX].step.i = 1;
                    bas.loop[bas.lpX].step.type = TYPE_INT;
                    convert(&bas.loop[bas.lpX].step, bas.loop[bas.lpX].v->data.type);
                }
                else {  // custom step provided
                    expression(&bas.Z, 0);
                    convert(&bas.Z, bas.loop[bas.lpX].v->data.type);
                    memcpy(&bas.loop[bas.lpX].step, &bas.Z, sizeof(data_t));     // set the step value
                }
                bas.loop[bas.lpX].type = L_FOR;
                bas.loop[bas.lpX].src = bas.tk_src;  // store return point in the source
                bas.lpX++;
                continue;
            }

            if(bas.t_token == T_VAR) {          // declaration of variables
                getToken();
                if(bas.t_token != T_TYPE_INT && bas.t_token != T_TYPE_REAL && bas.t_token != T_TYPE_STRING) error(E_SYNTAX);
                data_type_t dt = TYPE_FPN;      // default data type is 'real'
                if(bas.t_token == T_TYPE_INT) dt = TYPE_INT;
                else if(bas.t_token == T_TYPE_STRING) dt = TYPE_STR;
                while(1) {
                    if(bas.t_token != T_UNKNOWN) getToken();
                    if(bas.t_token == T_IDENTIFIER) error(E_DUPLICATE);
                    if(bas.t_token != T_UNKNOWN) error(E_SYNTAX);
                    getId();
                    var_t *v = bas.vars;
                    while(v && v->next) v = v->next;
                    var_t *newv = NULL;
                    if(x_malloc((byte **) &newv, (size_x) sizeof(var_t)) == NULL) error(E_MEMORY); // allocate for the new variable
                    x_setname((byte *) newv, bas.id_src);
                    newv->data.type = dt;
                    newv->name = bas.id_src;
                    newv->nlen = bas.id_len;
                    newv->parent = (bas.caX ? bas.cas[bas.caX - 1].cursub : NULL);
                    newv->ref = NULL;
                    if(v) v->next = newv; else bas.vars = newv; // attach the new variable to the variables list
                    getToken();
                    if(bas.t_token == T_OP_BRACKET || bas.t_token == T_OP_PAREN) {  // so it is an array
                        uint32_t t, total = 1;
                        for(t = 0; t < MAX_DIMENSIONS; t++) {
                            expression(&bas.Z, 0);
                            if(bas.Z.type != TYPE_INT || bas.Z.i <= 0) error(E_INDEX);
                            newv->dim[t] = (dim_t) bas.Z.i;
                            total *= (dim_t) bas.Z.i;
                            if(bas.t_token == T_COMMA) continue;
                            break;
                        }
                        if(bas.t_token != T_CL_BRACKET && bas.t_token != T_CL_PAREN) error(E_SYNTAX);
                        if(newv->data.type == TYPE_INT) total *= sizeof(INT_T);
                        else if(newv->data.type == TYPE_STR) total *= sizeof(STR_T);
                        else total *= sizeof(FPN_T);
                        newv->data.p = NULL;
                        if(x_malloc((byte **) &newv->data.p, (size_x) total) == NULL) error(E_MEMORY); // memory for the array
                        getToken();
                    }
                    else if(bas.t_token == T_ATTRIBUTION ) {    // attribution is mutually exclusive with arrays
                        expression(&bas.Z, 0);
                        memset(&bas.index, 0, sizeof(bas.index));
                        setVar(newv, (dim_t *) bas.index, &bas.Z);
                        if(bas.t_token == T_UNKNOWN) continue;  // this is likely the name of a new variable
                    }
                    if(bas.t_token == T_EOL || bas.t_token == T_COLON || bas.t_token == T_ETX) break;
                    if(bas.t_token == T_COMMA) continue;
                    break;
                }
                continue;
            }

            if(bas.t_token == T_ENDSUB || bas.t_token == T_RETURN) {
                if(bas.caX == 0 || bas.cas[bas.caX - 1].cursub == NULL) error(E_NO_SUB);
                if(bas.t_token == T_RETURN && bas.cas[bas.caX - 1].rettype != TYPE_NONE) {
                    expression(&bas.Z, 0);
                    convert(&bas.Z, bas.cas[bas.caX - 1].rettype);
                    if(bas.cas[bas.caX - 1].var) setVar(bas.cas[bas.caX - 1].var, (dim_t *) bas.cas[bas.caX - 1].index, &bas.Z);
                }
                bas.caX--;  // must not be earlier because variables are detected with [caX - 1]
                releaseVars(bas.cas[bas.caX].cursub);   // release local variables
                bas.src = bas.cas[bas.caX].retadr;      // return back to the caller
                continue;
            }

            if(bas.t_token == T_EXIT) {
                if(bas.lpX == 0) error(E_NO_LOOP);
                uint8_t extra_depth = 0;
                bas.flags.no_str_consts = 1;
                for( ; bas.t_token != T_ETX; ) {    // jump past the end of the current loop structure
                    getToken();
                    if(bas.t_token == T_ETX) error(E_SYNTAX);
                    if(bas.t_token == T_WHILE || bas.t_token == T_REPEAT || bas.t_token == T_FOR) {
                        if(++extra_depth >= MAX_LOOPS) error(E_TOO_COMPLEX);
                    }
                    if(bas.t_token == T_WEND || bas.t_token == T_UNTIL || bas.t_token == T_NEXT) {
                        if(extra_depth) extra_depth--;
                        if(extra_depth == 0) {
                            if(bas.loop[bas.lpX - 1].type == L_FOR && bas.t_token == T_NEXT) break;
                            if(bas.loop[bas.lpX - 1].type == L_WHILE && bas.t_token == T_WEND) break;
                            if(bas.loop[bas.lpX - 1].type == L_REPEAT && bas.t_token == T_UNTIL) {
                                expression(&bas.Z, 0);  // ignore the result but need to go through the expression
                                break;
                            }
                        }
                    }
                }
                bas.flags.no_str_consts = 0;
                bas.lpX--;
                continue;
            }

            if(bas.t_token == T_READ) {     // read from DATA statements
                for(;;) {
                    getToken();
                    if(bas.t_token != T_IDENTIFIER) error(E_UNKNOWN);   // expecting known variables here
                    var_t *v = (var_t *) bas.s_token;
                    if(v->dim[0]) getIndex(v);
                    if(bas.data_current == NULL || *bas.data_current == ETX) error(E_NO_DATA);
                    char *s = bas.src;
                    bas.src = bas.data_current;
                    expression(&bas.Z, 0);
                    setVar(v, (dim_t *) bas.index, &bas.Z);
                    if(bas.t_token != T_COMMA) {
                        if(bas.t_token != T_EOL && bas.t_token != T_COLON && bas.t_token != T_ETX) error(E_SYNTAX);
                        bas.data_current = NULL;
                        bas.flags.no_str_consts = 1;
                        while(bas.t_token != T_ETX) {   // jump to the next DATA element
                            getToken();
                            if(bas.t_token == T_ETX || bas.t_token == T_EOL || bas.t_token == T_NUMBER || bas.t_token == T_STRING) continue;
                            if(bas.t_token == T_IDENTIFIER || bas.t_token == T_SUBID || bas.t_token == T_LABELID) continue;
                            if(bas.t_token == T_UNKNOWN) { getId(); continue; }
                            if(bas.t_token == T_DATA) { bas.data_current = bas.src; break; }
                        }
                        bas.flags.no_str_consts = 0;
                    }
                    bas.data_current = bas.src;
                    bas.src = s;    // restore the original source pointer
                    getToken();
                    if(bas.t_token == T_COMMA) continue;    // move past the comma
                    revert_getToken();
                    break;
                }
                continue;
            }

            if(bas.t_token == T_GET || bas.t_token == T_INKEY) { // get single keys with and without waiting
                token_t k = bas.t_token;
                int32_t h = 0;
                getToken();
                if(bas.t_token == T_HASH) {
                    expression(&bas.Z, 0);
                    if(bas.Z.type != TYPE_INT || bas.Z.i < 1 || bas.Z.i > MAX_FILES || bas.t_token != T_COMMA) error(E_FILE);
                    h = bas.file[bas.Z.i - 1].hdr;
                }
                else revert_getToken();
                for(;;) {
                    getToken();
                    if(bas.t_token != T_IDENTIFIER) error(E_UNKNOWN);   // expecting known variables here
                    var_t *v = (var_t *) bas.s_token;
                    if(v->dim[0]) getIndex(v);
                    if(h == 0) {
                        bas.Z.i = ((k == T_GET) ? getchar() : kbdGet());
                        if(bas.flags.check_break) {
                            if((kbd_flags & KBD_FLAG_CTRL) && toupper((int) bas.Z.i) == 'C') {  // check for Ctrl-C
                                revert_getToken();
                                error(E_BREAK);
                            }
                        }
                        if(bas.Z.i == 0) bas.Z.i = -1;  // return -1 if no key pressed
                    }
                    else if(h > 0) {
                        bas.A.i = 0;
                        bas.B.i = flash_readFile((h - 1), &bas.file[bas.Z.i - 1].pos, 1, (uint8_t *) &bas.A.i);
                        bas.file[bas.Z.i - 1].pos++;
                        bas.Z.i = ((bas.B.i > 0) ? bas.A.i : -1);
                    }
                    else if(h == -1) {  // input from COM1
                        do {
                            if((kbd_flags & KBD_FLAG_CTRL) && toupper((int) bas.Z.i) == 'C') {  // check for Ctrl-C
                                revert_getToken();
                                error(E_BREAK);
                            }
                            bas.Z.i = _inbyte(5);
                        } while(k == T_GET && bas.Z.i == -1);
                    }
                    bas.Z.type = TYPE_INT;
                    if(v->data.type == TYPE_INT || v->data.type == TYPE_FPN) convert(&bas.Z, v->data.type);
                    else {  // the variable is of string type
                        bas.str_buf[0] = (uint8_t) bas.Z.i;
                        bas.str_buf[1] = 0;    // string termination
                        bas.Z.s = bas.str_buf;
                    }
                    bas.Z.type = v->data.type;
                    setVar(v, (dim_t *) bas.index, &bas.Z);
                    getToken();
                    if(bas.t_token == T_COMMA) continue;    // move past the comma
                    revert_getToken();
                    break;
                }
                continue;
            }

            if(bas.t_token == T_INPUT) {    // get input
                int32_t h = 0;
                getToken();
                if(bas.t_token == T_HASH) {
                    expression(&bas.Z, 0);
                    if(bas.Z.type != TYPE_INT || bas.Z.i < 1 || bas.Z.i > MAX_FILES || bas.t_token != T_COMMA) error(E_FILE);
                    h = bas.file[bas.Z.i - 1].hdr;
                }
                else revert_getToken();
                for(;;) {
                    getToken();
                    if(bas.t_token != T_IDENTIFIER) error(E_UNKNOWN);   // expecting known variables here
                    var_t *v = (var_t *) bas.s_token;
                    if(v->dim[0]) getIndex(v);
                    uint8_t ww = LCD_WIDTH - posX;  // string window width
                    if(ww < 4) { printf("\r\n"); ww = LCD_WIDTH; }  // don't allow too small string windows
                    uint8_t t, sc = 0, sw = 0;
                    int ch;
                    memset(line_buffer, 0, MAX_LINE_LEN + 1);
                    for(ch = 0; ; ch = 0) { // enter and edit string in (line_buffer[])
                        if(h == 0) {        // get input from the keyboard (default)
                            for(t = 0; t < ww; t++) {   // print the string
                                lcdSetPos(LCD_WIDTH - ww + t, posY);
                                if(sw + t < strlen(line_buffer)) lcdDrawChar(*(line_buffer + sw + t));
                                else lcdDrawChar(' ');
                            }
                            lcdSetPos(LCD_WIDTH - ww + sc, posY);
                            ch = (uint8_t) getchar();
                        }
                        else if(h > 0) {    // get input from file
                            bas.A.i = 0;
                            bas.B.i = flash_readFile((h - 1), &bas.file[bas.Z.i - 1].pos, 1, (uint8_t *) &bas.A.i);
                            bas.file[bas.Z.i - 1].pos++;
                            ch = ((bas.B.i > 0) ? (int) bas.A.i : '\r');    // fill finish at the end of the file
                        }
                        else if(h == -1) {  // get input from COM1
                            do {
                                if((kbd_flags & KBD_FLAG_CTRL) && toupper((int) bas.Z.i) == 'C') {  // check for Ctrl-C
                                    revert_getToken();
                                    error(E_BREAK);
                                }
                                ch = _inbyte(5);
                            } while(ch == -1);
                        }
                        if(bas.flags.check_break) {
                            if((kbd_flags & KBD_FLAG_CTRL) && toupper(ch) == 'C') { // check for Ctrl-C to break the execution
                                revert_getToken();
                                error(E_BREAK);
                            }
                        }
                        if(ch == '\r') break;   // repeat until Enter
                        if(ch == '\b') {        // backspace
                            if(*line_buffer) {
                                if(sw > 0) sw--;
                                else if(sc > 0) sc--;
                                *(line_buffer + strlen(line_buffer) - 1) = 0;
                            }
                        }
                        if(ch >= ' ') {     // printable characters
                            if(strlen(line_buffer) < MAX_LINE_LEN) {
                                if((sc + 1) < ww) sc++;
                                else if((sw + sc + 1) < MAX_LINE_LEN) sw++;
                                *(line_buffer + strlen(line_buffer) + 1) = 0;
                                *(line_buffer + strlen(line_buffer)) = (char) ch;
                            }
                        }
                    }
                    if(h == 0) printf("\r\n");
                    if(v->data.type == TYPE_FPN) {
                        bas.A.f = (FPN_T) strtod(line_buffer, NULL);
                        bas.A.type = TYPE_FPN;
                        setVar(v, (dim_t *) bas.index, &bas.A);
                    }
                    else if(v->data.type == TYPE_INT) {
                        bas.A.i = (INT_T) strtol(line_buffer, NULL, 10);
                        bas.A.type = TYPE_INT;
                        setVar(v, (dim_t *) bas.index, &bas.A);
                    }
                    else {  // string
                        if(x_malloc((byte **) &bas.S.s, (size_x) (strlen(line_buffer) + 1)) == NULL) error(E_MEMORY);
                        strcpy(bas.S.s, line_buffer);
                        *(bas.S.s + strlen(line_buffer)) = 0;
                        setVar(v, (dim_t *) bas.index, &bas.S);
                    }
                    getToken();
                    if(bas.t_token == T_COMMA) continue;    // move past the comma
                    revert_getToken();
                    break;
                }
                continue;
            }

            if(bas.t_token == T_PRINT) {    // print
                int32_t h = 0;
                getToken();
                if(bas.t_token == T_HASH) {
                    expression(&bas.Z, 0);
                    if(bas.Z.type != TYPE_INT || bas.Z.i < 1 || bas.Z.i > MAX_FILES || bas.t_token != T_COMMA) error(E_FILE);
                    h = bas.file[bas.Z.i - 1].hdr;
                }
                else revert_getToken();
                for(;;) {
                    expression(&bas.Z, 0);
                    if(bas.acX) {
                        popAcc(&bas.A, TYPE_NONE);
                        data_t *a = &bas.A;
                        if(h == 0) {        // output to the screen
                            if(a->type == TYPE_INT) printf("%li", (long) a->i);
                            else if(a->type == TYPE_FPN) printf("%1.6G", (double) a->f);
                            else if(a->s) {
                                printf("%s", a->s);
                                x_free((byte **) &a->s);
                            }
                        }
                        else if(h > 0) {    // add (a->s) to file with header page (h-1)
                            long l;
                            char *fn;
                            flash_getHeader((h - 1), &l, &fn);
                            if(flash_writeFile(fn, strlen(a->s) + 1, a->s, 'A') < 0) error(E_WRITE);
                        }
                        else if(h == -1) {  // output to COM1
                            memset(bas.str_buf, 0, sizeof(bas.str_buf));
                            char *p;
                            if(a->type == TYPE_INT) {
                                sprintf(bas.str_buf, "%li", (long) a->i);
                                for(p = (char *) &bas.str_buf; *p; p++) _outbyte(*p);
                            }
                            else if(a->type == TYPE_FPN) {
                                sprintf(bas.str_buf, "%1.6G", (double) a->f);
                                for(p = (char *) &bas.str_buf; *p; p++) _outbyte(*p);
                            }
                            else if(a->s) {
                                for(p = a->s; *p; p++) _outbyte(*p);
                                x_free((byte **) &a->s);
                            }
                        }
                    }
                    if(bas.t_token == T_COMMA || bas.t_token == T_SEMICOLON) {
                        if(bas.t_token == T_COMMA) {
                            if(h == 0) printf("\t");
                            else if(h > 0) {    // add "\t" to file with header page (h-1)
                                long l;
                                char *fn;
                                flash_getHeader((h - 1), &l, &fn);
                                char s[2] = "\t";
                                if(flash_writeFile(fn, 2, s, 'A') < 0) error(E_WRITE);
                            }
                            else if(h == -1) _outbyte('\t');
                        }
                        getToken(); // look ahead what is following
                        if(bas.t_token == T_EOL || bas.t_token == T_COLON || bas.t_token == T_ETX) break;
                        revert_getToken();
                        continue;
                    }
                    if(bas.t_token == T_EOL || bas.t_token == T_COLON || bas.t_token == T_ETX) {
                        if(h == 0) printf("\r\n");
                        else if(h > 0) {    // add "\r\n" to file with header page (h-1)
                            long l;
                            char *fn;
                            flash_getHeader((h - 1), &l, &fn);
                            char s[3] = "\r\n";
                            if(flash_writeFile(fn, 3, s, 'A') < 0) error(E_WRITE);
                        }
                        else if(h == -1) { _outbyte('\r'); _outbyte('\n'); }
                        break;
                    }
                    error(E_SYNTAX);
                }
                continue;
            }

        }

        if(bas.t_token == T_OPEN) {
            int32_t br, h = -1;
            expression(&bas.Z, 0);
            char *fn = bas.Z.s;
            if(bas.Z.type != TYPE_STR) error(E_TYPE);   // filename must be string
            if(!istrncmp(fn, "COM1:", 5)) { // COM port
                char *p = fn + 5;
                while(*p == ' ') p++;
                br = (int32_t) strtol(p, &p, 10);
                if((*p && *p != ' ') || br < 75 || br > 2000000) error(E_SYNTAX);
                h = -2; // will become -1 after the increment later
            }
            else {
                h = flash_findFile(fn);     // check if this file exists
                if(h == -1) h = flash_writeFile(fn, 0, NULL, 'W');  // create new file
                if(h == -1) error(E_WRITE); // unable to create new file
            }
            x_free((byte **) &fn);
            if(bas.t_token != T_AS) error(E_SYNTAX);    // must be followed by AS
            getToken();
            if(bas.t_token != T_HASH) error(E_FILE);
            expression(&bas.Z, 0);
            if(bas.Z.type != TYPE_INT || bas.Z.i < 1 || bas.Z.i > MAX_FILES) error(E_FILE);
            bas.Z.i--;
            h++;    // the reason for this is to distinguish between screen and header page 0
            if(h == -1) openXMPort((uint32_t) br);
            bas.file[bas.Z.i].hdr = h;
            bas.file[bas.Z.i].pos = 0;
            continue;
        }

        if(bas.t_token == T_CLOSE) {
            getToken();
            if(bas.t_token != T_HASH) error(E_FILE);
            expression(&bas.Z, 0);
            if(bas.Z.type != TYPE_INT || bas.Z.i < 1 || bas.Z.i > MAX_FILES) error(E_FILE);
            bas.Z.i--;
            if(bas.file[bas.Z.i].hdr == -1) closeXMPort();
            bas.file[bas.Z.i].hdr = 0;
            bas.file[bas.Z.i].pos = -1;
            continue;
        }

        // note: all custom implemented keywords (with priority -1) must be before this point

        if(bas.t_token > T_FUNCTION_) {     // commands and functions
            keyword_t *k = (keyword_t *) bas.s_token;
            if(k->params > 0) {     // defined number of parameters evaluate and fill the stack with parameters
                int8_t pc;
                for(pc = 0; pc < k->params; pc++) {
                    if(pc) {
                        if(bas.t_token != T_COMMA) error(E_PARAM);
                    }
                    expression(&bas.Z, pc);
                }
            }
            (k->func)();
            continue;
        }

        if(bas.t_token == T_ETX) break;     // end of text
        error(E_UNKNOWN);
    }
}


// compile commentaries and return 1 if a change was made
uint8_t compile_comments(void) {
    while(*bas.src <= ' ' && *bas.src > 0x03 && *bas.src != '\n' && *bas.src != ETX) bas.src++; // not skipping 0x01 .. 0x03
    if(*bas.src == '\'' && *(bas.src + 1) == ' ' && strlen(bas.src + 2) < 0x100) {  // compile the commentaries (if possible)
        *bas.src = 0x03;
        *(bas.src + 1) = (char) ((uint8_t) strlen(bas.src + 2));    // store the number of bytes to skip in the ' ' position
        bas.src += (2 + (uint8_t) *(bas.src + 1));
        return 1;
    }
    return 0;
}


// run source composed of zero-terminated script strings and finishing with a ETX character
// the (flags.use_codes) flag enables tokenisation by using instruction pseudo-codes (in such case source must be in RAM)
error_code_t runBasic(char **source, char use_codes) {
    srand((unsigned int) *source + *(*source)); // any better way to seed the random numbers?
    bas.entry = bas.src  = *source;
    if(bas.src == NULL || *bas.src == ETX) return E_OK;   // nothing to execute
    bas.flags.use_codes = use_codes;
    int ee = setjmp(bas.err_env);
    if(ee) goto end_here;

    // initialisations
    lcdCls();
    printf("%lu bytes free\r\n", (unsigned long) x_total());
    bas.flags.index_base = 1;   // default index base is 1 (standard for BASIC)
    bas.S.type = TYPE_STR; bas.S.s = NULL;
    bas.if_depth = 0;           // clear the 'if' depth
    memset(&bas.acc, 0, sizeof(bas.acc)); bas.acX = 0;      // clear the data stack
    memset(&bas.ops, 0, sizeof(bas.ops)); bas.opX = 0;      // clear the operations stack
    memset(&bas.fun, 0, sizeof(bas.fun)); bas.fuX = 0;      // clear the functions stack
    memset(&bas.loop, 0, sizeof(bas.loop)); bas.lpX = 0;    // clear the loops stack
    memset(&bas.cas, 0, sizeof(bas.cas)); bas.caX = 0;      // clear the sub calls stack
    memset(bas.str_buf, 0, sizeof(bas.str_buf));
    bas.vars = NULL; bas.subs = NULL;
    bas.flags.syntax_style = 0;
    bas.data_entry = bas.data_current = NULL;
    bas.flags.check_break = 1;  // by default enable Ctrl-C break

    // reset file structures
    for(bas.A.i = 0; bas.A.i < MAX_FILES; bas.A.i++) {
        bas.file[bas.A.i].hdr = 0;
        bas.file[bas.A.i].pos = -1;
    }

    // fill the keyword search acceleration table
    {
        memset(bas.kindex, 0, sizeof(bas.kindex));
        uint8_t ix = 0; // index 0 are the non-alphabetical elements
        const keyword_t *k;
        for(k = keywords; k->code; k++) {
            if(bas.kindex[ix] == NULL) bas.kindex[ix] = k;
            char nc = *(k->name);
            char nn = *((k + 1)->name);
            if(isalpha(nc)) {
                if(nn != nc) ix = (uint8_t) toupper(nn) - 'A' + 1;
            }
            else {
                if(isalpha(nn)) ix = (uint8_t) toupper(nn) - 'A' + 1;
            }
        }
    }

    // scan the source and create a list of sub entries (also records the start of DATA)
    bas.flags.no_str_consts = 1;
    bas.tk_src = bas.src = bas.entry;
    bas.t_token = T_UNKNOWN;
    while(bas.t_token != T_ETX) {
        getToken();
        if(bas.t_token == T_SUB || bas.t_token == T_LABEL) {
            getId();
            sub_t *st, *sp = NULL;
            if(x_malloc((byte **) &sp, (size_x) sizeof(sub_t)) == NULL) error(E_MEMORY); // allocate for the new sub structure
            sp->entry = bas.src;
            sp->name = bas.id_src;
            sp->nlen = bas.id_len;
            sp->isLabel = (bas.t_token == T_LABEL);
            sp->next = NULL;
            st = bas.subs;
            while(st && st->next) {
                if(st->nlen == sp->nlen && !istrncmp(st->name, sp->name, sp->nlen)) error(E_DUPLICATE);
                st = st->next;        // go to the currently last sub record
            }
            if(st) {
                if(st->nlen == sp->nlen && !istrncmp(st->name, sp->name, sp->nlen)) error(E_DUPLICATE);
                st->next = sp;
            }
            else bas.subs = sp;
            continue;
        }
        if(bas.t_token == T_DATA) {    // record the first DATA entry
            if(bas.data_entry == NULL) bas.data_entry = bas.data_current = bas.src;
            continue;
        }
        if(bas.t_token == T_UNKNOWN) getId();
    }

    // scan the source and create pseudo-codes
    bas.flags.no_str_consts = 1;
    bas.tk_src = bas.src = bas.entry;
    bas.t_token = T_UNKNOWN;
    if(bas.flags.use_codes) {
        while(bas.t_token != T_ETX) {
            if(compile_comments()) continue;
            getToken();
            if(bas.t_token == T_ETX || bas.t_token == T_EOL || bas.t_token == T_NUMBER || bas.t_token == T_STRING) continue;
            if(bas.t_token == T_IDENTIFIER || bas.t_token == T_SUBID || bas.t_token == T_LABELID) continue;
            if(bas.t_token != T_UNKNOWN) {
                const keyword_t *k = findIt(bas.tk_src);
                if(k->code) {
                    if(k->code >= 0x100) {  // two-byte codes
                        *bas.tk_src = (k->code >> 8);
                        *(bas.tk_src + 1) = (uint8_t) k->code;

                        // special case THEN
                        // search for matching ELSE or ENDIF
                        if(k->token == T_THEN) {
                            uint8_t extra_depth = 0;
                            uint8_t bfscf = bas.flags.no_str_consts;
                            char *loc = bas.tk_src; // this is where the 16-bit offset will be recorded
                            bas.flags.no_str_consts = 1;
                            while((bas.t_token != T_ELSE && bas.t_token != T_ENDIF)
                                    || extra_depth) {
                                if(bas.t_token == T_ETX) error(E_NO_ENDIF);
                                if(bas.t_token == T_IF) {
                                    if(++extra_depth >= MAX_IFS) error(E_TOO_COMPLEX);
                                }
                                if(bas.t_token == T_ENDIF) {
                                    if(extra_depth-- == 0) error(E_NO_IF);
                                }
                                if(compile_comments()) continue;
                                getToken();
                                if(bas.t_token == T_UNKNOWN) getId();
                            }
                            bas.flags.no_str_consts = !!bfscf;
                            if(bas.t_token == T_ELSE) bas.tk_src = bas.src; // if jumping to ELSE, it must be skipped
                            int32_t offs = bas.tk_src - loc;
                            if(offs >= 0 && offs <= 0xFFFF) {
                                *(loc + 2) = (char) ((uint8_t) (offs >> 8));
                                *(loc + 3) = (char) ((uint8_t) offs);
                            }
                            else memcpy((loc - 2), k->name, k->nlen);   // not possible so just use the text name then
                            continue;
                        }

                        // special case ELSE
                        // search for matching ENDIF
                        if(k->token == T_ELSE) {
                            uint8_t extra_depth = 0;
                            uint8_t bfscf = bas.flags.no_str_consts;
                            char *loc = bas.tk_src; // this is where the 16-bit offset will be recorded
                            bas.flags.no_str_consts = 1;
                            while(bas.t_token != T_ENDIF || extra_depth) {
                                if(bas.t_token == T_ETX) error(E_NO_ENDIF);
                                if(bas.t_token == T_IF) {
                                    if(++extra_depth >= MAX_IFS) error(E_TOO_COMPLEX);
                                }
                                if(bas.t_token == T_ENDIF) {
                                    if(extra_depth-- == 0) error(E_NO_IF);
                                }
                                if(compile_comments()) continue;
                                getToken();
                                if(bas.t_token == T_UNKNOWN) getId();
                            }
                            bas.flags.no_str_consts = !!bfscf;
                            int32_t offs = bas.tk_src - loc;
                            if(offs >= 0 && offs <= 0xFFFF) {
                                *(loc + 2) = (char) ((uint8_t) (offs >> 8));
                                *(loc + 3) = (char) ((uint8_t) offs);
                            }
                            else memcpy((loc - 2), k->name, k->nlen);   // not possible so just use the text name then
                            continue;
                        }

                    }
                    else *bas.tk_src = (uint8_t) k->code;   // single byte codes
                }
                else error(E_UNDEFINED);
            }
            else getId();
        }
        bas.tk_src = bas.src = bas.entry;
        bas.t_token = T_UNKNOWN;
    }

    // this is where the code gets executed
    bas.flags.no_str_consts = 0;
    while(*bas.src != ETX) statement();
    error(E_OK);        // successful execution

    end_here:;

    error_code_t ecode = bas.error_code;
    bas.error_code = E_OK;

    // restore the original source from pseudo-codes
    if(bas.flags.use_codes) {
        bas.data_current = bas.src; // use (*data_current) to store the original source point on exit
        bas.flags.no_str_consts = 1;
        bas.tk_src = bas.src = bas.entry;
        bas.t_token = T_UNKNOWN;
        while(bas.t_token != T_ETX && bas.error_code == E_OK) {
            if(compile_comments()) continue;
            getToken();
            if(bas.t_token == T_ETX || bas.t_token == T_EOL || bas.t_token == T_NUMBER || bas.t_token == T_STRING) continue;
            if(bas.t_token == T_IDENTIFIER || bas.t_token == T_SUBID || bas.t_token == T_LABELID) continue;
            if(bas.t_token != T_UNKNOWN) {
                uint16_t code = (uint16_t) *bas.tk_src;
                const keyword_t *k;
                if(code < 0x03) code = (code << 8) + *(bas.tk_src + 1);
                if(code >= 0x160) k = bas.kindex[((code - 0x160) >> 4) + 1];
                else k = bas.kindex[0];
                for( ; k->code && k->code != code; k++);
                if(k->code == code) {
                    uint8_t t;
                    switch(bas.flags.syntax_style) {
                        default:
                        case 0:
                        case 1:
                            for(t = 0; t < k->nlen; t++) *(bas.tk_src + t) = (char) tolower(*(k->name + t));
                            break;
                        case 2:
                            for(t = 0; t < k->nlen; t++) *(bas.tk_src + t) = (char) toupper(*(k->name + t));
                            break;
                        case 3:
                            for(t = 0; t < k->nlen; t++) *(bas.tk_src + t) = (char) tolower(*(k->name + t));
                            *bas.tk_src = (char) toupper(*bas.tk_src);
                            break;
                    }
                }
                // else what?
            }
            else getId();
        }
        bas.src = bas.data_current; // restore the original source pointer on exit
    }

    *source = bas.src;      // update the source position on exit
    //if(ecode == E_MEMORY) x_list_alloc();   // debug allocated memory on exit
    x_defrag();
    lcdCursorOn();
    return ecode;   // here the execution terminates with an error (or OK)
}
