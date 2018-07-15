#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"


static FILE *output = 0;

static char *output_message_format[] = {
   #define OUTPUT(k, s) s,/* OUTPUT info table */
   #include "output.h"
   #undef  OUTPUT
};

PRIVATE void self_check(void)
{
    int err_count = 0;

    if(!output) output = stdout; /* Can keep output */

    if (strcmp(output_message_format[OUTPUT_HEAD], "dummy") != 0) {
        fprintf(output,output_message_format[OUTPUT_FORMAT_ERR],OUTPUT_HEAD);
        err_count++;
    }

    if (strcmp(output_message_format[OUTPUT_TAIL],
               "dummy") != 0) {
        fprintf(output,output_message_format[OUTPUT_FORMAT_ERR],
                   OUTPUT_TAIL);
        err_count++;
    }

    if(err_count) exit(1);
}

void SetOutputStream(FILE *stream)
{ 
     output = stream;
}

FILE *GetOutputStream(void)
{
     return output;
}

void Output_fprintf(Output id, ...)
{

    self_check();

    if(id <= OUTPUT_HEAD || id >= OUTPUT_TAIL) {
        Output_fprintf(OUTPUT_ID_OUT_RANGE);
        return;
    }

    va_list args;
    va_start(args,id);
    vfprintf(output,output_message_format[id],args);
    va_end(args);
}
