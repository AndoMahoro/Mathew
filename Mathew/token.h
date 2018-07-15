#ifndef TOKEN
#error "You must define TOKEN macro before include this file"
#endif

//end
TOKEN(TK_END,                  "end of input")
//keywords
TOKEN(TK_ACCEPT,               "accept")
TOKEN(TK_CODE,                 "code")
TOKEN(TK_DESTRUCT,             "destructor")
TOKEN(TK_DFAULT_CODE,          "default_code")
TOKEN(TK_FALLBACK,             "fallback")
TOKEN(TK_INCLUDE,              "include")
TOKEN(TK_LEFT,                 "left")
TOKEN(TK_NONASSOC,             "nonassoc")
TOKEN(TK_NONTERMINAL_TYPE,     "nonterminal_type")
TOKEN(TK_NONTERMINAL_DESTRUCT, "nonterminal_destructor")
TOKEN(TK_PARSE_NAME,           "parse_name")
TOKEN(TK_PARSE_FAILED,         "parse_failed")
TOKEN(TK_RIGHT,                "right")
TOKEN(TK_START_SYMBOL,         "start")
TOKEN(TK_SYNTAX_ERROR,         "syntax_error")
TOKEN(TK_TYPE,                 "type")
TOKEN(TK_TOKEN_TYPE,           "token_type")
TOKEN(TK_TOKEN_DESTRUCT,       "token_destructor")
TOKEN(TK_TOKEN_PREFIX,         "token_prefix")

//identifier  
TOKEN(TK_ID,                   "ID")


//operators
TOKEN(TK_DECL,                 "%")
TOKEN(TK_DOT,                  ".")
TOKEN(TK_LPAREN,               "(")
TOKEN(TK_RPAREN,               ")")
TOKEN(TK_LBRACKET,             "[")
TOKEN(TK_RBRACKET,             "]")
TOKEN(TK_ARROW,                "->")

//punctuators
TOKEN(TK_CODE_BLOCK,           "{")
TOKEN(TK_TYPE_BLOCK,           "<")
TOKEN(TK_ALIAS,                "$")

TOKEN(TK_SPECIAL,              "special identifier")
//bad char
TOKEN(TK_BAD,                  "illegal character")
