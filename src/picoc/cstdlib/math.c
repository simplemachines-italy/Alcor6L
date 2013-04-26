
/* math.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
#ifndef NO_FP

static const double M_EValue =        2.7182818284590452354;   /* e */
static const double M_LOG2EValue =    1.4426950408889634074;   /* log_2 e */
static const double M_LOG10EValue =   0.43429448190325182765;  /* log_10 e */
static const double M_LN2Value =      0.69314718055994530942;  /* log_e 2 */
static const double M_LN10Value =     2.30258509299404568402;  /* log_e 10 */
static const double M_PIValue =       3.14159265358979323846;  /* pi */
static const double M_PI_2Value =     1.57079632679489661923;  /* pi/2 */
static const double M_PI_4Value =     0.78539816339744830962;  /* pi/4 */
static const double M_1_PIValue =     0.31830988618379067154;  /* 1/pi */
static const double M_2_PIValue =     0.63661977236758134308;  /* 2/pi */
static const double M_2_SQRTPIValue = 1.12837916709551257390;  /* 2/sqrt(pi) */
static const double M_SQRT2Value =    1.41421356237309504880;  /* sqrt(2) */
static const double M_SQRT1_2Value =  0.70710678118654752440;  /* 1/sqrt(2) */


void MathSin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sin(Param[0]->Val->FP);
}

void MathCos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = cos(Param[0]->Val->FP);
}

void MathTan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = tan(Param[0]->Val->FP);
}

void MathAsin(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = asin(Param[0]->Val->FP);
}

void MathAcos(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = acos(Param[0]->Val->FP);
}

void MathAtan(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = atan(Param[0]->Val->FP);
}

void MathAtan2(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = atan2(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathSinh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sinh(Param[0]->Val->FP);
}

void MathCosh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = cosh(Param[0]->Val->FP);
}

void MathTanh(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = tanh(Param[0]->Val->FP);
}

void MathExp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = exp(Param[0]->Val->FP);
}

void MathFabs(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = fabs(Param[0]->Val->FP);
}

void MathFmod(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = fmod(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathFrexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = frexp(Param[0]->Val->FP, Param[1]->Val->Pointer);
}

void MathLdexp(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = ldexp(Param[0]->Val->FP, Param[1]->Val->Integer);
}

void MathLog(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = log(Param[0]->Val->FP);
}

void MathLog10(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = log10(Param[0]->Val->FP);
}

void MathModf(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = modf(Param[0]->Val->FP, Param[0]->Val->Pointer);
}

void MathPow(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = pow(Param[0]->Val->FP, Param[1]->Val->FP);
}

void MathSqrt(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = sqrt(Param[0]->Val->FP);
}

void MathRound(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    /* this awkward definition of "round()" due to it being inconsistently
     * declared in math.h */
    ReturnValue->Val->FP = ceil(Param[0]->Val->FP - 0.5);
}

void MathCeil(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = ceil(Param[0]->Val->FP);
}

void MathFloor(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
    ReturnValue->Val->FP = floor(Param[0]->Val->FP);
}

/**
 * setup function.
 * You may include setup stuff here. For now, we have nothing.
 */
void MathSetupFunc(void)
{
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* rotable for core math variables */
const PICOC_RO_TYPE math_variables[] = {
    {STRKEY("M_E"), FLOAT(M_EValue)},
    {STRKEY("M_LOG2E"), FLOAT(M_LOG2EValue)},
    {STRKEY("M_LOG10E"), FLOAT(M_LOG10EValue)},
    {STRKEY("M_LN2"), FLOAT(M_LN2Value)},
    {STRKEY("M_LN10"), FLOAT(M_LN10Value)},
    {STRKEY("M_PI"), FLOAT(M_PIValue)},
    {STRKEY("M_PI_2"), FLOAT(M_PI_2Value)},
    {STRKEY("M_PI_4"), FLOAT(M_PI_4Value)},
    {STRKEY("M_1_PI"), FLOAT(M_1_PIValue)},
    {STRKEY("M_2_PI"), FLOAT(M_2_PIValue)},
    {STRKEY("M_2_SQRTPI"), FLOAT(M_2_SQRTPIValue)},
    {STRKEY("M_SQRT2"), FLOAT(M_SQRT2Value)},
    {STRKEY("M_SQRT1_2"), FLOAT(M_SQRT1_2Value)},
    {NILKEY, NILVAL}
};

/* all math.h functions */
const PICOC_REG_TYPE MathFunctions[] = {
    { FUNC(MathAcos),   PROTO("float acos(float);") },
    { FUNC(MathAsin),   PROTO("float asin(float);") },
    { FUNC(MathAtan),   PROTO("float atan(float);") },
    { FUNC(MathAtan2),  PROTO("float atan2(float, float);") },
    { FUNC(MathCeil),   PROTO("float ceil(float);") },
    { FUNC(MathCos),    PROTO("float cos(float);") },
    { FUNC(MathCosh),   PROTO("float cosh(float);") },
    { FUNC(MathExp),    PROTO("float exp(float);") },
    { FUNC(MathFabs),   PROTO("float fabs(float);") },
    { FUNC(MathFloor),  PROTO("float floor(float);") },
    { FUNC(MathFmod),   PROTO("float fmod(float, float);") },
    { FUNC(MathFrexp),  PROTO("float frexp(float, int *);") },
    { FUNC(MathLdexp),  PROTO("float ldexp(float, int);") },
    { FUNC(MathLog),    PROTO("float log(float);") },
    { FUNC(MathLog10),  PROTO("float log10(float);") },
    { FUNC(MathModf),   PROTO("float modf(float, float *);") },
    { FUNC(MathPow),    PROTO("float pow(float,float);") },
    { FUNC(MathRound),  PROTO("float round(float);") },
    { FUNC(MathSin),    PROTO("float sin(float);") },
    { FUNC(MathSinh),   PROTO("float sinh(float);") },
    { FUNC(MathSqrt),   PROTO("float sqrt(float);") },
    { FUNC(MathTan),    PROTO("float tan(float);") },
    { FUNC(MathTanh),   PROTO("float tanh(float);") },
    { NILFUNC,          NILPROTO }
};

#endif /* !NO_FP */
#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
