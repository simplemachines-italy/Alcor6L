
/* string.h library for large systems - small embedded systems use clibrary.c instead */
/* now, with the tiny ram patch we can use this. clibrary.c is used only when you disable
   optimizations (optram=0) */

#include "interpreter.h"
#include "rotable.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))

static const int trueValue = 1;
static const int falseValue = 0;

/* structure definitions */
const char StdboolDefs[] = "typedef int bool;";

/**
 * setup function.
 * You may include setup stuff here. For now, we have nothing.
 */
void StdboolSetupFunc(void)
{
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

/* rotable for stdbool variables */
const PICOC_RO_TYPE stdbool_variables[] = {
	/* defines */
        {STRKEY("true"), INT(trueValue)},
        {STRKEY("false"), INT(falseValue)},
        {STRKEY("__bool_true_false_are_defined"), INT(trueValue)},
        {NILKEY, NILVAL}
};

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */
