#include "interpreter.h"
#include "rotable.h"

void UnixSetupFunc(void)
{    
}

void Ctest (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    printf("test(%d)\n", Param[0]->Val->Integer);
    Param[0]->Val->Integer = 1234;
}

void Clineno (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    ReturnValue->Val->Integer = Parser->Line;
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
    { FUNC(Ctest),        PROTO("void test(int);") },
    { FUNC(Clineno),      PROTO("int lineno();") },
    { NILFUNC, 		  NILPROTO}
};

void PlatformLibraryInit()
{
    REGISTER("picoc_unix.h", &UnixSetupFunc, &UnixFunctions[0]);
}
