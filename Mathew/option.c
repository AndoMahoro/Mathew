#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"


PRIVATE int findlabel(char *a,struct option_info *opt)
{
    int i;
    for(i=0; opt[i].label; i++)
    {
        if(strcmp(a,opt[i].label)==0)break;
    }
    return i;
}

PRIVATE void err_line(char **argv, char *a, int k){
    int sp_count,i;
    
    sp_count = 0;
    /* First half of the argv */
    for(i = 0; argv[i] && strcmp(argv[i],a); i++){
        fprintf(GetErrorStream(),"%s ",argv[i]);
        sp_count += strlen(argv[i]) + 1;
    }
    sp_count += k;
    /* Second half of the argv */
    for(; argv[i]; i++)fprintf(GetErrorStream(),"%s ",argv[i]);

    /* Failure note */
    PrintFailureNote(sp_count);

}


PRIVATE int optionflags(char *a,struct option_info *opt, struct option *Op)
{
    int err_count = 0;
    int i;

    i = findlabel(&a[1],opt);
    if(opt[i].label==0){
        Error_fprintf(UNDEF_OPT);
        err_line(Op->argv,a,1);
        err_count++;
    }else if(opt[i].type == OPT_FLAG){
         *((int*)opt[i].arg) = 1;
    }else{
        Error_fprintf(MISSING_FLAG_ON_SWITCH);
        err_line(Op->argv,a,1);
        err_count++;
    }

    return err_count;
}

PRIVATE int optionvalues(char *a,struct option_info *opt,struct option *Op)
{
    union opt_value value;
    char *end;
    char *cp;
    int err_count = 0;
    int i;

    cp = strchr(a,'=');
    *cp = 0;
    i = findlabel(a,opt);
    *cp = '=';
    if(opt[i].label==0){
        Error_fprintf(UNDEF_OPT);
        err_line(Op->argv,a,0);
        err_count++;
    }else{
        cp++;
        if(opt[i].type==OPT_INT){
            /* Assignment integer value */
            value.iv = strtol(cp,&end,10);
            if(*end){
                Error_fprintf(ILLEGAL_CHAR_IN_INTEGER_ARG);
                err_line(Op->argv,a,(unsigned long)end-(unsigned long)a);
                err_count++;
            }else{
                *(int*)(opt[i].arg) = value.iv;
            }
        }else if(opt[i].type==OPT_DBL){
            /* Assignment double value */
            value.dv = strtod(cp,&end);
            if(*end){
                Error_fprintf(ILLEGAL_CHAR_IN_DOUBLE_ARG);
                err_line(Op->argv,a,(unsigned long)end-(unsigned long)a);
                err_count++;
            }else{
                 *(double*)(opt[i].arg) = value.dv;
            }
        }else if(opt[i].type==OPT_STR){
            /* Assignment string value */
            value.sv = (char*)Mem_alloc(strlen(cp)+1);
            MEM_CHECK(value.sv);
            strcpy(value.sv,cp);
            *(char**)(opt[i].arg) = value.sv;
        }else{
            Error_fprintf(MISSING_VALUE_ON_SWITCH);
            err_line(Op->argv,a,0);
            err_count++;
        }

    }

    return err_count;
}


PRIVATE int optionfuncs(char *a,struct option_info *opt, struct option *Op)
{
    char *end;
    char *cp;

    int err_count = 0;
    int i;

    cp = strchr(a,'(');
    *cp = 0;
    i = findlabel(a,opt);
    *cp = '(';
    if(opt[i].label == 0){
        Error_fprintf(UNDEF_OPT);
        err_line(Op->argv,a,0);
        err_count++;
    }else{
         cp++;
         if(opt[i].type == OPT_FUNC){
            end = cp + strlen(cp) - 1;
            if(*end == ')'){
                *end = 0;
                (*(void(*)())(opt[i].arg))(cp); /* Call option function */
                *end = ')';
            }else{
                Error_fprintf(MISSING_RPAREN);
                err_line(Op->argv,a,strlen(a));
                err_count++;
            }

         }else{
             Error_fprintf(MISSING_FUNC_ON_SWITCH);
             err_line(Op->argv,a,0);
             err_count++;
         }
    }
    return err_count;
}



