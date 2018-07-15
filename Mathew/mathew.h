#ifndef PRIVATE_MATHEW_H_INCLUDED
#define PRIVATE_MATHEW_H_INCLUDED

#ifndef __WIN32__
#   if defined(_WIN32) || defined(WIN32)
#	define __WIN32__
#   endif
#endif

#define PRIVATE static

/*********************** Memory interface***************************/
#define Mem_alloc   malloc
#define Mem_relloc  realloc
#define Mem_free    free

#define SEG             int
#define	BITS_PER_SEG	sizeof(SEG)

#define	SEGSIZE(n)	(((n) + BITS_PER_SEG - 1) / BITS_PER_SEG)
#define	SETBIT(x, i)	(((SEG*)x)[(i)/BITS_PER_SEG] |= (1<<((i) % BITS_PER_SEG)))
#define RESETBIT(x, i)	(((SEG*)x)[(i)/BITS_PER_SEG] &= ~(1<<((i) % BITS_PER_SEG)))
#define BITISSET(x, i)	((((SEG*)x)[(i)/BITS_PER_SEG] & (1<<((i) % BITS_PER_SEG))) != 0)

#define SYMBOL_TYPE(a)  islower(*a) ?  NONTERMINAL : TERMINAL
#define SYMBOL_NEW(a)   Symbol_new(a,SYMBOL_TYPE(a))

#define MEM_CHECK(x) if(x==0){\
    Error_fprintf(OUT_OF_MEMORY);\
    exit(1);\
}

typedef enum {
    #define ERROR(k, s) k,      /* Error info table */
    #include "error.h"
    #undef  ERROR
} Error;

typedef enum {
    #define OUTPUT(k, s) k,      /* Output info table */
    #include "output.h"
    #undef  OUTPUT
} Output;

typedef unsigned int(*hashfunc)();
typedef int(*comparefunc)();

enum e_token
{
    #define TOKEN(k, s) k,
    #include "token.h"
    #undef  TOKEN
};

#define MAXRHS 100
#define END_OF_INPUT 0
#define NO_OFFSET (-2147483647)

typedef enum {FALSE = 0,TRUE}BOOL;

enum e_assoc{
    LEFT,
    RIGHT,
    NONASSOC,
    UNKNOW
};

union opt_value
{
    int    iv;
    double dv;
    char   *sv;
};

struct option
{
    int allprintflag;
    int basisflag;
    int compressflag;
    int driverflag;
    int epsilonflag;
    int firstsetflag;
    int hiddenflag;
    int rpflag;
    int mhflag;
    int nousedruleflag;
    int qiuetflag;
    int statesflag;
    int usedruleflag;
    int versionflag;
    char *tplfilename;          /* template file name */
    int macroinc;               /* Integer value */
    int bufsize;                /* Integer value */
    char **argv;                /* Hide value argv */
    char *filename;             /* Hide value grammar file name*/
};

struct option_info
{
    enum { OPT_FLAG=1, OPT_INT, OPT_DBL, OPT_STR, OPT_FUNC} type;
    char *label;
    char *arg;
    char *message;
};

struct macro_define
{
    int count;
    int size;
    char **def_array;
};

/* Map red black tree */
enum COLOR { BLACK, RED, NIL };

struct Pair
{
    void *key;
    union{
        void *value;
        int num;
    };
};

typedef void(*pairfreefunc)(struct Pair *);

struct RBTNode
{
    struct Pair  obj;
    enum   COLOR color;
    struct RBTNode *parent;
    struct RBTNode *lchild;
    struct RBTNode *rchild;
};

struct map{
    struct RBTNode *root;
    comparefunc cmp;
};


/* Use global struct to include global variables */
struct global{
    struct state *states;       /* Sorted states list*/
    struct rule  *rule;         /* List of rules */
    struct rule  *lastrule;     /* Pointer to the most recently parsed rule */
    struct symbol *symbols;     /* Sorted symbols list*/
    struct symbol **symbol_array;
    struct symbol *start_symbol;

    struct option *opt;
    struct macro_define *madefine;

    int state_num;
    int rule_num;               /* Rule index */
    int symbol_num;
    int terminal_num;
    int err_count;
    int conflict_count;

    char *program_name;
    char *tplname;    
    char *filename;
    char *outname;

    char *arg;
    char *parse_name;
    char *token_prefix;
    char *start;               /* Name of start symbol */
    char *token_type;          /* Type of terminal symbols */
    char *nonterminal_type;    /* The nonterminal type of non-terminal symbols */
    //char *stack_size;        /* Size of the paser stack */
    char *include;
    char *error;
    //char *overflow;
    char *failed;
    char *accept;
    char *code;
    char *default_code;
    char *token_destruct;
    char *nonterminal_destruct;

