%out                    <yy_table.java>
%line_start             <1>
%special_escape         <>
%line                   </* #line %d "%s" */\r\n>
%include                <import %s;\r\n>
%include_file           <yy_token.java>
%token                  <    public static final int %s%-20s = %2d;\r\n>
%parse_name             <Parse>
%parse_decl             <\r\n    public static void %s(Object yypParser, int yymajor, %s yyminor);\r\n>
%parse_func_prefix      <    public static void %s(Object yypParser, int yymajor, %s yyminor)\r\n    {\r\n>
%parse_func_minor       <        yyminorunion.yy_%d = yyminor;\r\n>
%parse_func_suffix      <    }\r\n>
%type                   <byte,short,int,long,byte,short,int,long>
%symbol_num             <    static final int  YY_NSYMBOL = %d;\r\n>
%state_num              <    static final int  YY_NSTATE = %d;\r\n>
%rule_num               <    static final int  YY_NRULE = %d;\r\n>
%accept_action          <    static final int  YY_ACCEPT_ACTION = %d;\r\n>
%error_action           <    static final int  YY_ERROR_ACTION = %d;\r\n>
%shift_default_offset   <    static final int  YY_SHIFT_DEFAULT_OFFSET = %d;\r\n>
%reduce_default_offset  <    static final int  YY_REDUCE_DEFAULT_OFFSET = %d;\r\n>
%minor_prefix           <\r\n    static class YY_MINOR_TYPE{\r\n>
%minor_tok_type         <Object>
%minor_nt_type          <Object>
%minor_member           <        public %s yy_%d;\r\n>
%minor_suffix           <    }\r\n\r\n>
%array_width            <8>
%action_prefix          <    /* action table */\r\n    static final %s yy_action[] = {\r\n>
%action_member          <      %d,>
%action_suffix          <    };\r\n>
%state_prefix           <    /* verify state table */\r\n    static final %s yy_state[] = {\r\n>
%state_member           <      %d,>
%state_suffix           <    };\r\n>
%shift_offset_prefix    <    /* shift offset table */\r\n    static final %s yy_shift_offset[] = {\r\n>
%shift_offset_member    <      %d,>
%shift_offset_suffix    <    };\r\n>
%reduce_offset_prefix   <    /* reduce offset table */\r\n    static final %s yy_reduce_offset[] = {\r\n>
%reduce_offset_member   <      %d,>
%reduce_offset_suffix   <    };\r\n>
%default_prefix         <    /* default action table */\r\n    static final %s yy_default[] = {\r\n>
%default_member         <      %d,>
%default_suffix         <    };\r\n>
%fallback_prefix        <    /* fallback table */\r\n    static final %s yy_fallback[] = {\r\n>
%fallback_empty         <      0, /* %s =!> nothing */\r\n>
%fallback_member        <      %d, /* %s =!> %s */\r\n>
%fallback_suffix        <    };\r\n>
%symbol_prefix          <    /* symbol name */\r\n    static final String yy_symbolName[] = {\r\n>
%symbol_member          <      "%s",\r\n>
%symbol_suffix          <    };\r\n>
%rule_prefix            <    /* symbol name */\r\n    static final String yy_ruleName[] = {\r\n>
%rule_member_prefix     <      ">
%rule_member_suffix     <",\r\n>
%rule_suffix            <    };\r\n>
%rule_info_prefix       <    /* rule info for reduce pop stack */\r\n    static final YY_RULE_INFO yyruleInfo[] = {\r\n>
%rule_info_member       <      new YY_RULE_INFO(%d,%d),\r\n>
%rule_info_suffix       <    };\r\n>
%destructor_prefix      <static class yy_destructor_%d implements destructor{\r\n\
    /* destructor function */\r\n\
    @Override\r\n\
    public void destructor_proc(YY_MINOR_TYPE yypminor)\r\n\
    {\r\n>
%destructor_arg         <    yypminor.yy_%d>
%destructor_suffix      <    }\r\n}\r\n>
%destructor_init_prefix <    static void yy_destructor_init()\r\n    {\r\n>
%destructor_init_member <        freeprocs[%d] = new yy_destructor_%d();\r\n>
%destructor_init_suffix <    }\r\n>
%reduce_prefix          <static class yy_reduce_%d implements reduce{\r\n\
    /* reduce function */\r\n\
    @Override\r\n\
    public void reduce_proc(YY_PARSER pParser, YY_MINOR_TYPE yypgotominor, YY_STACK_ENTRY yystack[])\r\n\
    {\r\n>
%reduce_arg_lvalue      <yypgotominor.yy_%d>
%reduce_arg_rvalue      <yystack[pParser.yystack_top + (%d)].yyminor.yy_%d>
%reduce_arg_mvalue      <yystack[pParser.yystack_top + (%d)].yymajor>
%reduce_suffix          <    }\r\n}\r\n>
%reduce_init_prefix     <    static void yy_reduce_init()\r\n    {\r\n>
%reduce_init_member     <        reduceprocs[%d] = new yy_reduce_%d();\r\n>
%reduce_init_suffix     <    }\r\n>
%destructor_array       <    static destructor freeprocs[];\r\n>
%reduce_array           <    static reduce reduceprocs[];\r\n\r\n>
%syntax_error_major     <yymajor>
%syntax_error_minor     <yypminor.yy_%d>
%%
/* copy right 2017 by mathew java template example */

%%
/* include header */
import java.util.*;
import java.io.*;

public class MParse{

    static final int DEBUG = 0;
%%
/* header file prefix */
public class token{
%%

/* header file middle */
%%

}
/* header file suffix*/
%%
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

%%

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

%%
    /* syntax error */
    static void yy_syntax_error(YY_PARSER pParser, int yymajor, YY_MINOR_TYPE yypminor)
    {

%%
    }

%%
    /* parse failed */
    static void yy_parse_failed(YY_PARSER pParser)
    {

        if(DEBUG != 0){
            if(yytraceWriter != null){
                yy_traceFile(yytraceWriter,yytracePrompt + "Parse Failed!\r\n");
            }
        } /* DEBUG */

%%
    }

%%
    /* accept process */
    static void yy_accept(YY_PARSER pParser)
    {

        if(DEBUG != 0){
            if(yytraceWriter != null){
                yy_traceFile(yytraceWriter,yytracePrompt + "Accept!\r\n");
            }
        } /* DEBUG */

        while(pParser.yystack_top >= 0)yy_pop_stack(pParser);
%%
}

%%
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

%%  /* Parse function */
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
%%
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

%%

    /* after extra code */

}
%% /* end */
