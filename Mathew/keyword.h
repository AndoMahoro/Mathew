struct keyword
{
    char *name;
    int len;
    int tok;
};

static struct keyword keywordsA[] =
{
    {"accept", 6, TK_ACCEPT},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsB[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsC[] = 
{
    {"code", 4, TK_CODE},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsD[] =
{
    {"destructor", 10, TK_DESTRUCT},
    {"default_code", 12, TK_DFAULT_CODE},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsE[] =
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsF[] =
{
    {"fallback", 8, TK_FALLBACK},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsG[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsH[] =
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsI[] = 
{
    {"include", 7, TK_INCLUDE},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsJ[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsK[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsL[] = 
{
    {"left", 4, TK_LEFT},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsM[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsN[] = 
{
    {"nonassoc", 8, TK_NONASSOC},
    {"nonterminal_type", 16, TK_NONTERMINAL_TYPE},
    {"nonterminal_destructor", 22, TK_NONTERMINAL_DESTRUCT},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsO[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsP[] = 
{
    {"parse_name", 10, TK_PARSE_NAME},
    {"parse_failed", 12, TK_PARSE_FAILED},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsQ[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsR[] = 
{
    {"right", 5, TK_RIGHT},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsS[] = 
{
    {"start", 5, TK_START_SYMBOL},
    {"syntax_error", 12, TK_SYNTAX_ERROR},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsT[] = 
{
    {"type", 4, TK_TYPE},
    {"token_type", 10, TK_TOKEN_TYPE},
    {"token_destructor", 16, TK_TOKEN_DESTRUCT},
    {"token_prefix", 12, TK_TOKEN_PREFIX},
    {NULL, 0, TK_ID}
};

static struct keyword keywordsU[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsV[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsW[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsX[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsY[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword keywordsZ[] = 
{
    {NULL, 0, TK_ID}
};

static struct keyword *keywords[] =
{
    keywordsA, keywordsB, keywordsC, keywordsD,
    keywordsE, keywordsF, keywordsG, keywordsH,
    keywordsI, keywordsJ, keywordsK, keywordsL,
    keywordsM, keywordsN, keywordsO, keywordsP,
    keywordsQ, keywordsR, keywordsS, keywordsT,
    keywordsU, keywordsV, keywordsW, keywordsX,
    keywordsY, keywordsZ
};

