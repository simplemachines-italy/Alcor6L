
/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))

static int ZeroValue = 0;

#ifndef NO_FP
void StdlibAtof(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = atof(Param[0]->Val->Pointer);
}
#endif

void StdlibAtoi(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = atoi(Param[0]->Val->Pointer);
}

void StdlibAtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = atol(Param[0]->Val->Pointer);
}

#ifndef NO_FP
void StdlibStrtod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = strtod(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}
#endif

void StdlibStrtol(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strtol(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StdlibStrtoul(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strtoul(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StdlibMalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = malloc(Param[0]->Val->Integer);
}

void StdlibCalloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = calloc(Param[0]->Val->Integer, Param[1]->Val->Integer);
}

void StdlibRealloc(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = realloc(Param[0]->Val->Pointer, Param[1]->Val->Integer);
}

void StdlibFree(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    free(Param[0]->Val->Pointer);
}

void StdlibRand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = rand();
}

void StdlibSrand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    srand(Param[0]->Val->Integer);
}

void StdlibAbort(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ProgramFail(Parser, "abort");
}

void StdlibExit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    PlatformExit(Param[0]->Val->Integer);
}

void StdlibGetenv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = getenv(Param[0]->Val->Pointer);
}

void StdlibSystem(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    //ReturnValue->Val->Integer = system(Param[0]->Val->Pointer);
}

#if 0
void StdlibBsearch(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = bsearch(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer, Param[3]->Val->Integer, (int (*)())Param[4]->Val->Pointer);
}
#endif

void StdlibAbs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = abs(Param[0]->Val->Integer);
}

void StdlibLabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = labs(Param[0]->Val->Integer);
}

#if 0
void StdlibDiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = div(Param[0]->Val->Integer, Param[1]->Val->Integer);
}

void StdlibLdiv(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = ldiv(Param[0]->Val->Integer, Param[1]->Val->Integer);
}
#endif

#if 0
/* handy structure definitions */
const char StdlibDefs[] = "\
typedef struct { \
    int quot, rem; \
} div_t; \
\
typedef struct { \
    int quot, rem; \
} ldiv_t; \
";
#endif

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* all stdlib.h functions */
const PICOC_REG_TYPE StdlibFunctions[] = {
#ifndef NO_FP
    { FUNC(StdlibAtof),           PROTO("float atof(char *);") },
    { FUNC(StdlibStrtod),         PROTO("float strtod(char *,char **);") },
#endif
    { FUNC(StdlibAtoi),           PROTO("int atoi(char *);") },
    { FUNC(StdlibAtol),           PROTO("int atol(char *);") },
    { FUNC(StdlibStrtol),         PROTO("int strtol(char *,char **,int);") },
    { FUNC(StdlibStrtoul),        PROTO("int strtoul(char *,char **,int);") },
    { FUNC(StdlibMalloc),         PROTO("void *malloc(int);") },
    { FUNC(StdlibCalloc),         PROTO("void *calloc(int,int);") },
    { FUNC(StdlibRealloc),        PROTO("void *realloc(void *,int);") },
    { FUNC(StdlibFree),           PROTO("void free(void *);") },
    { FUNC(StdlibRand),           PROTO("int rand();") },
    { FUNC(StdlibSrand),          PROTO("void srand(int);") },
    { FUNC(StdlibAbort),          PROTO("void abort();") },
    { FUNC(StdlibExit),           PROTO("void exit(int);") },
    { FUNC(StdlibGetenv),         PROTO("char *getenv(char *);") },
    { FUNC(StdlibSystem),         PROTO("int system(char *);") },
/*    { FUNC(StdlibBsearch),        PROTO("void *bsearch(void *,void *,int,int,int (*)());") }, */
/*    { FUNC(StdlibQsort),          PROTO("void *qsort(void *,int,int,int (*)());") }, */
    { FUNC(StdlibAbs),            PROTO("int abs(int);") },
    { FUNC(StdlibLabs),           PROTO("int labs(int);") },
#if 0
    { FUNC(StdlibDiv),            PROTO("div_t div(int);") },
    { FUNC(StdlibLdiv),           PROTO("ldiv_t ldiv(int);") },
#endif
    { NILFUNC,                    NILPROTO }
};

/* creates various system-dependent definitions */
void StdlibSetupFunc(void)
{
    /* define NULL, TRUE and FALSE */
    if (!VariableDefined(TableStrRegister("NULL")))
        VariableDefinePlatformVar(NULL, "NULL", &IntType, (union AnyValue *)&ZeroValue, FALSE);
}

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