    int  include_line;
    int  error_line;
    int  failed_line;
    int  accept_line;
    int  code_line;
    int  default_code_line;
    int  token_destruct_line;
    int  nonterminal_destruct_line;
};

/* There is node of hash table element */
struct node{
    char *data;
    struct node *next;        /* Next entry has the same hash */
};

/* There is a hash table struct store value */
struct table{
    int count;                /* The number of currently slots filled */
    int size;                 /* The number available slots */
    struct node *tbl;         /* The data stored here */
    struct node **ht;         /* Hash table for lookups */
};

struct set{
    SEG *value;
    int size;
};

/* symbol struct include terminals and nonterminals */
struct symbol{
    char *name;
    int index;
    enum {
        TERMINAL,
        NONTERMINAL
    }type;
    struct rule *rule;
    struct symbol *fallback;
    int prec;
    enum e_assoc assoc;
    struct set firstset;
    char *destructor;
    int destructor_line;
    int destructor_proc_num;
    char *datatype;
    int dtnum;
    struct symbol *next;
    BOOL epsilon;
};

/* state struct */
struct state{
    int index;
    struct config *bp;
    struct config *cfp;
    struct action *ap;
    struct state *next;
    int tokOffset;
    int ntOffset;
    int defaultAction;
};

struct config{
    struct state *stp;
    struct rule *rp;
    int dot;
    enum {
      COMPLETE,                /* The status is used during followset and */
      INCOMPLETE               /* Shift computations */
    } status;
    struct set followset;      /* Follow set in this configuration only */
    struct plink *fplp;        /* Dot+1 to tail epsilon mother config point to child config */
    struct plink *bplp;        /* Basic config point to pre state mother config */
    struct config *bp;         /* The next basis configuration */
    struct config *next;       /* Next configuration in the state */
};

struct rule{
    struct symbol *lhs;        /* Left hand side of the rule */
    char *lhsalias;            /* Alias for the lhs (NULL if none) */
    int index;                 /* Rule index gp->rule_num++ */
    int nrhs;                  /* Right hand side rule number */
    int code_line;             /* Reduce action code line */
    int reduce_proc_num;       /* reduce code corresponding process number */
    BOOL used;                 /* Record no used rule */
    BOOL canReduce;            /* SHIFT REDUCE and REDUCE REDUCE conflict left rule */
    struct symbol **rhs;       /* Right hand side rule array */
    char **rhsalias;           /* Right hand side rule alias array */
    char *code;                /* Reduce action code */
    struct symbol *precsymbol;
    struct rule *nextlhs;      /* Same left hand side rule */
    struct rule *next;         /* Global next rule */
};


struct action{
    struct symbol *forward;    /* The forward symbol */
    enum e_action{
        SHIFT,
        REDUCE,
        ACCEPT,
        CONFLICT,
        SH_RESOLVED,
        RD_RESOLVED
    }type;
    union {
        struct state *stp;     /* The new state, if a shift */
        struct rule *rp;       /* The rule, if a reduce */
    }x;
    struct action *next;       /* The next action for this state */
};

struct plink {
    struct config *cfp;        /* The configuration to which linked */
    struct plink *next;        /* The next propagate link */
};

/* Insert action node*/
struct anode
{
    int n;
    int code_line;
    char *code;  
};

struct rqueue{
    unsigned long qSize;
    unsigned long qLength;
    unsigned long iHead;        /* Ring queue head */
    unsigned long iTail;        /* Ring queue tail */
    unsigned long iLen;
    void *pQueue;
};

/* string.c */
struct TplString{
    int size;
    int capacity;
    char *data;
};

/* lexer.c */
struct input{
    FILE *fp;
    char *filebuf;
    char *filename;
    char *cursor;
    int line;
    int column;
    int exclude;
    int mastartln;
    int bufsize;
    int readsize;
    BOOL proceed;
    int macolumn;
    struct macro_define *madefine;
    struct TplString tplstr;
};

struct token{
    enum e_token type;
    char *value;
    char *part;                  /* Output error info line */
    int line;                    /* Linenumber at which current token starts */
    int column;
    int part_column;
};

/* for lexer state switch */
typedef enum{
    NORMAL = 0,
    SPECIAL
}LEX_STATUS;

typedef enum{
    ADDITION = 0,
    CAPTURE
}CURSOR_STATUS;

typedef enum{
    HIDDEN = 0,
    INDICATE
}FAILURE_STATUS;

/* acttab.c */
struct actel_lookahead{
    int lookahead;
    int action;
};

struct actel_state{
    int state;
    int action;
};

struct alookahead{
    int minLookahead;
    int maxLookahead;
    int nLookahead;
    int nLookaheadAlloc;
    struct actel_lookahead *aLookahead;
};

struct acttab{
    struct alookahead al[2];
    struct actel_state *aAction;
    int nAction;
    int nActionAlloc;
    /* for shift and reduce offset range */
    int minTokOffset;
    int maxTokOffset;
    int minNtOffset;
    int maxNtOffset;
};

