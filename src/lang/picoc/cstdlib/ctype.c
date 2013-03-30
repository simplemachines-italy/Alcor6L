
/* ctype.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include <ctype.h>
#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))

void StdIsalnum(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalnum(Param[0]->Val->Integer);
}

void StdIsalpha(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isalpha(Param[0]->Val->Integer);
}

void StdIsblank(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    int ch = Param[0]->Val->Integer;
    ReturnValue->Val->Integer = (ch == ' ') | (ch == '\t');
}

void StdIscntrl(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = iscntrl(Param[0]->Val->Integer);
}

void StdIsdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isdigit(Param[0]->Val->Integer);
}

void StdIsgraph(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isgraph(Param[0]->Val->Integer);
}

void StdIslower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = islower(Param[0]->Val->Integer);
}

void StdIsprint(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isprint(Param[0]->Val->Integer);
}

void StdIspunct(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = ispunct(Param[0]->Val->Integer);
}

void StdIsspace(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isspace(Param[0]->Val->Integer);
}

void StdIsupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isupper(Param[0]->Val->Integer);
}

void StdIsxdigit(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isxdigit(Param[0]->Val->Integer);
}

void StdTolower(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = tolower(Param[0]->Val->Integer);
}

void StdToupper(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = toupper(Param[0]->Val->Integer);
}

void StdIsascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = isascii(Param[0]->Val->Integer);
}

void StdToascii(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = toascii(Param[0]->Val->Integer);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* all string.h functions */
const PICOC_REG_TYPE StdCtypeFunctions[] = {
    { FUNC(StdIsalnum),      PROTO("int isalnum(int);") },
    { FUNC(StdIsalpha),      PROTO("int isalpha(int);") },
    { FUNC(StdIsblank),      PROTO("int isblank(int);") },
    { FUNC(StdIscntrl),      PROTO("int iscntrl(int);") },
    { FUNC(StdIsdigit),      PROTO("int isdigit(int);") },
    { FUNC(StdIsgraph),      PROTO("int isgraph(int);") },
    { FUNC(StdIslower),      PROTO("int islower(int);") },
    { FUNC(StdIsprint),      PROTO("int isprint(int);") },
    { FUNC(StdIspunct),      PROTO("int ispunct(int);") },
    { FUNC(StdIsspace),      PROTO("int isspace(int);") },
    { FUNC(StdIsupper),      PROTO("int isupper(int);") },
    { FUNC(StdIsxdigit),     PROTO("int isxdigit(int);") },
    { FUNC(StdTolower),      PROTO("int tolower(int);") },
    { FUNC(StdToupper),      PROTO("int toupper(int);") },
    { FUNC(StdIsascii),      PROTO("int isascii(int);") },
    { FUNC(StdToascii),      PROTO("int toascii(int);") },
    { NILFUNC,               NILPROTO}
};

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
