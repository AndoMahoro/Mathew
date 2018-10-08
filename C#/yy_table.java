/* copy right 2017 by mathew java template example */

/* #line 3 "gram.y" */

package com;

import java.awt.*;

//#define DEBUG
/* #line 11 "yy_table.java" */
/* include header */
import java.util.*;
import java.io.*;

public class MParse{

    static final int DEBUG = 0;

    static class YY_MINOR_TYPE{
        public int yy_11;
    }

    public static final int PLUS                 =  1;
    public static final int MINUS                =  2;
    public static final int DIVIDE               =  3;
    public static final int TIMES                =  4;
    public static final int INTEGER              =  5;

    static final int  YY_NSYMBOL = 8;
    static final int  YY_NSTATE = 11;
    static final int  YY_NRULE = 6;
    static final int  YY_ACCEPT_ACTION = 17;
    static final int  YY_ERROR_ACTION = 18;
    static final int  YY_SHIFT_DEFAULT_OFFSET = -6;
    static final int  YY_REDUCE_DEFAULT_OFFSET = -6;
    /* data struct info */
    static interface destructor{
        public void destructor_proc(YY_MINOR_TYPE yypminor);
    }

    static BufferedWriter yytraceWriter = null;
    static String yytracePrompt = null;

    static class YY_STACK_ENTRY{
        public int yymajor;
        public int yystate;
        public YY_MINOR_TYPE yyminor;
    }

    static interface reduce{
        public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[]);
    }

    static class YY_PARSER{
        public int yystack_top;
        public int yystack_depth;
        public int yyerr_count;
        public YY_STACK_ENTRY yystack[];
    }

    static class YY_RULE_INFO{
        public YY_RULE_INFO(int l, int n)
        {
            lhs = l;
            nrhs = n;
        }
        public int lhs;
        public int nrhs;
    }

    /* action table */
    static final byte yy_action[] = {
      10,      17,      1,      11,      2,      4,      8,      6,
      10,      3,      8,      6,      10,      5,      8,      6,
      10,      7,      10,      9,
    };
    /* verify state table */
    static final byte yy_state[] = {
      0,      0,      0,      1,      1,      1,      1,      1,
      2,      2,      3,      3,      4,      4,      5,      5,
      6,      6,      8,      8,
    };
    /* shift offset table */
    static final byte yy_shift_offset[] = {
      -5,      3,      3,      7,      7,      11,      11,      -6,
      13,      -6,      -6,
    };
    /* reduce offset table */
    static final byte yy_reduce_offset[] = {
      -5,      -6,      2,      -6,      6,      -6,      10,      -6,
      12,      -6,      -6,
    };
    /* default action table */
    static final byte yy_default[] = {
      18,      18,      18,      12,      18,      13,      18,      14,
      18,      15,      16,
    };
    /* fallback table */
    static final byte yy_fallback[] = {
      0, /* # => nothing */
      0, /* PLUS => nothing */
      0, /* MINUS => nothing */
      0, /* DIVIDE => nothing */
      0, /* TIMES => nothing */
      0, /* INTEGER => nothing */
      0, /* program => nothing */
      0, /* expr => nothing */

      0, /* [default] => nothing */

    };
    /* symbol name */
    static final String yy_symbolName[] = {
      "#",
      "PLUS",
      "MINUS",
      "DIVIDE",
      "TIMES",
      "INTEGER",
      "program",
      "expr",

      "[default]",

    };
    /* symbol name */
    static final String yy_ruleName[] = {
      "program -> expr.",
      "expr -> expr PLUS expr.",
      "expr -> expr MINUS expr.",
      "expr -> expr TIMES expr.",
      "expr -> expr DIVIDE expr.",
      "expr -> INTEGER.",

    };
    /* rule info for reduce pop stack */
    static final YY_RULE_INFO yyruleInfo[] = {
      new YY_RULE_INFO(6,1),
      new YY_RULE_INFO(7,3),
      new YY_RULE_INFO(7,3),
      new YY_RULE_INFO(7,3),
      new YY_RULE_INFO(7,3),
      new YY_RULE_INFO(7,1),

    };
    static destructor freeprocs[];
    static reduce reduceprocs[];

