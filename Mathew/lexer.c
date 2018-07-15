#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mathew.h"
#include "keyword.h"

#define FILE_NAME   (Input->filename)
#define CURSOR      (Input->cursor)
#define LINE        (Input->line)
#define COLUMN      (Input->column)
#define EXCLUDE     (Input->exclude)
#define MASTARTLN   (Input->mastartln)
#define MACOLUMN    (Input->column)
#define MADEFINE    (Input->madefine)
#define PROCEED     (Input->proceed)
#define PTPLSTRING  (&Input->tplstr)

#define GET_CURSOR(a) getCursor(a)

#define SINGLE_CHAR_SCANNER(t) \
PRIVATE int scan##t()          \
{                              \
    ADD_CURSOR(1);                  \
    return TK_##t;             \
}

typedef int (*Scanner)();
static Scanner    scanners[256];
static Scanner    scanAll;

static struct token tokenValue;
static struct token look_ahead_token;              /* Look a head token */
static int look_ahead_token_exists;                /* Look a head token exists */
static struct input *Input;

typedef int (*CursorAction)(int);
static CursorAction    ADD_CURSOR;

typedef int (*PartAction)();
static PartAction      GET_PARTCOLUMN;

PRIVATE void measure(int i)
{
    int size;
    int len = CURSOR - Input->filebuf;
    len = len + i;
    if(len >= Input->bufsize){                    /* (+) positive */

        len = (&Input->filebuf[Input->bufsize]) - CURSOR;
        if(len + i >= Input->bufsize){
            Error_fprintf(MEASURE_FAILED,i);       /* cur[0] and cur[i] need in same buffer */
            exit(1);
        }

        fseek(Input->fp,-len,SEEK_CUR);      /* rollback end-cur locate to cur then read */
        size = fread(Input->filebuf,1,Input->bufsize,Input->fp);

        if(size < Input->bufsize){
            PROCEED = FALSE;
        }

        Input->readsize = size;
        Input->filebuf[size] = 0;                  /* end of input */
        CURSOR   = Input->filebuf;

    }else if(CURSOR + i < Input->filebuf){         /* (-) negative */

        len = Input->readsize + (Input->filebuf - (CURSOR + i));

        if(len + (CURSOR - Input->filebuf) >= Input->bufsize){
            Error_fprintf(MEASURE_FAILED,i);       /* cur[-i] and cur[0] need in same buffer */
            exit(1);
        }

        fseek(Input->fp,-len,SEEK_CUR);
        size = fread(Input->filebuf,1,Input->bufsize,Input->fp);

        if(size >= Input->bufsize){
            PROCEED = TRUE;
        }

        Input->readsize = size;
        Input->filebuf[size] = 0;                 /* end of input */

        CURSOR   = Input->filebuf + len;
    }
}

PRIVATE int getCursor(int i)
{
    if(PROCEED){
        measure(i);
    }
    return CURSOR[i];
}

PRIVATE int addCursor(int i)
{
    int ch = *CURSOR;
    if(PROCEED){
        measure(i);
    }
    COLUMN += i;
    CURSOR += i;
    return ch;
}
/* capture cursor */
PRIVATE int capCursor(int i)
{
    int j, ch;

    ch = *CURSOR;
    for(j = 0; j < i; j++){
        Tplstring_chcat(PTPLSTRING,addCursor(1));
    }
    return ch;
}

PRIVATE void SetCursorState(CURSOR_STATUS s)
{
    if(s == ADDITION){
        ADD_CURSOR = addCursor;
    }else{
        ADD_CURSOR = capCursor;
    }
}


PRIVATE int scanNormal()
{
    return (*scanners[*CURSOR])();
}

PRIVATE int scanSpecial()
{

    ADD_CURSOR(1); /* skip one ch then take string */
    Tplstring_reset(PTPLSTRING);
    while(!isspace(*CURSOR) && *CURSOR != END_OF_INPUT){
        Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
    }

    Tplstring_chcat(PTPLSTRING,0);

    tokenValue.value    = StrSafe(PTPLSTRING->data);

    return TK_SPECIAL;
}

PRIVATE int findKeyword(char *str, int len)
{
    struct keyword *p;
    int index;

    index = toupper(*str) - 'A';
    p = keywords[index];
    while (p->name){
        if (p->len == len && strncmp(str, p->name, len) == 0)
            break;
        p++;
    }
    return p->tok;
}

