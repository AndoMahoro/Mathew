#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"


static FILE *err = 0;

static char *error_message_format[] = {
   #define ERROR(k, s) s,/* Error info table */
   #include "error.h"
   #undef  ERROR
};

PRIVATE void self_check(void)
{
    int err_count = 0;

    if(!err) err = stderr; /* Can keep output */

    if (strcmp(error_message_format[ERROR_HEAD], "dummy") != 0) {
        fprintf(err,error_message_format[MESSAGE_FORMAT_ERR],ERROR_HEAD);
        err_count++;
    }

    if (strcmp(error_message_format[ERROR_TAIL],
               "dummy") != 0) {
        fprintf(err,error_message_format[MESSAGE_FORMAT_ERR],
                   ERROR_TAIL);
        err_count++;
    }

    if(err_count) exit(1);
}

void SetErrorStream(FILE *stream)
{ 
     err = stream;
}

FILE *GetErrorStream(void)
{
     return err;
}

void PrintFailureNote(int sp_count)
{   /* Failure note */
    if(sp_count < 20){
         Error_fprintf(FAILURE_R_NOTE,sp_count,"");
    }else{
         Error_fprintf(FAILURE_L_NOTE,sp_count-7,"");
    }
}

void EmptyFailureNote(int sp_count)
{
    /* empty failure note */
}

void Error_fprintf(Error id, ...)
{

    self_check();

    if(id <= ERROR_HEAD || id >= ERROR_TAIL) {
        Error_fprintf(ID_OUT_RANGE_ERR);
        return;
    }

    va_list args;
    va_start(args,id);
    vfprintf(err,error_message_format[id],args);
    va_end(args);
}