/* generate.c */
#define LINESIZE 1024
#define YY_TYPE_COUNT 8

struct tpl_member{
    struct global *gp;
    struct map tpl_map;         /* template file rules map */
    struct map type_map;        /* type string corresponding hash map */
    struct acttab action_table;
    FILE *in;
    FILE *out;
    char *line;
    char *temp;
    char *parse_name;
    char *token_type;
    char *nonterminal_type;
    char special_escape;
    int lsize;
    int line_num;
    int *pf;                    /* parse name failure array */
    int tok_hash;
    int nt_hash;
    int array_width;
    char *yy_type[YY_TYPE_COUNT];
};

/* The state of the parser */
struct pstate {
    char *filename;
    char **declargslot;            /* Where the declaration argument should be put */
    int  *declargslot_line;
    char *declkeyword;             /* Keyword of a declaration */
    int maxrhs;                    /* The max number of right hand side symbol array can auto growth */
    struct rqueue rq;              /* Insert action queue */
    int action_index;              /* Insert action increase number */
    int err_count;                 /* Number of errors so far */
    struct global *gp;             /* Global state vector */
    struct symbol *fallback;       /* The fallback token */
    struct symbol *lhs;            /* Left-hand side of current rule */
    char *lhsalias;                /* Alias for the LHS */
    int nrhs;                      /* Number of right-hand side symbols seen */
    struct symbol **rhs;           /* RHS symbols */
    char **rhsalias;               /* Aliases for each RHS symbol (or NULL) */
    struct rule *prevrule;         /* Previous rule parsed */

    enum e_assoc declassoc;        /* Assign this association to decl arguments */
    int preccounter;               /* Assign this precedence to decl arguments */
    struct rule *firstrule;        /* Pointer to first rule in the grammar */
    struct rule *lastrule;         /* Pointer to the most recently parsed rule */
    enum e_state {
        INITIALIZE,                /* state 0 */
        WAITING_DECL_OR_RULE,      /* state 1 */
        WAITING_DECL_KEYWORD,      /* state 2 */
        WAITING_ARROW,             /* state 3 */
        IN_RHS,                    /* state 4 */
        LHS_ALIAS_1,               /* state 5 */
        LHS_ALIAS_2,               /* state 6 */
        LHS_ALIAS_3,               /* state 7 */
        RHS_ALIAS_1,               /* state 8 */
        RHS_ALIAS_2,               /* state 9 */
        PRECEDENCE_MARK_1,         /* state 10 */
        PRECEDENCE_MARK_2,         /* state 11 */
        WAITING_DECL_CODE,         /* state 12 */
        WAITING_DECL_TYPE,         /* state 13 */
        WAITING_IDENTIFIER,        /* state 14 */
        WAITING_TERMINAL,          /* state 15 */
        WAITING_NONTERMINAL,       /* state 16 */
        WAITING_FALLBACK_SYMBOLS,  /* state 17 */
        WAITING_PRECEDENCE_SYMBOLS,/* state 18 */
        WAITING_DESTRUCT_SYMBOLS,  /* state 19 */
        WAITING_TYPE_SYMBOLS,      /* state 20 */
        WAITING_RULE_CODE,         /* state 21 */
        RESYNC_AFTER_ERROR         /* state 22 */
    } state;
};


/************************ From the file "error.c" ********************************/
void Error_fprintf(Error id,...);
void SetErrorStream(FILE *stream);
FILE *GetErrorStream(void);
void PrintFailureNote(int sp_count);
void EmptyFailureNote(int sp_count);

/************************ From the file "output.c" ********************************/
void Output_fprintf(Output id,...);
void SetOutputStream(FILE *stream);
FILE *GetOutputStream(void);

/************************ From the file "option.c" *******************************/
void OptionInit(char **argv, struct option_info *opt, struct macro_define *maDefine, struct option *Op);

/************************ From the file "table.c" ********************************/
void Table_Init(void);

char *Str_find(char *str);
struct symbol *Symbol_find(char *x);
struct state *State_find(struct state *x);
struct config *Config_find(struct config *x);

int Str_insert(char *str);
int Symbol_insert(struct symbol *x);
int State_insert(struct state *x);
int Config_insert(struct config *x);

int Symbol_count(void);
int Str_count(void);
int State_count(void);
int Config_count(void);

void Config_clear(int (*freefunc)());

struct symbol **Symbol_sort_array(void);
void TestStrHashTable(void);
void TestSymbolHashTable(void);

