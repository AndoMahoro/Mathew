#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "mathew.h"


#define ACTTAB_SIZE(x) ((x).nAction)
#define ACTTAB_YYACTION(x,n) ((x).aAction[n].action)
#define ACTTAB_YYSTATE(x,n) ((x).aAction[n].state)


PRIVATE int file_exists(char *filename)
{
    return (access(filename,0) == 0);
}

PRIVATE char *envsearch(char *filename)
{
    char *cp, *ep, *format, *path, *pathlist;
    char c, specer;
    pathlist = getenv("PATH");

#ifdef __WIN32__
    specer = ';';
    format = "%s\\%s";
#else
    specer = ':';
    format = "%s/%s";
#endif

    if(pathlist == 0)pathlist = ".:/bin:/usr/bin"; /* Unix/Linux */
    path = Mem_alloc(strlen(pathlist) + strlen(filename) + 1);
    while(*pathlist){
        ep = strchr(pathlist,specer);
        if(ep == 0){
            ep = &pathlist[strlen(pathlist)];
        }

        /* Trim prefix and sufix space char file_exists need trim space char */
        while(isspace(*pathlist))pathlist++; /* Trim prefix space char */
        cp = ep;
        while(isspace(*--cp));               /* Turn back find not space char */
        cp++;                                /* Not space next char */
        c = *cp;
        *cp = 0;
        if(*(cp-1) == '\\' || *(cp-1) == '/')
            sprintf(path,"%s%s",pathlist,filename);
        else
            sprintf(path,format,pathlist,filename);
        *cp = c;   /* if cp == ep need reback *cp */
        if(*ep == 0){
            pathlist = "";
        }else{
            pathlist = &ep[1];
        }
        if(file_exists(path))break;
    }
    return path;
}

PRIVATE FILE *tpl_open(struct global *gp)
{
    char *specified_name = gp->opt->tplfilename;
    char *default_name_1 = Make_filename(gp->filename,".tpl"); /* Filename include full path */
    char *default_name_2 = "template.tpl";

    char *tplname;
    FILE *fp;

    tplname = 0;

    if(file_exists(specified_name)){ /* First find gram.tpl file */
        tplname = specified_name;
    }else if(file_exists(default_name_1)){
        tplname = default_name_1;
    }else if(file_exists(default_name_2)){
        tplname = default_name_2;
    }else{
        /* Environment search template.tpl */
        tplname = envsearch(default_name_2);
    }

    if(tplname == 0){
        Error_fprintf(CANNOT_FIND_TEMPLATE_FILE);
        gp->err_count++;
        return 0;
    }

    fp = fopen(tplname,"rb");
    if(fp == 0){
        Error_fprintf(CANNOT_OPEN_TEMPLATE_FILE,tplname);
        gp->err_count++;
        exit(1);
        return 0;
    }
    gp->tplname = StrSafe(tplname);
    return fp;
}

PRIVATE tpl_fprintf(struct tpl_member *tplpvar, char *format, ...)
{
    int i , j;
    va_list args;

    for(i = 0, j = 0; format[i]; i++, j++){
        if(format[i] == '\\'){
            if(format[i+1] == 'a'){
                tplpvar->temp[j] = '\a';
            }else if(format[i+1] == 'b'){
                tplpvar->temp[j] = '\b';
            }else if(format[i+1] == 'f'){
                tplpvar->temp[j] = '\f';
            }else if(format[i+1] == 'n'){
                tplpvar->temp[j] = '\n';
                tplpvar->line_num++;
            }else if(format[i+1] == 'r'){
                tplpvar->temp[j] = '\r';
            }else if(format[i+1] == 't'){
                tplpvar->temp[j] = '\t';
            }else if(format[i+1] == 'v'){
                tplpvar->temp[j] = '\v';
            }else if(format[i+1] == '\\'){
                tplpvar->temp[j] = '\\';
            }else if(format[i+1] == '\''){
                tplpvar->temp[j] = '\'';
            }else if(format[i+1] == '\"'){
                tplpvar->temp[j] = '\"';
            }else if(format[i+1] == '?'){
                tplpvar->temp[j] = '\?';
            }else if(format[i+1] == '0'){
                tplpvar->temp[j] = '\0';
            }else{
                tplpvar->temp[j] = '\\';
                i--;
            }
            i++;
        }else{
            tplpvar->temp[j] = format[i];
        }
    }

    tplpvar->temp[j] = 0;
    va_start(args,format);
    if(tplpvar->temp[0])vfprintf(tplpvar->out,tplpvar->temp,args);  /* if "" == '\0' not write any thing */
    va_end(args);
}

/* String cat perhaps change dest string size */
PRIVATE void tpl_strcat(char **line, char *str, int llen, int *lsize)
{
    if(llen >= *lsize){
        *lsize = llen + 1;
        *line = (char*)Mem_relloc(*line,(*lsize) *sizeof(char));
        MEM_CHECK(*line);
    }
    strcat(*line,str);
}

/* Separation segment read line every segment len is LINESIZE dynamic hover in max lsize*/
PRIVATE char *tpl_fgetline(FILE *in, char **line, int *lsize, int *llen)
{
    int len;
    BOOL is_continue;
    char *cp, str[LINESIZE];
    (*line)[0] = 0;
    *llen = 0;
    do{
        if(!fgets(str,LINESIZE,in))return 0;
        len = 0;
        cp = str;
        is_continue = TRUE;
        while(cp[len]){
            if(cp[len] == '!'){
                len++; /* jump !\ */
            }else if(cp[len] == '\\'){
                if(isspace(cp[len + 1])){
                    cp[len] = 0; /* drop '\' */
                    break; /* line continue */
                }else{
                    len++; /* jump special char \a \b \\ ... */
                }

            }

            if(cp[len] == '\n'){
                is_continue = FALSE; /* !\n */
            }
            len++;
        }

        *llen += len;
        tpl_strcat(line,str,*llen,lsize);
    }while(is_continue);

    return *line;
}