PRIVATE void skipComments()
{
    if(*CURSOR == '/'){
        ADD_CURSOR(1);
        while ((*CURSOR != END_OF_INPUT) && *CURSOR != '\n' )
        {
           ADD_CURSOR(1);
        }
    }else{
        int ch;
        ADD_CURSOR(1);                                  /* Jump tow char prevent "/\*\/" */
        ch = ADD_CURSOR(1);
        while((*CURSOR != END_OF_INPUT) && (*CURSOR != '/' || ch != '*')){
           if(*CURSOR == '\n'){
              LINE++;
              COLUMN = 0;
           }
           ch = ADD_CURSOR(1);
        }
        if(*CURSOR != END_OF_INPUT)                     /* Skip '/' */
           ADD_CURSOR(1);
    }
}

PRIVATE void skipWhiteSpace()
{
    int ch;
    ch = *CURSOR;
    while((ch != END_OF_INPUT) && (isspace(ch) || ch == '/')){
         if(ch =='\n'){                               /* Is end of line */
             LINE++;
             COLUMN = 0;
             ADD_CURSOR(1);
         }else if(ch == '/'){                         /* Is comments */
             if (GET_CURSOR(1) != '/' && GET_CURSOR(1) != '*')
                 return;
             ADD_CURSOR(1);
             skipComments();
         } else {                                     /* Is space char */
             ADD_CURSOR(1);
         }

         ch = *CURSOR;
    }
}

PRIVATE void macroNest()
{
    int ch;

    ch = *CURSOR;
    while(EXCLUDE > 0 && (ch != END_OF_INPUT) ){
        skipWhiteSpace();
        ch = *CURSOR;
        GET_CURSOR(7);          /* measure */
        /* Jump string a character literals */
        if(ch == '\'' || ch == '\"'){
            int startch,prevch;
            startch = ch;
            prevch = 0;
            CURSOR++;
            while(*CURSOR != END_OF_INPUT && (*CURSOR != startch || prevch == '\\')){
                if(*CURSOR == '\n'){
                    LINE++;
                    COLUMN = 0;
                }
                if(prevch == '\\')prevch = 0;
                else prevch = *CURSOR;
                ADD_CURSOR(1);
            }
        }else if(ch == '%' && ((strncmp(CURSOR,"%ifdef",6) == 0 && isspace(CURSOR[6]))
            || (strncmp(CURSOR,"%ifndef",7) == 0 && isspace(CURSOR[7])))){
            EXCLUDE++;
            CURSOR += 6 + (CURSOR[3] == 'n')*(7 - 6);
        }else if(strncmp(CURSOR,"%endif",6) == 0 && isspace(CURSOR[6])){
            EXCLUDE--; /* Need from 1 to 0 */
            CURSOR += 6;
        }else{
            CURSOR++;
        }
    }
}

PRIVATE int macroProcess()
{
    int ch;

    ch = *CURSOR;
    if(ch == '%' ){
        GET_CURSOR(7); /* measure */
        if((strncmp(CURSOR,"%ifdef",6) == 0 && isspace(CURSOR[6]))
            || (strncmp(CURSOR,"%ifndef",7) == 0 && isspace(CURSOR[7]))){

            int i, m, n;
            int nDefine = MADEFINE->count;
            char **maDefine = MADEFINE->def_array;


            m = 6 + (CURSOR[3] == 'n')*(7 - 6);

            for(ADD_CURSOR(m); isspace(*CURSOR) && (*CURSOR != '\n'); ){
                ADD_CURSOR(1);   /* jump space macro value need at same row */
            }

            n = 0;
            for(Tplstring_reset(PTPLSTRING); *CURSOR && !isspace(*CURSOR); ){
                Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
                n++;
            }

            Tplstring_chcat(PTPLSTRING,0);
            EXCLUDE = 1;
            for(i = 0; i < nDefine; i++){
                if(strncmp(maDefine[i],PTPLSTRING->data,n)==0 && strlen(maDefine[i])==n){
                    EXCLUDE = 0;
                    break;
                }
            }

            if(m == 7)EXCLUDE = !EXCLUDE;
            if(EXCLUDE){
                MASTARTLN =  LINE;
                MACOLUMN  = COLUMN;
                macroNest();
            }
            return 1;
        }else if(strncmp(CURSOR,"%endif",6) == 0 && isspace(CURSOR[6])){
            EXCLUDE--;
            CURSOR += 6;
            return 1;
        }

    }
    return 0;
}
/* proceed for:
 * %ifdef Win32
 *
 * %endif
*/
PRIVATE void preprocess()
{
    int proceed;

    do{
        skipWhiteSpace();
        proceed = macroProcess();
        skipWhiteSpace();
    }while(proceed);

}

