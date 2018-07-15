#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "mathew.h"


static struct option Option;
static struct macro_define maDefine;
static struct option_info options[] = {
    {OPT_FLAG,"a",(char*)&Option.allprintflag,         "������е�����"},
    {OPT_FLAG,"b",(char*)&Option.basisflag,            "ֻ��ӡ������Ŀ"},
    {OPT_FLAG,"c",(char*)&Option.compressflag,         "��ѹ��������"},
    {OPT_FLAG,"d",(char*)&Option.driverflag,           "�����������ļ�"},
    {OPT_FLAG,"e",(char*)&Option.epsilonflag,          "�����ʹ�õĲ���ʽ�����Ƴ���(epsilon)�ķ���"},
    {OPT_FLAG,"f",(char*)&Option.firstsetflag,         "������ս�����׷���"},
    {OPT_FLAG,"h",(char*)&Option.hiddenflag,           "����ʾ����ָʾ��"},
    {OPT_FLAG,"r",(char*)&Option.rpflag,               "���´�ӡ�﷨�ļ�(.y)�Ҳ���ӡ����"},
    {OPT_FLAG,"m",(char*)&Option.mhflag,               "�������ͷ�ļ�"},
    {OPT_FLAG,"n",(char*)&Option.nousedruleflag,       "���δʹ�õĲ���ʽ"},
    {OPT_FLAG,"q",(char*)&Option.qiuetflag,            "������gram.out�ļ�"},
    {OPT_FLAG,"s",(char*)&Option.statesflag,           "����������״̬��Ŀ"},
    {OPT_FLAG,"u",(char*)&Option.usedruleflag,         "�����ʹ�õĲ���ʽ"},
    {OPT_FLAG,"v",(char*)&Option.versionflag,          "����汾��Ϣ"},
    {OPT_STR,"tpl",(char*)&Option.tplfilename,         "ָ��ģ���ļ���"},
    {OPT_INT,"macroinc",(char*)&Option.macroinc,       "������ÿ��Ԥ���ռ��С"},
    {OPT_INT,"bufsize",(char*)&Option.bufsize,         "�ļ��������Ĵ�С"},
    {OPT_FUNC,"echo",(char*)&printf,                   "ԭ������ַ���"},
    {OPT_FLAG,0,0,0}
};

int main(int argc, char **argv)
{
    struct global gl;
    SetErrorStream(stderr);
    SetOutputStream(stdout);
    OptionInit(argv,options,&maDefine,&Option);
    Table_Init();
    InitGlobal(&gl,&Option,&maDefine);
    SYMBOL_NEW("#");
    Parse(&gl);

    if(gl.rule == 0){
        Error_fprintf(GRAMMAR_FILE_WITHOUT_ANY_RULE);
        exit(1);
    }
    if(gl.err_count)Error_fprintf(PARSE_ERR_COUNT,gl.filename,gl.err_count);
    FindStartSymbol(&gl); /* Maybe err_count++ but not parse error count */
    //Symbol_new("[default]",NONTERMINAL);
    gl.symbol_num = Symbol_count();
    gl.symbols = GetOrderlySymbols();
    ResetSymbolsIndex(gl.symbols);

    gl.terminal_num = Terminal_count(&gl);
  
    ResetRulesUsed(&gl);
    BuildFirstSets(&gl);     /* Maybe Add New rule */
    SetRluesPrecedence(&gl);

    /* Sorted symbol serialize */
    gl.symbol_array = (struct symbol **)Mem_alloc(sizeof(struct symbol *)*gl.symbol_num);
    Symbol_arrayof(&gl.symbol_array,gl.symbols);

    if(Option.allprintflag){
        Option.rpflag = 1;
        Option.epsilonflag = 1;
        Option.firstsetflag = 1;
        Option.nousedruleflag = 1;
        Option.statesflag = 1;
        Option.usedruleflag = 1;
    }

    /* Generate a reprint of the grammar, if requested on the command line */
    if(Option.rpflag){
        Reprint(&gl);
    }

    if(Option.epsilonflag){
        PrintUsedRuleEpsilonSymbols(&gl);
    }

    if(Option.firstsetflag){
        PrintNonterminalsFirstSet(&gl);
    }

    if(Option.nousedruleflag){
        PrintUsedRules(&gl);
    }

    if(Option.usedruleflag){
        PrintNoUsedRules(&gl);
    }

    BuildStates(&gl);
    if(Option.statesflag)Output_fprintf(STATE_NUM,gl.state_num);

    gl.states = GetStates();              /* Get orderly states list */
    BuildLinks_And_ResetStatus(&gl);
    BuildFollowSets(&gl);
    BuildActions(&gl);
    if(Option.compressflag == 0){
        CompressActions(&gl);
    }

    if(Option.qiuetflag == 0)GenerateOutputReport(&gl);
    if(Option.driverflag == 0)GenerateParserDriver(&gl);

    if(Option.versionflag)Output_fprintf(VERSION);

    return 0;    
}