PRIVATE char *minimum_size_type(struct tpl_member *tplpvar, int low, int up)
{
    if(low >= 0){
        if(up < 256){
            return tplpvar->yy_type[0];  /* unsigened char */
        }else if(up < 65536){
            return tplpvar->yy_type[1];  /* unsigened short */
        }else if(up < 4294967296){
            return tplpvar->yy_type[2];  /* unsigened int */
        }else{
            return tplpvar->yy_type[3];  /* unsigened long */
        }
    }else if(low >= -127 && up <= 127){
        return tplpvar->yy_type[4];      /* char */
    }else if(low >= -32767 && up <= 32767){
        return tplpvar->yy_type[5];      /* short */
    }else if(low >= -2147483647 && up <= 2147483647){
        return tplpvar->yy_type[6];      /* int */
    }else{
        return tplpvar->yy_type[7];      /* long */
    }
}

/* extract separate by ',' transferred meaning is '!' */
PRIVATE int set_name(char *temp, char **str, char **slot)
{
    int i, len = 0;

    for(i = 0; (*str)[i] && (*str)[i] != ','; i++){
        if((*str)[i] == '!'){                 /* !(ch) only read ch !, read , !! read ! */
            temp[len++] = (*str)[++i];        /* used loop final i++ jump > */
            if((*str)[i] == 0)break;          /* end of file pre is '!' */
        }
        else
            temp[len++] = (*str)[i];
    }

    (*str) += i;
    temp[len] = 0;
    *slot = StrSafe(temp);

    if((**str) == 0) return 1;
    (*str)++;                                 /* jump ',' */

    return 0;
}

PRIVATE void tpl_init_type(struct tpl_member *tplpvar)
{
    struct Pair *p;

    p = MapFind(&tplpvar->tpl_map,"%type");
    /* init type array */
    if(p){
        int i;
        char *name;
        char *cp = p->value;
        for(i = 0; i < YY_TYPE_COUNT; i++){
            set_name(tplpvar->temp,&cp,&name);  /* if new_name == null we will save it some language no type */
            tplpvar->yy_type[i] = name;
        }
    }
}

PRIVATE void tpl_init_replace(struct tpl_member *tplpvar)
{
    int len;
    struct Pair *p;

    p = MapFind(&tplpvar->tpl_map,"%parse_name");

    /* init parse name and calculate failure array */
    if(p && (len = strlen(p->value)) > 0){
        tplpvar->parse_name = p->value;
        tplpvar->pf = (int*)Mem_alloc(sizeof(int)*len);        /* As the failure function value array f space distribution */
        MEM_CHECK(tplpvar->pf);
        GetFailure(tplpvar->parse_name, len, Kmpcharcmp, tplpvar->pf);
    }

}

/* single pair arg repalce transfer by space start */
PRIVATE void sigtransfer(struct tpl_member *tplpvar, char *old_name, char *new_name, int *f)
{
    int istart, olen , llen;
    olen = strlen(old_name);

    while(tpl_fgetline(tplpvar->in,&tplpvar->line,&tplpvar->lsize,&llen) &&
          (tplpvar->line[0] != '%'|| tplpvar->line[1] != '%')){
        istart = 0;
        tplpvar->line_num++;
        if(old_name && new_name && old_name[0]){
            int i;
            /* i == 0 cut for i - 1 = -1 */
            for(i = 0; (i = KMP_find(old_name,tplpvar->line,olen,llen,Kmpcharcmp,f,istart)) != -1 &&
                ((i == 0) || isspace(tplpvar->line[i-1])); istart = i + olen){
                if(i > istart && tplpvar->out)fprintf(tplpvar->out,"%.*s", i - istart, &tplpvar->line[istart]);
                if(new_name[0] && tplpvar->out)fprintf(tplpvar->out,"%s",new_name);
            }
        }
        if(tplpvar->out)fprintf(tplpvar->out,"%s", &tplpvar->line[istart]);
    }

}

/* transfer original text if find specified name replace it by space start */
PRIVATE void tpl_transfer(struct tpl_member *tplpvar)
{
    sigtransfer(tplpvar,tplpvar->parse_name,tplpvar->gp->parse_name,tplpvar->pf);
}

PRIVATE void tpl_linedir(struct tpl_member *tplpvar, int line, char *filename)
{
    struct Pair *result;
    result = MapFind(&tplpvar->tpl_map,"%line");
    if(result){                                           /* %line <> perhaps "" value[0] = 0 */
        tpl_fprintf(tplpvar,result->value,line,filename); /* fprintf(out," ",a,b) only print " " ,"" is nothing print */
    }
}

/* */
PRIVATE void tpl_ycode(struct tpl_member *tplpvar, char *str, int str_line)
{
    if(str == 0)return;
    tpl_linedir(tplpvar,str_line,tplpvar->gp->filename);

    while(*str){
        if(*str == '\n'){
            tplpvar->line_num++;
        }
        putc(*str,tplpvar->out);
        str++;
    }

    if(str[-1] != '\n'){
        putc('\r',tplpvar->out);
        putc('\n',tplpvar->out);   /* Increase '\n' */
        tplpvar->line_num++;
    }

    tpl_linedir(tplpvar,tplpvar->line_num + 1,tplpvar->gp->outname); /* Redirect #LINE */

}