PRIVATE void optionmacros(char *a, struct macro_define *maDefine,struct option *Op)
{
    char **pma;

    if(maDefine->count >= maDefine->size)
    {
        maDefine->size += Op->macroinc > 0 ? Op->macroinc : 1; /* Least one increment */
        maDefine->def_array = (char**)Mem_relloc(maDefine->def_array,sizeof(maDefine->def_array[0])*maDefine->size);
        MEM_CHECK(maDefine->def_array);
    }
    pma = (char**)&maDefine->def_array[maDefine->count];
    *pma = (char*)Mem_alloc(strlen(a)+1);

    MEM_CHECK(*pma);

    strcpy(*pma,a);
    *(*pma + strlen(a)) = 0;

    maDefine->count++;

}

PRIVATE void OptionPrint(struct option_info *opt)
{
    int i;
    int len,max_len;

    len = 0;
    max_len = 0;

    for(i = 0; opt[i].label; i++){
        len = strlen(opt[i].label);
        if(opt[i].type == OPT_INT){
            len += 10;  /* Length of "=<integer>" */
        }else if(opt[i].type == OPT_DBL){
            len += 9;   /* Length of "=<double>" */
        }else if(opt[i].type == OPT_STR){
            len += 9;   /* Length of "=<string>" */
        }else if(opt[i].type == OPT_FUNC){
            len += 10;  /* Length of "(<string>)" */
        }
        if(len > max_len) max_len = len;
    }

    for(i=0; opt[i].label; i++){
        if(opt[i].type == OPT_FLAG){
            fprintf(GetErrorStream(),"   -%-*s   %s\n",max_len,opt[i].label,opt[i].message);
        }else if(opt[i].type == OPT_INT){
            fprintf(GetErrorStream(),"   %s=<integer>%*s   %s\n",opt[i].label,max_len - strlen(opt[i].label) - 9,"",opt[i].message);
        }else if(opt[i].type == OPT_DBL){
            fprintf(GetErrorStream(),"   %s=<double>%*s   %s\n",opt[i].label,max_len - strlen(opt[i].label) - 8,"",opt[i].message);
        }else if(opt[i].type == OPT_STR){
            fprintf(GetErrorStream(),"   %s=<string>%*s   %s\n",opt[i].label,max_len - strlen(opt[i].label) - 8,"",opt[i].message);
        }else if(opt[i].type == OPT_FUNC){
            fprintf(GetErrorStream(),"   %s(<string>)%*s   %s\n",opt[i].label,max_len - strlen(opt[i].label) - 9,"",opt[i].message);
        }
    }

}


void OptionInit(char **argv, struct option_info *opt, struct macro_define *maDefine, struct option *Op)
{
   int err_count;

   err_count = 0;
   Op->argv = argv;
   
 
   if(argv && *argv && opt){      /* argv[0] is program name methew or mathew.exe */
       if(argv[1]){
          int i;
          if(access(argv[1],4)){  /* Check the file of exist and check read authority*/
              Error_fprintf(FILE_NOT_EXIST,argv[1]);
              exit(1);
          }
          Op->filename = (char*)Mem_alloc(strlen(argv[1])+1);
          MEM_CHECK(Op->filename);
          strcpy(Op->filename,argv[1]);
          for(i=2; argv[i]; i++){
             if(argv[i][0]=='-'){
                 err_count += optionflags(argv[i],opt,Op);
             }else if(strchr(argv[i],'=')){/* '=' precede than '('*/
                 err_count += optionvalues(argv[i],opt,Op);
             }else if(strchr(argv[i],'(')){
                 err_count += optionfuncs(argv[i],opt,Op);
             }else{
                 optionmacros(argv[i],maDefine,Op); /* Others input we are set in macro set */
             }
          }
       }else{
           Error_fprintf(MISSING_GRAMMAR_FILE);
           err_line(argv,argv[0],strlen(argv[0])+1);
           err_count++;
       }
   }

   if(err_count>0){
       Error_fprintf(VALID_COMMAND_OPT_ARE);
       OptionPrint(opt);
       exit(1);
   }
}


