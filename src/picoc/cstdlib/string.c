
/* string.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))

static int ZeroValue = 0;

void StringStrcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strcpy(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrncpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strncpy(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringStrcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strcmp(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrncmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strncmp(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringStrcat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strcat(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrncat(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strncat(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringIndex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = index(Param[0]->Val->Pointer, Param[1]->Val->Integer);
}

void StringRindex(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = rindex(Param[0]->Val->Pointer, Param[1]->Val->Integer);
}

void StringStrlen(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strlen(Param[0]->Val->Pointer);
}

void StringMemset(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = memset(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void StringMemcpy(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = memcpy(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringMemcmp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = memcmp(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringMemmove(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = memmove(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringMemchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = memchr(Param[0]->Val->Pointer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void StringStrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strchr(Param[0]->Val->Pointer, Param[1]->Val->Integer);
}

void StringStrrchr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strrchr(Param[0]->Val->Pointer, Param[1]->Val->Integer);
}

void StringStrcoll(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strcoll(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrerror(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strerror(Param[0]->Val->Integer);
}

void StringStrspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strspn(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrcspn(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strcspn(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrpbrk(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strpbrk(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrstr(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strstr(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrtok(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strtok(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void StringStrxfrm(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = strxfrm(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}

void StringStrdup(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strdup(Param[0]->Val->Pointer);
}

void StringStrtok_r(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strtok_r(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Pointer);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* all string.h functions */
const PICOC_REG_TYPE StringFunctions[] = {
    { FUNC(StringIndex),         PROTO("char *index(char *,int);") },
    { FUNC(StringRindex),        PROTO("char *rindex(char *,int);") },
    { FUNC(StringMemcpy),        PROTO("void *memcpy(void *,void *,int);") },
    { FUNC(StringMemmove),       PROTO("void *memmove(void *,void *,int);") },
    { FUNC(StringMemchr),        PROTO("void *memchr(char *,int,int);") },
    { FUNC(StringMemcmp),        PROTO("int memcmp(void *,void *,int);") },
    { FUNC(StringMemset),        PROTO("void *memset(void *,int,int);") },
    { FUNC(StringStrcat),        PROTO("char *strcat(char *,char *);") },
    { FUNC(StringStrncat),       PROTO("char *strncat(char *,char *,int);") },
    { FUNC(StringStrchr),        PROTO("char *strchr(char *,int);") },
    { FUNC(StringStrrchr),       PROTO("char *strrchr(char *,int);") },
    { FUNC(StringStrcmp),        PROTO("int strcmp(char *,char *);") },
    { FUNC(StringStrncmp),       PROTO("int strncmp(char *,char *,int);") },
    { FUNC(StringStrcoll),       PROTO("int strcoll(char *,char *);") },
    { FUNC(StringStrcpy),        PROTO("char *strcpy(char *,char *);") },
    { FUNC(StringStrncpy),       PROTO("char *strncpy(char *,char *,int);") },
    { FUNC(StringStrerror),      PROTO("char *strerror(int);") },
    { FUNC(StringStrlen),        PROTO("int strlen(char *);") },
    { FUNC(StringStrspn),        PROTO("int strspn(char *,char *);") },
    { FUNC(StringStrcspn),       PROTO("int strcspn(char *,char *);") },
    { FUNC(StringStrpbrk),       PROTO("char *strpbrk(char *,char *);") },
    { FUNC(StringStrstr),        PROTO("char *strstr(char *,char *);") },
    { FUNC(StringStrtok),        PROTO("char *strtok(char *,char *);") },
    { FUNC(StringStrxfrm),       PROTO("int strxfrm(char *,char *,int);") },
    { FUNC(StringStrdup),        PROTO("char *strdup(char *);") },
    { FUNC(StringStrtok_r),      PROTO("char *strtok_r(char *,char *,char **);") },
    { NILFUNC,                   NILPROTO}
};

/* creates various system-dependent definitions */
void StringSetupFunc(void)
{
    /* define NULL */
    if (!VariableDefined(TableStrRegister("NULL")))
        VariableDefinePlatformVar(NULL, "NULL", &IntType, (union AnyValue *)&ZeroValue, FALSE);
}

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