PRIVATE void tpl_initmap(struct tpl_member *tplpvar)
{
     struct Pair pair;
     char ch;
     int i, llen, vlen, vsize;
     char *value;

     /* initialize value */
     value = (char*)Mem_alloc(sizeof(char)*(tplpvar->lsize));
     MEM_CHECK(value);
     vsize = tplpvar->lsize;

     while(tpl_fgetline(tplpvar->in,&tplpvar->line,&tplpvar->lsize,&llen) &&
           (tplpvar->line[0] != '%'|| tplpvar->line[1] != '%')){
         if(tplpvar->line[0] != '%')continue;
         if(tplpvar->lsize > vsize){
             vsize = tplpvar->lsize;
             value = (char*)Mem_relloc(value,sizeof(char)*vsize);
             MEM_CHECK(value);
         }

         for(i = 0; !isspace(tplpvar->line[i]); i++);               /* find ' ' */
         ch = tplpvar->line[i];
         tplpvar->line[i] = 0;
         pair.key = StrSafe(tplpvar->line);
         tplpvar->line[i] = ch;

         for(; tplpvar->line[i] && tplpvar->line[i-1] != '<'; i++); /* find '<' and jump it */
         vlen = 0;
         for(; tplpvar->line[i] && tplpvar->line[i] != '>'; i++){
             if(tplpvar->line[i] == '!'){                           /* !(ch) only read ch !> read > !! read ! */
                 value[vlen++] = tplpvar->line[++i];                /* used loop final i++ jump > */
                 if(tplpvar->line[i] == 0)break;                    /* end of file pre is '!' */
             }
             else
                 value[vlen++] = tplpvar->line[i];
         }
         value[vlen] = 0;
         pair.value = StrSafe(value);                   /* if <> value = "" or %type .... \n value = "" */
         MapInsert(&tplpvar->tpl_map,pair);
     }

     Mem_free(value);
}

PRIVATE int add_type(char *temp, struct symbol *sp, char *type, char **types, int array_size)
{
    int i, hash;

    i = 0;
    hash = 0;

    /* if (void \n *) type line_num will record it see tpl_fprintf */
    while(isspace(*type))type++;
    while(*type)temp[i++] = *type++;
    while(i > 0 && isspace(temp[i-1]))i--;
    temp[i] = 0;

    /* confirm not "" */
    if(temp[0] == 0){temp[0] = ' ';  temp[1] = 0;}

    for(i = 0; temp[i]; i++){
        hash = hash * 53 + temp[i];
    }
    hash = (hash & 0x7fffffff) % array_size;
    while(types[hash]){
        if(strcmp(types[hash],temp) == 0){
            if(sp)sp->dtnum = hash;
            break;
        }
        hash++;
        if(hash >= array_size)hash = 0;
    }
    if(types[hash] == 0){
        if(sp)sp->dtnum = hash;
        types[hash] = StrSafe(temp);
    }
    return hash;
}

PRIVATE void tpl_minor_struct(struct tpl_member *tplpvar)
{
    struct symbol *sp;
    struct Pair *p;
    struct Pair pair;
    char *cp, **types;
    int i, tok_hash, nt_hash;
    int array_size = tplpvar->gp->symbol_num * 2;
    types = (char**)Mem_alloc(sizeof(char*)*array_size);
    MEM_CHECK(types);
    for(i = 0; i < array_size; i++) types[i] = 0;

    /* token type */
    if(tplpvar->gp->token_type){
        cp = tplpvar->gp->token_type;
    }else{
        p = MapFind(&tplpvar->tpl_map,"%minor_tok_type");
        if(p)cp = p->value;
        else cp = " ";
    }
    tplpvar->token_type = cp; /* <  tokenType * > not influence in Parse process as ", tokenType * yypminor);" */
    tok_hash = add_type(tplpvar->temp,0,cp,types,array_size);

    /* nonterminal type */
    if(tplpvar->gp->nonterminal_type){
        cp = tplpvar->gp->nonterminal_type;
    }else{
        p = MapFind(&tplpvar->tpl_map,"%minor_nt_type");
        if(p)cp = p->value;
        else cp = " ";
    }
    tplpvar->nonterminal_type = cp;
    nt_hash = add_type(tplpvar->temp,0,cp,types,array_size);

    /* for destructor */
    tplpvar->tok_hash = tok_hash;
    tplpvar->nt_hash = nt_hash;

    for(sp = tplpvar->gp->symbols; sp; sp = sp->next){
        cp = sp->datatype;
        /* confirm default type */
        if(cp == 0){
            sp->dtnum = sp->type == TERMINAL ? tok_hash : nt_hash;
            continue;
        }
        add_type(tplpvar->temp,sp,cp,types,array_size);
    }

    p = MapFind(&tplpvar->tpl_map,"%minor_prefix");
    if(p)tpl_fprintf(tplpvar,p->value);

    p = MapFind(&tplpvar->tpl_map,"%minor_member");
    if(p){
        for(i = 0; i < array_size; i++){
            if(types[i]){
                tpl_fprintf(tplpvar,p->value,types[i],i);
                pair.key = types[i];
                pair.num = i;
                MapInsert(&tplpvar->type_map,pair);
            }
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%minor_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);

    Mem_free(types);
}

PRIVATE void tokens_print(struct tpl_member *tplpvar, char *format, char *prefix)
{
    struct symbol *sp = tplpvar->gp->symbols->next;

    while(sp && (sp->type == TERMINAL)){
        tpl_fprintf(tplpvar,format,prefix,sp->name,sp->index);
        sp = sp->next;
    }
}

PRIVATE void tpl_include(struct tpl_member *tplpvar)
{
    int i, line_num;
    FILE *out;
    char *prefix, *include_name;
    struct Pair *p;

    if(tplpvar->gp->token_prefix)
        prefix = tplpvar->gp->token_prefix;
    else
        prefix = "";

    if(tplpvar->gp->opt->mhflag && (p = MapFind(&tplpvar->tpl_map,"%include_file"))){
        include_name = (char*)p->value;
        FILE *include = fopen(include_name,"wb");

        if(!include){
            include_name = Make_filename(tplpvar->gp->filename,".inc");
            include = fopen(include_name,"wb");
            Error_fprintf(INVALID_TPL_INCLUDE_NAME,tplpvar->gp->tplname,p->value,include_name);
        }

        p = MapFind(&tplpvar->tpl_map,"%include");
        if(p)tpl_fprintf(tplpvar,p->value,include_name);


        /* Do not calculate line exchange out file
        ** include file header */
        line_num = tplpvar->line_num;
        out = tplpvar->out;
        tplpvar->out = include;

        tpl_transfer(tplpvar);

        p = MapFind(&tplpvar->tpl_map,"%token");
        if(p){
            tokens_print(tplpvar,p->value,prefix);
        }

        /* include file middle */
        tpl_transfer(tplpvar);

        /* include dynamic parse function declaration */
        p = MapFind(&tplpvar->tpl_map,"%parse_decl");
        if(p){
            char *parse_name = tplpvar->gp->parse_name ? tplpvar->gp->parse_name : tplpvar->parse_name;
            tpl_fprintf(tplpvar,p->value,parse_name,tplpvar->token_type);
        }

        /* include file tail */
        tpl_transfer(tplpvar);
        tplpvar->out = out;
        tplpvar->line_num = line_num;

        fclose(include);

    }else{
        /* include file header */
        line_num = tplpvar->line_num;
        out = tplpvar->out;
        tplpvar->out = 0;
        tpl_transfer(tplpvar);

        /* include file middle */
        tpl_transfer(tplpvar);
        tplpvar->out = out;
        tplpvar->line_num = line_num;

        p = MapFind(&tplpvar->tpl_map,"%token");
        if(p)tokens_print(tplpvar,p->value,prefix);

        /* include file tail */
        line_num = tplpvar->line_num;
        tplpvar->out = 0;
        tpl_transfer(tplpvar);
        tplpvar->out = out;
        tplpvar->line_num = line_num;

    }

}

PRIVATE void add_constants(struct tpl_member *tplpvar)
{
    struct Pair *p;

    fprintf(tplpvar->out,"\r\n");
    tplpvar->line_num++;

    p = MapFind(&tplpvar->tpl_map,"%symbol_num");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->symbol_num);

    p = MapFind(&tplpvar->tpl_map,"%state_num");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->state_num);

    p = MapFind(&tplpvar->tpl_map,"%rule_num");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->rule_num);

    p = MapFind(&tplpvar->tpl_map,"%accept_action");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->state_num + tplpvar->gp->rule_num);

    p = MapFind(&tplpvar->tpl_map,"%error_action");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->state_num + tplpvar->gp->rule_num + 1);

    p = MapFind(&tplpvar->tpl_map,"%shift_default_offset");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->action_table.minTokOffset - 1);

    p = MapFind(&tplpvar->tpl_map,"%reduce_default_offset");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->action_table.minNtOffset - 1);

}