/************************ From the file "tool.c" ********************************/
char *Make_filename(char *name, char *suffix);
int Symbolcmp_1(struct symbol *s1,char *s2);
int Symbolcmp_2(struct symbol *s1,struct symbol *s2);
int Symbolcmp_3(struct symbol *s1,struct symbol *s2);
int Statecmp(struct state *s1, struct state *s2);
int Configcmp(struct config *c1, struct config *c2);
int Actioncmp(struct action *ap1, struct action *ap2);
char *StrSafe(char *str);
struct symbol *Symbol_new(char *x,int type);
struct symbol *GetOrderlySymbols();
void ResetSymbolsIndex(struct symbol *symbols);
void Symbol_arrayof(struct symbol ***sp_array,struct symbol *symbols);
int Terminal_count(struct global *gp);
struct rule *ProcessNoRuleSymbol(struct global *gp, struct symbol *sp);
void SetRluesPrecedence(struct global *gp);
void Reprint(struct global *gp);
void ResetRulesUsed(struct global *gp);
void PrintUsedRules(struct global *gp);
void PrintNoUsedRules(struct global *gp);
void PrintUsedRuleEpsilonSymbols(struct global *gp);
void PrintNonterminalsFirstSet(struct global *gp);
void ConfigPrint(FILE *fp, struct config *cfp);
void SetPrint(FILE *fp, struct set s, struct global *gp);
void PlinkPrint(FILE *fp, struct plink *plp, char *tag);
int  ActionPrint(FILE *fp, struct action *ap,int align);
void Insertitem(char **list, char **next, char *item, int(*comparefunc)());
char *Insertsort(char *list, char **next, int(*comparefunc)());
int Kmpcharcmp(void *pat, void *ob, int a, int b);
int Kmpintegercmp(void *pat, void *ob, int a, int b);
int Kmpcombcmp(void *pat, void *ob, int a, int b);
int Kmpinsertcmp(void *pat, void *ob, int a, int b);
void GetFailure(void *pat, int pat_len, comparefunc cmp ,int *f);
int KMP_find(void *pat, void *ob, int pat_len, int ob_len, comparefunc cmp, int *f, int p);
void InitGlobal(struct global *gp, struct option *popt, struct macro_define *pmac);

/************************ From the file "string.c" ********************************/
void Tplstring_init(struct TplString *pstr, int initsize);
void Tplstring_reset(struct TplString *pstr);
void Tplstring_free(struct TplString *pstr);
void Tplstring_realloc(struct TplString *pstr, int new_size);
void Tplstring_chcat(struct TplString *pstr, int ch);

/************************ From the file "lexer.c" ********************************/
struct token *GetNextToken(void);
struct token *GetToken(void);
void UnGetToken(struct token *tok);
void InitLexer(struct input *in);
void SetScanState(LEX_STATUS s);
void SetPartState(FAILURE_STATUS s);

/************************ From the file "parse.c" ********************************/
void Parse(struct global *gp);

/************************ From the file "queue.c" ********************************/
void InitRingQueue(struct rqueue *rqp, unsigned long ilen, unsigned long size);
void ResetRingQueue(struct rqueue *rqp);
int PopRingQueue(struct rqueue *rqp, void *item);
void PushRingQueue(struct rqueue *rqp, void *item);
unsigned long RingQueueLength(struct rqueue *rqp);
void DeleteRingQueue(struct rqueue *rqp);

/************************ From the file "set.c" ********************************/
void SetNew(struct set *s, int n);
void SetResize(struct set *s, int n);
void SetAdd(struct set *s, int e);
void SetUnion(struct set *s1, struct set s2);
int  SetIsExpand(struct set s1, struct set s2);
void SetFree(struct set *s);

/************************ From the file "ffsets.c" ********************************/
void BuildFirstSets(struct global *gp);
void BuildFollowSets(struct global *gp);

/************************ From the file "state.c" ********************************/
struct state *GetStates();
void BuildLinks_And_ResetStatus(struct global *gp);
void BuildStates(struct global *gp);

/************************ From the file "action.c" ********************************/
void Add_Action(struct action **pap, enum e_action type, struct symbol *sp, char *arg);
void BuildActions(struct global *gp);
void CompressActions(struct global *gp);
void GenerateOutputReport(struct global *gp);

/************************ From the file "map.c" ********************************/
void MapNew(struct map *mp, comparefunc cmp);
struct Pair *MapFind(struct map *mp, void *key);
void MapClear(struct map *mp, pairfreefunc pairfree);
void MapInsert(struct map *mp, struct Pair pair);
void MapRemove(struct map *mp, void *key, pairfreefunc pairfree);
void MapInOrderHelp(struct RBTNode *r);

/************************ From the file "acttab.c" ********************************/
void Initacttab(struct acttab *pActtab);
void Releaseacttab(struct acttab *pActtab);
void ComputeActtab(struct global *gp, struct acttab *pActtab);

/************************ From the file "generate.c" ********************************/
void GenerateParserDriver(struct global *gp);

#endif /* PRIVATE_MATHEW_H_INCLUDED */