static class yy_destructor_0 implements destructor{
    /* destructor function */
    @Override
    public void destructor_proc(YY_MINOR_TYPE yypminor)
    {
    }
}
static class yy_destructor_1 implements destructor{
    /* destructor function */
    @Override
    public void destructor_proc(YY_MINOR_TYPE yypminor)
    {
    }
}
    static void yy_destructor_init()
    {
        freeprocs[0] = new yy_destructor_0();
        freeprocs[1] = new yy_destructor_0();
        freeprocs[2] = new yy_destructor_0();
        freeprocs[3] = new yy_destructor_0();
        freeprocs[4] = new yy_destructor_0();
        freeprocs[5] = new yy_destructor_0();
        freeprocs[6] = new yy_destructor_1();
        freeprocs[7] = new yy_destructor_1();
        freeprocs[8] = new yy_destructor_1();
    }
static class yy_reduce_0 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 24 "gram.y" */

    System.out.println("Result = " + yystack[pParser.yystack_top + (0)].yyminor.yy_11);

/* #line 185 "yy_table.java" */
    }
}
static class yy_reduce_1 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 28 "gram.y" */

    yypgotominor.yy_11 = yystack[pParser.yystack_top + (-2)].yyminor.yy_11 + yystack[pParser.yystack_top + (0)].yyminor.yy_11;

/* #line 197 "yy_table.java" */
    }
}
static class yy_reduce_2 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 32 "gram.y" */

    yypgotominor.yy_11 = yystack[pParser.yystack_top + (-2)].yyminor.yy_11 - yystack[pParser.yystack_top + (0)].yyminor.yy_11;

/* #line 209 "yy_table.java" */
    }
}
static class yy_reduce_3 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 36 "gram.y" */

    yypgotominor.yy_11 = yystack[pParser.yystack_top + (-2)].yyminor.yy_11 * yystack[pParser.yystack_top + (0)].yyminor.yy_11;

/* #line 221 "yy_table.java" */
    }
}
static class yy_reduce_4 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 40 "gram.y" */

    if(yystack[pParser.yystack_top + (0)].yyminor.yy_11 != 0)
        yypgotominor.yy_11 = yystack[pParser.yystack_top + (-2)].yyminor.yy_11 / yystack[pParser.yystack_top + (0)].yyminor.yy_11;
    else
        System.out.println("Divide by zero!");