PRIVATE  int scanIdentifier()
{

    int tok;
    int i = 1;

    Tplstring_reset(PTPLSTRING);
    Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
    while(isalnum(*CURSOR) || *CURSOR == '_'){
        Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
        i++;
    }
    Tplstring_chcat(PTPLSTRING,0);  /* end of template string */
    tok = findKeyword(PTPLSTRING->data, i);
    
    /* Save Identifier in string hash table keyword also save */

    tokenValue.value    = StrSafe(PTPLSTRING->data);

    return tok;
}

PRIVATE int scanCodeBlock()
{
    int level;
    int ch;
    int line;
    int column;

    line = LINE;
    column = COLUMN;

    ADD_CURSOR(1); /* skip '{' */
    Tplstring_reset(PTPLSTRING);
    SetCursorState(CAPTURE);
    for(level = 1; ((ch = *CURSOR) != END_OF_INPUT) && (level > 1 || ch != '}'); ){ 
        if( ch == '\n' ) {
            LINE++;
            COLUMN = 0;
            ADD_CURSOR(1);
        }else if( ch == '{' ){
            level++;
            ADD_CURSOR(1);
        }else if( ch == '}' ){
            level--;
            ADD_CURSOR(1);
        }else if(ch == '/'){                          /* Is comments */
            ADD_CURSOR(1);
            if (*CURSOR != '/' && *CURSOR != '*')
                continue;
            skipComments();                           /* need Tplstring_chcat comments ;*/
        }else if( ch =='\'' || ch =='\"' ){           /* To prevent string have '{' or '}' */
            int startchar, prevc;
            int str_line;
            int str_column;

            prevc = 0;
            startchar = ch;
            str_line = LINE;
            str_column = COLUMN;

            for(ADD_CURSOR(1); (ch = *CURSOR) != END_OF_INPUT && (ch != startchar || prevc == '\\'); ADD_CURSOR(1)){
               if( ch == '\n' ) {
                  LINE++;
                  COLUMN = 0;
               }
               if( prevc == '\\' ) prevc = 0;
               else                prevc = ch;
            }

            if( ch == END_OF_INPUT ){                                   /* Will add code block till file tail */
                Error_fprintf(VALUE_STRING_NOT_TERMINAL,FILE_NAME,str_line,str_column,'"');
            }

            if(ch != END_OF_INPUT)                     /* Skip '\'' or '"' */
                ADD_CURSOR(1);
        }else{
            ADD_CURSOR(1);
        }

    }

    SetCursorState(ADDITION);
    Tplstring_chcat(PTPLSTRING,0);  /* end of template string */

    if(ch == END_OF_INPUT){                          /* Will add code block till file tail */
        Error_fprintf(CODE_STRING_NOT_TERMINAL,FILE_NAME,line,column,'}');
    }

    tokenValue.value  = StrSafe(PTPLSTRING->data);

    if(ch == '}'){                                    /* Skip end of code block '}' */
        ADD_CURSOR(1);
    }

    return TK_CODE_BLOCK;
}

PRIVATE int scanTypeBlock()
{
    int ch;
    int line;
    int column;

    line = LINE;
    column = COLUMN;

    ADD_CURSOR(1);
    Tplstring_reset(PTPLSTRING);
    while((ch = *CURSOR) != END_OF_INPUT && ch !='>'){
       if(ch == '\n') {
           LINE++;
           COLUMN = 0;
       }
       Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
    }
    Tplstring_chcat(PTPLSTRING,0);

    if(ch == 0){                                    /* Will add code block till file tail */
        Error_fprintf(TYPE_STRING_NOT_TERMINAL,FILE_NAME,line,column,'>');
    }

    tokenValue.value  = StrSafe(PTPLSTRING->data);

    if(ch == '>'){ /* Skip end of type block '>' */
        ADD_CURSOR(1);
    }

    return TK_TYPE_BLOCK;

}

PRIVATE int scanAlias()
{
    int column;

    column = COLUMN;
    Tplstring_reset(PTPLSTRING);
    Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
    if (isalpha(*CURSOR))                             /* Alias $alpha[(alpha|number|_)]*/
    {
        char ch;

        Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
        while(isalnum(*CURSOR) || *CURSOR == '_'){
            Tplstring_chcat(PTPLSTRING,ADD_CURSOR(1));
        }

        Tplstring_chcat(PTPLSTRING,0);

        tokenValue.value  = StrSafe(PTPLSTRING->data);

        return TK_ALIAS;
    }
    else
    {
        Error_fprintf(ILLEGAL_CHARACTER, FILE_NAME, LINE, column, '$');
        return TK_BAD;
    }
}

