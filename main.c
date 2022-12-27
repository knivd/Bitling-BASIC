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

    //memcpy(TEXT, "dim int t=1,r=t+1\0print t,r,\"hello\"\0\x1f", 300);
    //memcpy(TEXT, "var int a[5,5],t,r\0for t=1 to 5\0for r=1 to 5\0a[t,r]=t*r\0print t,r,a[t,r]\0next\0next\0\x1f", 300);
    //memcpy(TEXT, "var int t\0for t=0 to 9:print t;:next\0\x1f", 300);
    //memcpy(TEXT, "var int t\0for t=0 to 9:print \"hello\":next\0\x1f", 300);
    //memcpy(TEXT, "sub x(int p)\0print p;\0end sub\0dim int t\0for t=4 to 6:x(t):next\0\x1f", 300);
    //memcpy(TEXT, "sub x(string s)\0print s\0end sub\0dim int t\0for t=2 to 8:x(\"hello\"):next\0\x1f", 300);
    //memcpy(TEXT, "sub x(int p,string s)\0print p,s\0end sub\0dim int t\0for t=3 to 7:x(t,\"hello\"):next\0\x1f", 300);
    //memcpy(TEXT, "dim string s[5]:dim int i,t[5]\0for i=1 to 5:t[i]=i:next\0print \"zz\"s[1];t[1],t[2]\0\x1f", 300);
    //memcpy(TEXT, "sub x(int a,int b) as int\0return a*b\0end sub\0dim int t,r\0for t=1 to 500\0r=x(t,t+1)\0print t,r\0next t\0\x1f", 300);
    //memcpy(TEXT, "dim int t=1\0label z\0print t;\"  \";:t=t+1\0goto z\0\x1f", 300);
    //memcpy(TEXT, "dim int t\0for t=1 to 1000\0next\0\x1f", 300);
    //memcpy(TEXT, "data 1,\"hi\"\0data 2,\"hello\",3,\"bye\"\0dim int t,r:dim string a\0for t=1 to 6:read r,a\0? r,a:next\0data 4,\"hi2\",5,\"hello2\"\0data 6,\"bye2\"\0\x1f", 300);
    //memcpy(TEXT, "var string a,b,s=\"hello\"\0a=s:b=a+s\0print s,a,b\0\x1f", 300);
    //memcpy(TEXT, "var string a[3]\0a[1]=\"hello\"\0print a[1]\0\x1f", 300);
    //memcpy(TEXT, "var string a[3]\0a[1]=\"hello\"\0a[2]=\":\"\0a[3]=\"world\"\0print a[1];a[2];a[3]\0\x1f", 300);
    //memcpy(TEXT, "var string a[3]\0a[1]=\"hello\"\0a[2]=\" \"\0a[3]=\"world\"\0dim string s=a[1]+a[2]+a[3]\0a[1]=\"!!!\":print s+a[1]\0\x1f", 300);
    //memcpy(TEXT, "var string a,b,s=\"hello world\"\0a=mid$(s,3,9)\0? s,a\0\x1f", 300);
    //memcpy(TEXT, "var int a\0label m\0print \"Input:\";:input a\0print a\0goto m\0\x1f", 300);
    //memcpy(TEXT, "dim integer t=1\0print t\0\x1f", 300);
    //memcpy(TEXT, "var int t\0timer(1)\0while timer(0)<30000\0print timer(0)\0wend\0\x1f", 300);
    //memcpy(TEXT, "var int t,r=0\0print \"Testing...\"\0timer(1)\0while timer(0)<10000\0r=r+1\0wend\0print r/10;\" its/s\"\0\x1f", 300);
    //memcpy(TEXT, "var string s=\"\"\0var int t\0for t=1 to 20\0s=s+str$(t,0)\0print s;\" \";\0next\0\x1f", 300);
    //memcpy(TEXT, "print #\"COM1:9600\", \"Hello in the console!\"\0\x1f", 300);

    #if 0
    memcpy(TEXT,
        "dim integer t,i=0" "\0"
        "dim float x[100],f=0.0" "\0"
        "dim string s=\"\"" "\0"
        "print \"Grainbench 2016/22\"" "\0"
        "print \"Calculating...\"" "\0"
        "s=\"\":f=0.0:i=0" "\0"
        "TIMER(1)" "\0"
        "while TIMER(0)<=30000" "\0"
            "i=i+2:f=f+2.0002" "\0"
            "if (i // 2)=0 then" "\0"
                "i=i*2:i=i/2" "\0"
                "f=f*2.0002:f=f/2.0002" "\0"
            "endif" "\0"
            "i=i-1" "\0"
            "for t=1 to 100" "\0"
                "f=f-1.0001" "\0"
                "if (f-trunc(f))>=0.5 then" "\0"
                    "f=sin(f*logd(i))" "\0"
                    "s=str$(f,6)" "\0"
                "endif" "\0"
                "f=(f-tan(i))*(RND(0)/i)" "\0"
                "if instr(s,left$(str$(i,0),2),1)>0 then " "\0"
                    "s=s+\"0\"" "\0"
                "endif" "\0"
            "next" "\0"
            "x[1+(i // 100)]=f" "\0"
        "wend" "\0"
        "print \"Performance:\"" "\0"
        "print str$((i*1024)/286,0)+\" grains\"" "\0"
        "\x1f", 700);
    #endif

    #if 0
    memcpy(TEXT,
        "sub x(string @s,int p)\0"
        "  print s[p]\0"
        "end sub\0"

        "dim int t\0"
        "dim string m[10]\0"
        "for t=1 to 10\0"
        "  m[t]=\"test\"+str$(t,0)+str$(t-1,0)\0"
        "  ? \"HERE \";m[t]\0"
        "next\0"

        "label back\0"
        "  input t\0"
        "  x(@m,t)\0"
        "goto back\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "var int a, t=TIMER(0)" "\0"
        "while (TIMER(0)-t)<1000" "\0"
        "a=a+1" "\0"
        "wend" "\0"
        "? a*7;\" ops/s\"" "\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "var int a" "\0"
        "var string s=\"Hello LC1! \"" "\0"
        "label k" "\0"
        "? a;\"  \";s" "\0"
        "s=right$(s,10)+left$(s,1)" "\0"
        "a=a+1:goto k" "\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "print \"Number:\";" "\0"
        "var int a" "\0"
        "input a" "\0"
        "if a<10 then a=10!" "\0"
        "print a:? \"next line\"" "\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "var float a=100-99.9" "\0"
        "var float b=10-9.9" "\0"
        "if a==b then print \"Same\"" "\0"
        "else print \"Different\"" "\0"
        "endif" "\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "var float a=100-99.9" "\0"
        "var float b=10-9.9" "\0"
        "? a+b,b+a" "\0"
        "\x1f", 200);
    #endif

    #if 0
    memcpy(TEXT,
        "var string s=\"* Merry Christmas! *\"" "\0"
        "var string d=\" from ELLO Computer \"" "\0"
        "var string e=\" and Bitling BASIC  \"" "\0"
        "var int t,r,p=(TDWDT()-len$(s))/2" "\0"
        "cls:cursor 0:vloc 1" "\0"
        "for t=1 to len$(s)" "\0"
        "  if mid$(s,t,1)<>\" \" or mid$(d,21-t,1)<>\" \" or mid$(e,t,1)<>\" \" then" "\0"
        "    for r=TDWDT() to (p+t) step -1" "\0"
        "      vloc 1:hloc r:putch asc(mid$(s,t,1))" "\0"
        "      'wait 50" "\0"
        "      vloc 1:hloc r:putch asc(\" \")" "\0"
        "    next" "\0"
        "    vloc 1:hloc r:putch asc(mid$(s,t,1))" "\0"
        "    vloc 2:hloc 21-r:putch asc(mid$(d,21-t,1))" "\0"
        "    vloc 3:hloc r:putch asc(mid$(e,t,1))" "\0"
        "  endif" "\0"
        "next" "\0"
        " 'vloc 4" "\0"
        " 'for t=1 to TDWDT()" "\0"
        " '  hloc t:putch 0x12" "\0"
        " 'next" "\0"
        " 'get s" "\0"
        "\x1f", 600);
    #endif

    while(1) editor(TEXT);  // main loop
    Reset();        // double assurance; execution should never reach this line
}