/* #line 236 "yy_table.java" */
    }
}
static class yy_reduce_5 implements reduce{
    /* reduce function */
    @Override
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
/* #line 47 "gram.y" */
yypgotominor.yy_11 = yystack[pParser.yystack_top + (0)].yyminor.yy_11;
/* #line 246 "yy_table.java" */
    }
}
    static void yy_reduce_init()
    {
        reduceprocs[0] = new yy_reduce_0();
        reduceprocs[1] = new yy_reduce_1();
        reduceprocs[2] = new yy_reduce_2();
        reduceprocs[3] = new yy_reduce_3();
        reduceprocs[4] = new yy_reduce_4();
        reduceprocs[5] = new yy_reduce_5();
    }

    /* inner part */
    /* trace file */
    static void yy_traceFile(BufferedWriter traceWriter, String traceMsg)
    {
        try {
             traceWriter.write(traceMsg);
        } catch (IOException e) {
             // TODO Auto-generated catch block
             e.printStackTrace();
        }
    }

    /* set process array size and initialize */
    static void init_procsArray()
    {
        int i;

        freeprocs = new destructor[YY_NSYMBOL + 1];
        reduceprocs = new reduce[YY_NRULE];

        for(i = 0; i < YY_NSYMBOL + 1; i++){
            freeprocs[i] = null;
        }

        for(i = 0; i < YY_NRULE; i++){
            reduceprocs[i] = null;
        }
    }

    static String get_symbolName(int index)
    {
        if(index >= 0 && index < yy_symbolName.length){
            return yy_symbolName[index];
        }else{
            return "Unknown";
        }
    }

    static void yy_destructor(int yymajor, YY_MINOR_TYPE yypminor)
    {
        if(freeprocs[yymajor] != null)freeprocs[yymajor].destructor_proc(yypminor);
    }

    static void yy_reduce_proc(YY_PARSER pParser, int yyrule, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])
    {
        if(reduceprocs[yyrule] != null)reduceprocs[yyrule].reduce_proc(pParser,yypgotominor,yystack);
    }

    static int yy_pop_stack(YY_PARSER pParser)
    {
        int yymajor;
        YY_STACK_ENTRY yytop = pParser.yystack[pParser.yystack_top];

        if(pParser.yystack_top < 0)return 0;

        if(DEBUG != 0){
            if(yytraceWriter != null){ /* trace stack */
                yy_traceFile(yytraceWriter, yytracePrompt + "Poping " + get_symbolName(yytop.yymajor) + "\r\n");
            }
        } /* DEBUG */

        yymajor = yytop.yymajor;
        yy_destructor(yymajor,yytop.yyminor); /* free stack top symbol */
        pParser.yystack_top--;
        return yymajor;
    }

    static int yy_find_shift_action(YY_PARSER pParser, int lookahead)
    {
        int i;
        int state = pParser.yystack[pParser.yystack_top].yystate;

        i = yy_shift_offset[state];

        /* default action */
        if(i == YY_SHIFT_DEFAULT_OFFSET){
            return yy_default[state];
        }

        if(lookahead >= YY_NSYMBOL){
            return YY_ERROR_ACTION;
        }

        i += lookahead;

        if(i < 0 || i >= yy_state.length || yy_state[i] != state){
            /* try fallback */
            int fallback;
            if(lookahead > 0 && lookahead < yy_fallback.length && (fallback = yy_fallback[0]) != 0){

            if(DEBUG != 0){
                if(yytraceWriter != null){ /* trace fallback */
                    yy_traceFile(yytraceWriter,yytracePrompt + "Fallback " + get_symbolName(lookahead) + " => " + get_symbolName(fallback) + "\r\n");
                }
            } /* DEBUG */

                return yy_find_shift_action(pParser,fallback);
            }
            return yy_default[state]; /* used default action */
        }else{
            return yy_action[i];
        }

    }

    /* this shift nonterminal is reduce action */
    static int yy_find_reduce_action(int state, int lookahead)
    {
        int i;

        i = yy_reduce_offset[state];

        if(i == YY_REDUCE_DEFAULT_OFFSET){
            return yy_default[state];
        }

        if(lookahead >= YY_NSYMBOL){
            return YY_ERROR_ACTION;
        }

        i += lookahead;

        if(i < 0 || i >= yy_state.length || yy_state[i] != state){
            return yy_default[state];
        }else{
            return yy_action[i];
        }
    }

    static void yy_shift(YY_PARSER pParser, int yynewState, int yymajor, YY_MINOR_TYPE yypminor)
    {
        pParser.yystack_top++;

        if(pParser.yystack_top >= pParser.yystack_depth){

        if(DEBUG != 0){
            if(yytraceWriter != null){ /* trace stack overflow */
                yy_traceFile(yytraceWriter,yytracePrompt + "Stack Overflow ! Try realloc stack.\r\n");
            }
        } /* DEBUG */
            YY_STACK_ENTRY yystack[] = pParser.yystack;
            pParser.yystack = new YY_STACK_ENTRY[pParser.yystack_depth * 2];
            /* move to new stack */
            for(int i = 0; i < pParser.yystack_depth; i++){
                pParser.yystack[i] = yystack[i];
            }
            pParser.yystack_depth *= 2;
        }
        /* entry stack */

        pParser.yystack[pParser.yystack_top] = new YY_STACK_ENTRY();
        pParser.yystack[pParser.yystack_top].yystate = yynewState;
        pParser.yystack[pParser.yystack_top].yymajor = yymajor;
        pParser.yystack[pParser.yystack_top].yyminor = yypminor;

        if(DEBUG != 0){
            if(yytraceWriter != null && pParser.yystack_top > 0){
                int i;
                yy_traceFile(yytraceWriter,yytracePrompt + "Shift " + get_symbolName(yymajor) + " => State(" + yynewState + ")\r\n");
                yy_traceFile(yytraceWriter,yytracePrompt + "Stack:");
                for(i = 0; i <= pParser.yystack_top; i++)yy_traceFile(yytraceWriter," " + get_symbolName(pParser.yystack[i].yymajor));
                yy_traceFile(yytraceWriter,"\r\n");
           }
        } /* DEBUG */

    }

    static void yy_reduce(YY_PARSER pParser, int yyrule)
    {
        int yygoto;
        int yyaction;
        int yysize;

        YY_MINOR_TYPE yygotominor = new YY_MINOR_TYPE();

        if(DEBUG != 0){
            if(yytraceWriter != null && yyrule >= 0 && yyrule < yy_ruleName.length){ /* missing realloc process or realloc stack failure */
                yy_traceFile(yytraceWriter,yytracePrompt + "Reduce [" + yy_ruleName[yyrule] + "]\r\n");
            }
        } /* DEBUG */

        yy_reduce_proc(pParser,yyrule,yygotominor,pParser.yystack); /* invoke reduce code */

        yygoto = yyruleInfo[yyrule].lhs;
        yysize = yyruleInfo[yyrule].nrhs;

        yyaction = yy_find_reduce_action(pParser.yystack[pParser.yystack_top - yysize].yystate,yygoto);
        pParser.yystack_top -= yysize;

        if(yyaction < YY_NSTATE){/* nonterminal shift so yyaction need less state number */

        if(DEBUG != 0){
            /* if not debugging direct shift action improve performance */
            pParser.yystack_top++;
            pParser.yystack[pParser.yystack_top].yystate = yyaction;
            pParser.yystack[pParser.yystack_top].yymajor = yygoto;
            pParser.yystack[pParser.yystack_top].yyminor = yygotominor;
        } /* DEBUG */
            else
            {
                yy_shift(pParser,yyaction,yygoto,yygotominor);
            }
        }else if(yyaction == YY_ACCEPT_ACTION){
            yy_accept(pParser);
        }
    }

    /* syntax error */
    static void yy_syntax_error(YY_PARSER pParser, int yymajor, YY_MINOR_TYPE yypminor)
    {

/* #line 15 "gram.y" */

    System.out.println("Syntax Error! \n By Shift " + yymajor + " value(" + yypminor.yy_11 + ")");

/* #line 474 "yy_table.java" */
    }

    /* parse failed */
    static void yy_parse_failed(YY_PARSER pParser)
    {

        if(DEBUG != 0){
            if(yytraceWriter != null){
                yy_traceFile(yytraceWriter,yytracePrompt + "Parse Failed!\r\n");
            }
        } /* DEBUG */

/* #line 19 "gram.y" */

    System.out.println("Parse Failed!");
    System.exit(1);
/* #line 491 "yy_table.java" */
    }

    /* accept process */
    static void yy_accept(YY_PARSER pParser)
    {

        if(DEBUG != 0){
            if(yytraceWriter != null){
                yy_traceFile(yytraceWriter,yytracePrompt + "Accept!\r\n");
            }
        } /* DEBUG */

        while(pParser.yystack_top >= 0)yy_pop_stack(pParser);
}

    /* interface part */
    /* Initialize trace configuration */
    public static void ParseTrace(BufferedWriter traceWriter, String tracePrompt)
    {
        yytraceWriter = traceWriter;
        yytracePrompt = tracePrompt;
    }

    public static Object ParseAlloc()
    {
        YY_PARSER pParser;
        int yystack_depth = 100;

        init_procsArray();

        /* init process */
        yy_destructor_init();
        yy_reduce_init();

        pParser = new YY_PARSER();

        pParser.yystack_top = -1;
        pParser.yystack = new YY_STACK_ENTRY[yystack_depth];
        pParser.yystack_depth = yystack_depth;
        pParser.yyerr_count = 0;

        return pParser;
    }

    public static void ParseFree(Object p)
    {
        YY_PARSER pParser = (YY_PARSER)p;
        if(pParser == null)return;
        while(pParser.yystack_top >= 0)yy_pop_stack(pParser);
        pParser = null;
    }

    public static void Parse(Object yypParser, int yymajor, int yyminor)
    {
        YY_MINOR_TYPE yyminorunion = new YY_MINOR_TYPE();
        YY_PARSER pParser;
        boolean yyendofinput;
        int yyaction;

        pParser = (YY_PARSER)yypParser;

        /* initialize */
        if(pParser.yystack_top < 0){
            pParser.yystack_top = 0;
            pParser.yyerr_count = -1;
            pParser.yystack[0] = new  YY_STACK_ENTRY();
            pParser.yystack[0].yymajor = 0; /* '#' */
            pParser.yystack[0].yystate = 0; /* state number */
        }
        /* yyminorunion.yy_%d = yyminor; */
        yyminorunion.yy_11 = yyminor;
        yyendofinput = (yymajor == 0);

        if(DEBUG != 0){
            if(yytraceWriter != null){
                yy_traceFile(yytraceWriter,yytracePrompt + "Input " + get_symbolName(yymajor) + "\r\n");
            }
        } /* DEBUG */

        do{
            yyaction = yy_find_shift_action(pParser,yymajor);

            if(yyaction < YY_NSTATE){
                yy_shift(pParser,yyaction,yymajor,yyminorunion);
                pParser.yyerr_count--;
                if(yyendofinput == true && pParser.yystack_top >= 0){
                    yymajor = 0;
                }else{
                    yymajor = YY_NSYMBOL; /* end of loop flag for shift after reduce action */
                }
            }else if(yyaction < YY_NSTATE + YY_NRULE){
                yy_reduce(pParser,yyaction - YY_NSTATE); /* shift nt for next loop yy_find_shift_action find lookahead(yymajor) reduce rule */
            }else if(yyaction == YY_ERROR_ACTION){

                if(DEBUG != 0){
                    if(yytraceWriter != null){
                        yy_traceFile(yytraceWriter,yytracePrompt + "Syntax Error!\r\n");
                    }
                } /* DEBUG */

                if(pParser.yyerr_count <= 0){
                    yy_syntax_error(pParser,yymajor,yyminorunion);
                }
                pParser.yyerr_count = 3; /* 3 times error only show 1 time */
                yy_destructor(yymajor,yyminorunion); /* drop the cause of the error token(yymajor) */

                if(yyendofinput == true){
                    yy_parse_failed(pParser);
                }
                yymajor = YY_NSYMBOL;

            }else{
                yy_accept(pParser);
                yymajor = YY_NSYMBOL;
            }

        }while(yymajor != YY_NSYMBOL && pParser.yystack_top >= 0);

    }
/* #line 49 "gram.y" */

    public static void main(String[] args)
    {
        Object pParser = ParseAlloc();
        Parse(pParser,INTEGER,5);
        Parse(pParser,PLUS,0);
        Parse(pParser,INTEGER,6);
        Parse(pParser,TIMES,0);
        Parse(pParser,INTEGER,3);
        Parse(pParser,0,0);
        ParseFree(pParser);
    }
/* #line 624 "yy_table.java" */

    /* after extra code */

}