PRIVATE int scanArrow()
{
    ADD_CURSOR(1);
    if (*CURSOR == '>')
    {
        ADD_CURSOR(1);
        return TK_ARROW;
    }
    else
    {
        Error_fprintf(ILLEGAL_CHARACTER, FILE_NAME, LINE, COLUMN, '-');
        return TK_BAD;
    }
}

PRIVATE int scanBadChar()
{
    Error_fprintf(ILLEGAL_CHARACTER, FILE_NAME, LINE, COLUMN, *CURSOR);
    ADD_CURSOR(1);
    return TK_BAD;
}

PRIVATE int scanEND()
{
    return TK_END;
}

SINGLE_CHAR_SCANNER(DECL)
SINGLE_CHAR_SCANNER(DOT)
SINGLE_CHAR_SCANNER(LBRACKET)
SINGLE_CHAR_SCANNER(RBRACKET)
SINGLE_CHAR_SCANNER(LPAREN)
SINGLE_CHAR_SCANNER(RPAREN)

void SetScanState(LEX_STATUS s)
{
    if(s == SPECIAL){
        scanAll = scanSpecial;
    }else{
        scanAll = scanNormal;
    }
}

PRIVATE int getPartcolumn()
{
    int i, n, column;
    char *cursor;
    char *start;

    column = 0;
    cursor = CURSOR;
    start  = (cursor - 12 < Input->filebuf) ? Input->filebuf : (cursor - 12);

    n =  cursor - start;


    Tplstring_reset(PTPLSTRING);

    /* start ---- cursor */
    for(i = 0; i < n; i++){
        if(start[i] == '\n'){
            Tplstring_reset(PTPLSTRING);
            column = 0;
        }else{
            Tplstring_chcat(PTPLSTRING,start[i]);
            column++;
        }

    }

    /* cursor ---- end line */
    while((*cursor != END_OF_INPUT) && (*cursor != '\n'))
    {
        Tplstring_chcat(PTPLSTRING,*cursor);
        cursor++;
    }

    Tplstring_chcat(PTPLSTRING,0);

    tokenValue.part = StrSafe(PTPLSTRING->data);

    return column;
}

PRIVATE int getEmptyPartcolumn()
{
    tokenValue.part = "";
    return 0;
}

void SetPartState(FAILURE_STATUS s)
{
    if(s == INDICATE){
        GET_PARTCOLUMN = getPartcolumn;
    }else{
        GET_PARTCOLUMN = getEmptyPartcolumn;
    }
}


/* Initialize  lexer*/
void InitLexer(struct input *in)
{
    int i;
    look_ahead_token_exists = 0;
    Input = in;

    for (i = 1; i < 256; i++){
        if (isalpha(i)){
            scanners[i] = scanIdentifier;
        }else{
            scanners[i] = scanBadChar;
        }
    }

    scanners['%'] = scanDECL;
    scanners['<'] = scanTypeBlock;
    scanners['.'] = scanDOT;
    scanners['-'] = scanArrow;
    scanners['{'] = scanCodeBlock;
    scanners['['] = scanLBRACKET;
    scanners[']'] = scanRBRACKET;
    scanners['('] = scanLPAREN;
    scanners[')'] = scanRPAREN;
    scanners['$'] = scanAlias;
    scanners[END_OF_INPUT] = scanEND;

    SetScanState(NORMAL);
    SetPartState(INDICATE);
    SetCursorState(ADDITION);

}

PRIVATE struct token *getNextToken()
{

    preprocess();

    tokenValue.value        = 0;

    tokenValue.line         = LINE;
    tokenValue.column       = COLUMN;
    tokenValue.part_column  = GET_PARTCOLUMN();

    tokenValue.type         = scanAll();
    return &tokenValue;
}

struct token *GetNextToken()
{
    if(look_ahead_token_exists){
        look_ahead_token_exists = 0;
        return &look_ahead_token;
    } else {
        return getNextToken();
    }
}

struct token *GetToken()
{
    if(look_ahead_token_exists){
        return &look_ahead_token;
    } else {
        return &tokenValue;
    }

}

void UnGetToken(struct token *tok)
{
    look_ahead_token = *tok;
    look_ahead_token_exists = 1;
}