PRIVATE void tpl_yy_action(struct tpl_member *tplpvar)
{
    int i, j, n, act;
    struct Pair *p;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%action_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%action_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,0,tplpvar->gp->state_num + tplpvar->gp->rule_num + 10)); /* yy_action type */

    n = ACTTAB_SIZE(tplpvar->action_table);

    for(i = j = 0; i < n; i++){
        act = ACTTAB_YYACTION(tplpvar->action_table,i);
        if(act < 0)act = tplpvar->gp->state_num + tplpvar->gp->rule_num + 1; /* YY_ERROR_ACTION */
        tpl_fprintf(tplpvar,member,act);
        if(j == tplpvar->array_width - 1 || i == n - 1){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%action_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_state(struct tpl_member *tplpvar)
{
    int i, j, n, la;
    struct Pair *p;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%state_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%state_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,0,tplpvar->gp->state_num + 10)); /* yy_state type */

    n = ACTTAB_SIZE(tplpvar->action_table);

    for(i = j = 0; i < n; i++){
        la = ACTTAB_YYSTATE(tplpvar->action_table,i);
        if(la < 0)la = tplpvar->gp->state_num;  /* empty flag (-1) by YYNSYMBOL indicate */
        tpl_fprintf(tplpvar,member,la);
        if(j == tplpvar->array_width - 1 || i == n - 1){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%state_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_shift_offset(struct tpl_member *tplpvar)
{
    int j, offset;
    struct Pair *p;
    struct state *stp;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%shift_offset_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%shift_offset_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,tplpvar->action_table.minTokOffset - 1,tplpvar->action_table.maxTokOffset)); /* yy_shift_offset type */

    for(j = 0, stp = tplpvar->gp->states; stp; stp = stp->next){
        offset = stp->tokOffset;
        if(offset == NO_OFFSET)offset = tplpvar->action_table.minTokOffset - 1;
        tpl_fprintf(tplpvar,member,offset);
        if(j == tplpvar->array_width - 1 || stp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%shift_offset_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_reduce_offset(struct tpl_member *tplpvar)
{
    int j, offset;
    struct Pair *p;
    struct state *stp;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%reduce_offset_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_offset_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,tplpvar->action_table.minNtOffset - 1,tplpvar->action_table.maxNtOffset)); /* yy_reduce_offset type */

    for(j = 0, stp = tplpvar->gp->states; stp; stp = stp->next){
        offset = stp->ntOffset;
        if(offset == NO_OFFSET)offset = tplpvar->action_table.minNtOffset - 1;
        tpl_fprintf(tplpvar,member,offset);
        if(j == tplpvar->array_width - 1 || stp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%reduce_offset_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_default(struct tpl_member *tplpvar)
{
    int j, act;
    struct Pair *p;
    struct state *stp;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%default_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%default_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,0,tplpvar->gp->state_num + tplpvar->gp->rule_num + 10)); /* yy_default type */

    for(j = 0, stp = tplpvar->gp->states; stp; stp = stp->next){
        act = stp->defaultAction;
        tpl_fprintf(tplpvar,member,act);
        if(j == tplpvar->array_width - 1 || stp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%default_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_fallback(struct tpl_member *tplpvar)
{
    int j;
    struct Pair *p;
    struct symbol *sp;
    char *empty = "";
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%fallback_empty");
    if(p)empty = p->value;

    p = MapFind(&tplpvar->tpl_map,"%fallback_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%fallback_prefix");
    if(p)tpl_fprintf(tplpvar,p->value,minimum_size_type(tplpvar,0,tplpvar->gp->symbol_num + 10)); /* yy_fallback type */

    for(j = 0, sp = tplpvar->gp->symbols; sp; sp = sp->next){
        if(sp->fallback == 0){
            tpl_fprintf(tplpvar,empty,sp->name);
        }else{
            tpl_fprintf(tplpvar,member,sp->fallback->index,sp->name,sp->fallback->name);
        }
        if(j == tplpvar->array_width - 1 || sp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%fallback_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_symbolName(struct tpl_member *tplpvar)
{
    int j;
    struct Pair *p;
    struct symbol *sp;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%symbol_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%symbol_prefix");
    if(p)tpl_fprintf(tplpvar,p->value);     /* yy_symbolName no need calculate type */

    for(j = 0, sp = tplpvar->gp->symbols; sp; sp = sp->next){
        tpl_fprintf(tplpvar,member,sp->name);
        if(j == tplpvar->array_width - 1 || sp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%symbol_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_ruleName(struct tpl_member *tplpvar)
{
    int j, i;
    struct Pair *p;
    struct rule *rp;
    char *member_prefix = "";
    char *member_suffix = "";

    p = MapFind(&tplpvar->tpl_map,"%rule_member_prefix");
    if(p)member_prefix = p->value;
    p = MapFind(&tplpvar->tpl_map,"%rule_member_suffix");
    if(p)member_suffix = p->value;

    p = MapFind(&tplpvar->tpl_map,"%rule_prefix");
    if(p)tpl_fprintf(tplpvar,p->value);     /* yy_ruleName no need calculate type */

    for(j = 0, rp = tplpvar->gp->rule; rp; rp = rp->next){
        tpl_fprintf(tplpvar,member_prefix);\
        fprintf(tplpvar->out,rp->lhs->name);
        fprintf(tplpvar->out," ->");
        for(i = 0; i < rp->nrhs; i++){
            fprintf(tplpvar->out," ");
            fprintf(tplpvar->out,rp->rhs[i]->name);
        }
        fprintf(tplpvar->out,".");
        tpl_fprintf(tplpvar,member_suffix);
        if(j == tplpvar->array_width - 1 || rp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%rule_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void tpl_yy_ruleInfo(struct tpl_member *tplpvar)
{
    int j;
    struct Pair *p;
    struct rule *rp;
    char *member = "";

    p = MapFind(&tplpvar->tpl_map,"%rule_info_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%rule_info_prefix");
    if(p)tpl_fprintf(tplpvar,p->value);     /* yy_symbolName no need calculate type */

    for(j = 0, rp = tplpvar->gp->rule; rp; rp = rp->next){
        tpl_fprintf(tplpvar,member,rp->lhs->index,rp->nrhs);
        if(j == tplpvar->array_width - 1 || rp->next == 0){
            fprintf(tplpvar->out,"\r\n");
            tplpvar->line_num++;
            j = 0;
        }else{
            j++;
        }
    }

    p = MapFind(&tplpvar->tpl_map,"%rule_info_suffix");
    if(p)tpl_fprintf(tplpvar,p->value);
}

PRIVATE void emit_destructor_code(struct tpl_member *tplpvar, char *arg, char *code, int code_line, int hash)
{
    /* not have code */
    if(code == 0)return;

    tpl_linedir(tplpvar,code_line,tplpvar->gp->filename);
    for(; *code; code++){
        if(tplpvar->special_escape && *code == tplpvar->special_escape){
            code++;
            if(*code == 0)break;
        }else if(*code == '$' && code[1] == '$'){
            tpl_fprintf(tplpvar,arg,hash);
            code++;
            continue;
        }
        if(*code == '\n')tplpvar->line_num++;
        fputc(*code,tplpvar->out);
    }
    fprintf(tplpvar->out,"\r\n");
    tplpvar->line_num++;
    tpl_linedir(tplpvar,tplpvar->line_num + 1,tplpvar->gp->outname);
}

PRIVATE void tpl_syntax_error(struct tpl_member *tplpvar, char *code, int code_line)
{
    tpl_transfer(tplpvar); /* header */

    if(code){
        struct Pair *p;
        char *major = "";
        char *minor = "";

        p = MapFind(&tplpvar->tpl_map,"%syntax_error_major");
        if(p)major = p->value;

        p = MapFind(&tplpvar->tpl_map,"%syntax_error_minor");
        if(p)minor = p->value;

        tpl_linedir(tplpvar,code_line,tplpvar->gp->filename);

        for(; *code; code++){
            if(tplpvar->special_escape && *code == tplpvar->special_escape){
                code++;
                if(*code == 0)break;
            }else if(*code == '@' && code[1] == '@'){
                tpl_fprintf(tplpvar,major);
                code++;
                continue;
            }else if(*code == '$' && code[1] == '$'){
                tpl_fprintf(tplpvar,minor,tplpvar->tok_hash);
                code++;
                continue;
            }
            if(*code == '\n')tplpvar->line_num++;
            fputc(*code,tplpvar->out);
        }

        fprintf(tplpvar->out,"\r\n");
        tplpvar->line_num++;
        tpl_linedir(tplpvar,tplpvar->line_num + 1,tplpvar->gp->outname);
    }

    tpl_transfer(tplpvar); /* tail */
}

PRIVATE void tpl_destructor_proc(struct tpl_member *tplpvar)
{
    struct Pair *p;
    struct symbol *sp, *nsp;
    int proc_num = 0;
    char *prefix = "";
    char *arg = "";
    char *suffix = "";

    p = MapFind(&tplpvar->tpl_map,"%destructor_prefix");
    if(p)prefix = p->value;

    p = MapFind(&tplpvar->tpl_map,"%destructor_arg");
    if(p)arg = p->value;

    p = MapFind(&tplpvar->tpl_map,"%destructor_suffix");
    if(p)suffix = p->value;

    /* initialize token default destruct */
    tpl_fprintf(tplpvar,prefix,proc_num++);
    emit_destructor_code(tplpvar,arg,tplpvar->gp->token_destruct,tplpvar->gp->token_destruct_line,tplpvar->tok_hash);
    tpl_fprintf(tplpvar,suffix);

    /* initialize nonterminal default destruct */
    tpl_fprintf(tplpvar,prefix,proc_num++);
    emit_destructor_code(tplpvar,arg,tplpvar->gp->nonterminal_destruct,tplpvar->gp->nonterminal_destruct_line,tplpvar->nt_hash);
    tpl_fprintf(tplpvar,suffix);

    /* initialize destructor process number */
    for(sp = tplpvar->gp->symbols; sp; sp = sp->next){
        if(sp->destructor){
            sp->destructor_proc_num = proc_num;
            /* generate process */
            tpl_fprintf(tplpvar,prefix,proc_num++);
            emit_destructor_code(tplpvar,arg,sp->destructor,sp->destructor_line,sp->dtnum);
            tpl_fprintf(tplpvar,suffix);
            for(nsp = sp->next; nsp; nsp = nsp->next){
                if(nsp->destructor && strcmp(nsp->destructor,sp->destructor) == 0 && nsp->dtnum == sp->dtnum){/* need same code and same type */
                    nsp->destructor_proc_num = sp->destructor_proc_num;
                    nsp->destructor = 0;
                }
            }
        }else if(sp->destructor_proc_num == -1 && sp->type == TERMINAL && sp->dtnum == tplpvar->tok_hash){  /* default token destructor process */
            sp->destructor_proc_num = 0;
        }else if(sp->destructor_proc_num == -1 && sp->type == NONTERMINAL && sp->dtnum == tplpvar->nt_hash){   /* default nonterminal destructor process */
            sp->destructor_proc_num = 1;
        }
    }
}

PRIVATE void tpl_destructor_init(struct tpl_member *tplpvar)
{
    struct Pair *p;
    struct symbol *sp;
    char *prefix = "";
    char *member = "";
    char *suffix = "";

    p = MapFind(&tplpvar->tpl_map,"%destructor_init_prefix");
    if(p)prefix = p->value;

    p = MapFind(&tplpvar->tpl_map,"%destructor_init_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%destructor_init_suffix");
    if(p)suffix = p->value;

    tpl_fprintf(tplpvar,prefix);

    /* initialize destructor process number */
    for(sp = tplpvar->gp->symbols; sp; sp = sp->next){
        if(sp->destructor_proc_num != -1){
            tpl_fprintf(tplpvar,member,sp->index,sp->destructor_proc_num);
        }
    }

    tpl_fprintf(tplpvar,suffix);
}

PRIVATE int getllen(char *str)
{
    int len;
    char *cur = str;

    while(*cur != END_OF_INPUT && *cur != '\n')
    {
        cur++;
    }

    len = cur - str;
    return len;
}

PRIVATE void emit_reduce_code(struct tpl_member *tplpvar, struct rule *rp, char *lvalue, char *rvalue, char *mvalue, char *code, int code_line)
{
    /* not have code */
    if(code == 0)return;

    char *cp, *ncp, *bp, *xp, *yp;
    char saved;
    int i, line;
    bp = code;
    line = code_line;

    tpl_linedir(tplpvar,code_line,tplpvar->gp->filename); /* line redirection */
    for(cp = code; *cp; cp++){
        if(tplpvar->special_escape && *cp == tplpvar->special_escape){
            cp++;
            if(*cp == 0)break;
        }else if(*cp == '@'){
            /* @num, @num is yy for dynamic
             * switch yystack_top[-n].yymajor
             * for yystack_top[-n].yyminor.yy_n */
            int num = 0;
            cp++;
            ncp = cp; /* '+' or '-' or num */
            ncp++;
            while(*ncp && *ncp >= '0' && *ncp <= '9')ncp++;
            saved = *ncp;
            *ncp = 0;
            num = atoi(cp);
            *ncp = saved;
            if(num > rp->nrhs) num = rp->nrhs; /* if $num num > nrhs then num = nrhs */
            tpl_fprintf(tplpvar,mvalue,num - rp->nrhs);
            if(num == 0 && ncp == cp + 1)cp = ncp - 2; /* $- is ($0)- , $+ is ($0)+, $-0 is ($0) */
            else cp = ncp - 1;
            continue;
        }else if(*cp == '$'){
            cp++;
            /* $$ is rule lvalue */
            if(*cp == '$'){
                tpl_fprintf(tplpvar,lvalue,rp->lhs->dtnum);
            }else if(*cp == '<'){ /* $<type>$ or $<type>num */
                cp++;
                struct Pair *p;
                int num = 0;
                BOOL type_exist;
                ncp = cp;
                while(*ncp && *ncp != '>'){
                    if(*ncp == '\n'){
                        tplpvar->line_num++;
                        line++;
                        bp = ncp + 1;
                    }
                    ncp++;
                }
                /* jump space */
                xp = cp;
                yp = ncp;
                while(isspace(*xp))xp++;
                while(isspace(*(yp-1)))yp--;
                saved = *yp;
                *yp = 0;
                p = MapFind(&tplpvar->type_map,xp);
                *yp = saved;

                if(p){
                    num = p->num;
                    type_exist = TRUE;
                }else{
                    int column = cp - bp + 1;
                    Error_fprintf(UNRECORDED_SYMBOL_TYPE,tplpvar->gp->filename,
                                  line,
                                  column,
                                  column + getllen(cp) - 1,
                                  bp);
                    PrintFailureNote(column - 1);     /* Print ^-- this */
                    type_exist = FALSE;
                }
                if(*ncp == '>')cp = ncp + 1; /* jump '>' */
                else cp = ncp;               /* *ncp == '\0' then as $<type>0 */

                if(*cp == '$'){
                    if(type_exist)tpl_fprintf(tplpvar,lvalue,num);
                }else if(isalpha(*cp)){      /* alias */
                    ncp = cp;
                    ncp++;
                    while(*ncp && (isalnum(*ncp) || *ncp == '_'))ncp++;
                    saved = *ncp;
                    *ncp = 0;
                    if(rp->lhsalias && strcmp(cp - 1,rp->lhsalias) == 0){ /* left hand alias */
                        if(type_exist)tpl_fprintf(tplpvar,lvalue,num);
                    }else{/* right hand alias */
                        for(i = 0; i < rp->nrhs; i++){
                            if(rp->rhsalias[i] && strcmp(cp - 1,rp->rhsalias[i]) == 0){
                                if(type_exist)tpl_fprintf(tplpvar,rvalue,i - rp->nrhs + 1,num);
                                break;
                            }
                        }
                        if(i == rp->nrhs){
                            int column = cp - bp;
                            *ncp = saved;
                            Error_fprintf(UNRECORDED_ALIAS,tplpvar->gp->filename,
                                          line,
                                          column,
                                          column + getllen(cp),
                                          bp);
                            PrintFailureNote(column - 1);     /* Print ^-- this */
                        }
                    }
                    *ncp = saved;
                    cp = ncp - 1;
                }else{
                    int inum = 0;
                    ncp = cp;
                    ncp++;
                    while(*ncp && *ncp >= '0' && *ncp <= '9')ncp++;
                    saved = *ncp;
                    *ncp = 0;
                    inum = atoi(cp);
                    *ncp = saved;
                    if(inum > rp->nrhs) inum = rp->nrhs; /* if $num num > nrhs then num = nrhs */
                    if(type_exist)tpl_fprintf(tplpvar,rvalue,inum - rp->nrhs,num);
                    if(num == 0 && ncp == cp + 1)cp = ncp - 2; /* $- is ($0)- , $+ is ($0)+, $-0 is ($0) */
                    else cp = ncp - 1;
                }
            }else if(isalpha(*cp)){ /* alias */
                ncp = cp;
                ncp++;
                while(*ncp && (isalnum(*ncp) || *ncp == '_'))ncp++;
                saved = *ncp;
                *ncp = 0;
                if(rp->lhsalias && strcmp(cp - 1,rp->lhsalias) == 0){ /* left hand alias */
                    tpl_fprintf(tplpvar,lvalue,rp->lhs->dtnum);
                }else{/* right hand alias */
                    for(i = 0; i < rp->nrhs; i++){
                        if(rp->rhsalias[i] && strcmp(cp - 1,rp->rhsalias[i]) == 0){
                            tpl_fprintf(tplpvar,rvalue,i - rp->nrhs + 1,rp->rhs[i]->dtnum);
                            break;
                        }
                    }
                    if(i == rp->nrhs){
                        int column = cp - bp;
                        *ncp = saved;
                        Error_fprintf(UNRECORDED_ALIAS,tplpvar->gp->filename,
                                      line,
                                      column,
                                      column + getllen(cp),
                                      bp);
                        PrintFailureNote(column - 1);     /* Print ^-- this */
                    }
                }
                *ncp = saved;
                cp = ncp - 1;
            }else{
                int num = 0;
                ncp = cp; /* '+' or '-' or num */
                ncp++;
                while(*ncp && *ncp >= '0' && *ncp <= '9')ncp++;
                saved = *ncp;
                *ncp = 0;
                num = atoi(cp);
                *ncp = saved;
                if(num > rp->nrhs) num = rp->nrhs; /* if $num num > nrhs then num = nrhs */
                if(num >= 1 && num <= rp->nrhs){   /* yacc $1 $2 $3 */
                    tpl_fprintf(tplpvar,rvalue,num - rp->nrhs,rp->rhs[num - 1]->dtnum);
                }else{
                    int column = cp - bp;
                    Error_fprintf(MISSING_SYMBOL_TYPE,tplpvar->gp->filename,
                                  line,
                                  column,
                                  column + getllen(cp),
                                  bp);
                    PrintFailureNote(column - 1);     /* Print ^-- this */
                    /* tpl_fprintf(tplpvar,rvalue,num - rp->nrhs,tplpvar->nt_hash); */
                }
                if(num == 0 && ncp == cp + 1)cp = ncp - 2; /* $- is ($0)- , $+ is ($0)+, $-0 is ($0) */
                else cp = ncp - 1;
            }
            continue;
        }
        if(*cp == '\n'){
            tplpvar->line_num++;
            line++;
            bp = cp + 1;
        }
        fputc(*cp,tplpvar->out);
    }
    fprintf(tplpvar->out,"\r\n");
    tplpvar->line_num++;
    tpl_linedir(tplpvar,tplpvar->line_num + 1,tplpvar->gp->outname); /* line redirection */
}

PRIVATE void tpl_reduce_proc(struct tpl_member *tplpvar)
{
    struct Pair *p;
    struct rule *rp;
    int proc_num = 0;
    char *prefix = "";
    char *lvalue = "";
    char *rvalue = "";
    char *mvalue = "";
    char *suffix = "";

    p = MapFind(&tplpvar->tpl_map,"%reduce_prefix");
    if(p)prefix = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_arg_lvalue");
    if(p)lvalue = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_arg_rvalue");
    if(p)rvalue = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_arg_mvalue");
    if(p)mvalue = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_suffix");
    if(p)suffix = p->value;

    /* initialize token default reduce code */
    if(tplpvar->gp->default_code){
        tpl_fprintf(tplpvar,prefix,proc_num++);
        emit_reduce_code(tplpvar,0,"","","",tplpvar->gp->default_code,tplpvar->gp->default_code_line);
        tpl_fprintf(tplpvar,suffix);
    }

    /* initialize destructor process number */
    for(rp = tplpvar->gp->rule; rp; rp = rp->next){
        if(rp->code){
            rp->reduce_proc_num = proc_num;
            tpl_fprintf(tplpvar,prefix,proc_num++);
            emit_reduce_code(tplpvar,rp,lvalue,rvalue,mvalue,rp->code,rp->code_line);
            tpl_fprintf(tplpvar,suffix);
        }else if(tplpvar->gp->default_code){
            rp->reduce_proc_num = 0;
        }
    }
}

PRIVATE void tpl_reduce_init(struct tpl_member *tplpvar)
{
    struct Pair *p;
    struct rule *rp;
    char *prefix = "";
    char *member = "";
    char *suffix = "";

    p = MapFind(&tplpvar->tpl_map,"%reduce_init_prefix");
    if(p)prefix = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_init_member");
    if(p)member = p->value;

    p = MapFind(&tplpvar->tpl_map,"%reduce_init_suffix");
    if(p)suffix = p->value;

    tpl_fprintf(tplpvar,prefix);

    /* initialize reduce process number */
    for(rp = tplpvar->gp->rule; rp; rp = rp->next){
        if(rp->reduce_proc_num != -1){
            tpl_fprintf(tplpvar,member,rp->index,rp->reduce_proc_num);
        }
    }

    tpl_fprintf(tplpvar,suffix);
}

PRIVATE void tpl_procs_array(struct tpl_member *tplpvar)
{
    struct Pair *p;

    p = MapFind(&tplpvar->tpl_map,"%destructor_array");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->symbol_num + 1); /* include [default] */

    p = MapFind(&tplpvar->tpl_map,"%reduce_array");
    if(p)tpl_fprintf(tplpvar,p->value,tplpvar->gp->rule_num);
}

PRIVATE void tpl_proc(struct tpl_member *tplpvar, char *code, int code_line)
{

    tpl_transfer(tplpvar); /* header */

    if(code){
        tpl_ycode(tplpvar,code,code_line);
    }

    tpl_transfer(tplpvar); /* tail */
}

void GenerateParserDriver(struct global *gp)
{
    struct tpl_member tplvar;
    struct Pair *p;

    tplvar.gp = gp;
    tplvar.parse_name = "";
    tplvar.in = tpl_open(gp);            /* open template file */
    tplvar.special_escape = 0;

    MapNew(&tplvar.tpl_map,(int(*)())strcmp);
    MapNew(&tplvar.type_map,(int(*)())strcmp);

    tplvar.line = (char*)Mem_alloc(sizeof(char)*LINESIZE);
    MEM_CHECK(tplvar.line);
    tplvar.lsize = LINESIZE;

    tpl_initmap(&tplvar);

    /* int map will find a suit size */
    tplvar.temp= (char*)Mem_alloc(sizeof(char)*(tplvar.lsize));
    MEM_CHECK(tplvar.temp);

    p = MapFind(&tplvar.tpl_map,"%out");

    /* Confirm out put file */
    if(p && (tplvar.out = fopen((char*)p->value,"wb")) != 0){
        gp->outname = (char*)p->value;
    }else{
        gp->outname = Make_filename(gp->filename,".src");
        tplvar.out = fopen(gp->outname,"wb");
        Error_fprintf(INVALID_TPL_OUT_NAME,gp->tplname,p ? p->value:"",gp->outname);
    }

    /* set line start number */
    p = MapFind(&tplvar.tpl_map,"%line_start");
    if(p){
        tplvar.line_num = atoi(p->value);
    }

    /* set special escape */
    p = MapFind(&tplvar.tpl_map,"%special_escape");
    if(p && !isspace(*((char*)p->value))){
        tplvar.special_escape = *((char*)p->value);
    }

    tpl_init_type(&tplvar);
    tpl_init_replace(&tplvar);

    /* compute yy table */
    Initacttab(&tplvar.action_table);
    ComputeActtab(gp,&tplvar.action_table);

    /* prefixe info */
    tpl_transfer(&tplvar);

    /* .y include file */
    tpl_ycode(&tplvar,gp->include,gp->include_line);

    tpl_transfer(&tplvar);

    tpl_minor_struct(&tplvar);

    /* token header file */
    tpl_include(&tplvar);

    /* add constants */
    add_constants(&tplvar);

    /* struct */
    tpl_transfer(&tplvar);

    /* set array width */
    p = MapFind(&tplvar.tpl_map,"%array_width");
    if(p){
        tplvar.array_width = atoi(p->value);
        if(tplvar.array_width <= 0)tplvar.array_width = 10;
    }

    /* generate table */
    tpl_yy_action(&tplvar);
    tpl_yy_state(&tplvar);
    tpl_yy_shift_offset(&tplvar);
    tpl_yy_reduce_offset(&tplvar);
    tpl_yy_default(&tplvar);
    tpl_yy_fallback(&tplvar);
    tpl_yy_symbolName(&tplvar);
    tpl_yy_ruleName(&tplvar);
    tpl_yy_ruleInfo(&tplvar);

    /* reduce and destructor proces process */
    tpl_procs_array(&tplvar);

    tpl_destructor_proc(&tplvar);
    tpl_destructor_init(&tplvar);

    tpl_reduce_proc(&tplvar);
    tpl_reduce_init(&tplvar);

    /* inner function */
    tpl_transfer(&tplvar);

    /* yy_syntax_error */
    tpl_syntax_error(&tplvar,gp->error,gp->error_line);
    /* yy_parse_error */
    tpl_proc(&tplvar,gp->failed,gp->failed_line);
    /* yy_accept */
    tpl_proc(&tplvar,gp->accept,gp->accept_line);

    /* interface function */
    tpl_transfer(&tplvar);

    /* parse function */
    p = MapFind(&tplvar.tpl_map,"%parse_func_prefix");
    if(p){
        char *parse_name = tplvar.gp->parse_name ? tplvar.gp->parse_name : tplvar.parse_name;
        tpl_fprintf(&tplvar,p->value,parse_name,tplvar.token_type);
    }

    tpl_transfer(&tplvar);

    p = MapFind(&tplvar.tpl_map,"%parse_func_minor");
    if(p){
        tpl_fprintf(&tplvar,p->value,tplvar.tok_hash);
    }

    tpl_transfer(&tplvar);

    p = MapFind(&tplvar.tpl_map,"%parse_func_suffix");
    if(p){
        tpl_fprintf(&tplvar,p->value);
    }

    /* extra code */
    tpl_ycode(&tplvar,gp->code,gp->code_line);

    /* after extra code */
    tpl_transfer(&tplvar);

    Releaseacttab(&tplvar.action_table);
    MapClear(&tplvar.tpl_map,0);
    MapClear(&tplvar.type_map,0);
    fclose(tplvar.out);
    fclose(tplvar.in);
    Mem_free(tplvar.pf);
    Mem_free(tplvar.temp);
    Mem_free(tplvar.line);
}
