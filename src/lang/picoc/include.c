#include "picoc.h"
#include "interpreter.h"
#include "picoc_mod.h"
#include "platform_conf.h"
#include "rotable.h"
#include "rodefs.h"

#ifndef NO_HASH_INCLUDE

/* A list of libraries we can include */
struct IncludeLibrary
{
    const char *IncludeName;
#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
    void (* const SetupFunction)(void);
#else
    void (*SetupFunction)(void);
#endif
    const struct LibraryFunction *FuncList;
    const char *SetupCSource;
#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
    const struct IncludeLibrary *NextLib;
#else
    struct IncludeLibrary *NextLib;
#endif
};

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
/* libraries */
extern const PICOC_REG_TYPE pd_library[];
extern const PICOC_REG_TYPE pio_library[];
extern const PICOC_REG_TYPE can_library[];
extern const PICOC_REG_TYPE adc_library[];
extern const PICOC_REG_TYPE picoc_library[];
extern const PICOC_REG_TYPE pwm_library[];
extern const PICOC_REG_TYPE spi_library[];
#ifdef BUILD_TERM
  extern const PICOC_REG_TYPE term_library[];
#endif
extern const PICOC_REG_TYPE tmr_library[];
extern const PICOC_REG_TYPE uart_library[];

/**
 * FIXME: Look into the platform specific
 * module section in platform/lm3s/platform.c
 * This should not be directly included here.
 */
extern const PICOC_REG_TYPE lm3s_display_library[];

extern const PICOC_REG_TYPE cpu_library[];

#ifndef NO_FP
/* math.c */
#define PICOC_CORE_LIB_MATH "math.h"
#define PICOC_CORE_VAR_MATH "math"
extern const PICOC_REG_TYPE MathFunctions[];
extern const picoc_roentry math_variables[];
void MathSetupFunc(void);
#endif /* #ifndef NO_FP */

/* stdio.c */
#define PICOC_CORE_LIB_STDIO "stdio.h"
#define PICOC_CORE_VAR_STDIO "stdio"
extern const char StdioDefs[];
extern const PICOC_REG_TYPE StdioFunctions[];
extern const picoc_roentry stdio_variables[];
void StdioSetupFunc(void);

/* ctype.c */
#define PICOC_CORE_LIB_CTYPE "ctype.h"
extern const PICOC_REG_TYPE StdCtypeFunctions[];

/* string.c */
#define PICOC_CORE_LIB_STRING "string.h"
extern const PICOC_REG_TYPE StringFunctions[];
void StringSetupFunc(void);

/* stdlib.c */
#define PICOC_CORE_LIB_STDLIB "stdlib.h"
extern const PICOC_REG_TYPE StdlibFunctions[];
void StdlibSetupFunc(void);

/* errno.c */
#define PICOC_CORE_LIB_ERRNO "errno.h"
#define PICOC_CORE_VAR_ERRNO "errno"
extern const picoc_roentry errno_variables[];
void StdErrnoSetupFunc(void);

/* stdbool.c */
#define PICOC_CORE_LIB_STDBOOL "stdbool.h"
#define PICOC_CORE_VAR_STDBOOL "stdbool"
extern const char StdboolDefs[];
extern const picoc_roentry stdbool_variables[];
void StdboolSetupFunc(void);

static const struct IncludeLibrary IncludeLibList[] = {
#ifdef PICOC_PLATFORM_LIBS_REG
	PICOC_PLATFORM_LIBS_REG,
#endif
#if defined (PICOC_PLATFORM_LIBS_ROM)
#undef _ROM
#define _ROM(header, setup_func, lib, def)\
	{header, setup_func, lib, def, NULL},
	PICOC_PLATFORM_LIBS_ROM
#if defined (PICOC_CORE_LIBS_ROM)
	PICOC_CORE_LIBS_ROM
#endif
#endif /* PICOC_PLATFORM_LIBS_ROM */
	{NULL, NULL, NULL, NULL, NULL}
};

/* platform variables */
extern const picoc_roentry can_variables[];
extern const picoc_roentry i2c_variables[];
extern const picoc_roentry pio_variables[];
extern const picoc_roentry spi_variables[];
extern const picoc_roentry term_variables[];
extern const picoc_roentry uart_variables[];

/* list of RO platform variables */
const picoc_rt picoc_rotable[] = {
#ifdef PICOC_PLATFORM_VARS_REG
	PICOC_PLATFORM_VARS_REG,
#endif
#if defined (PICOC_PLATFORM_VARS_ROM)
#undef _ROM
#define _ROM(name, var_list) {#name, var_list},
	PICOC_PLATFORM_VARS_ROM
#if defined (PICOC_CORE_VARS_ROM)
	PICOC_CORE_VARS_ROM
#endif
#endif /* PICOC_PLATFORM_VARS_ROM */
	{NULL, NULL}
};

#else

/* the classical list */
struct IncludeLibrary *IncludeLibList = NULL;

#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB)) */

/**
 * This is probably not going to be used without the Picoc Tiny
 * (magic) RAM patch anyway. If Tiny RAM is diabled, we always
 * have clibrary.
 */

/* initialise the built-in include libraries */
void IncludeInit(void)
{
/* #ifndef BUILTIN_MINI_STDLIB
    IncludeRegister("ctype.h", NULL, &StdCtypeFunctions[0], NULL);
    IncludeRegister("errno.h", &StdErrnoSetupFunc, NULL, NULL);
#ifndef NO_FP
    IncludeRegister("math.h", &MathSetupFunc, &MathFunctions[0], NULL);
#endif
    IncludeRegister("stdbool.h", &StdboolSetupFunc, NULL, StdboolDefs);
    IncludeRegister("stdio.h", &StdioSetupFunc, &StdioFunctions[0], StdioDefs);
    IncludeRegister("stdlib.h", &StdlibSetupFunc, &StdlibFunctions[0], NULL);
    IncludeRegister("string.h", &StringSetupFunc, &StringFunctions[0], NULL);
    IncludeRegister("time.h", &StdTimeSetupFunc, &StdTimeFunctions[0], StdTimeDefs);
    IncludeRegister("unistd.h", &UnistdSetupFunc, &UnistdFunctions[0], UnistdDefs);
#endif */
}

/* clean up space used by the include system */
void IncludeCleanup(void)
{
#if ((PICOC_OPTIMIZE_MEMORY == 0) && defined (BUILTIN_MINI_STDLIB))
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    struct IncludeLibrary *NextInclude;
    
    while (ThisInclude != NULL)
    {
        NextInclude = ThisInclude->NextLib;
        HeapFreeMem(ThisInclude);
        ThisInclude = NextInclude;
    }

    IncludeLibList = NULL;
#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 0) && defined (BUILTIN_MINI_STDLIB)) */
}

#if ((PICOC_OPTIMIZE_MEMORY == 0) && defined (BUILTIN_MINI_STDLIB))
/* register a new build-in include file */
void IncludeRegister(const char *IncludeName, void (*SetupFunction)(void),
		const struct LibraryFunction *FuncList, const char *SetupCSource)
{
    struct IncludeLibrary *NewLib = HeapAllocMem(sizeof(struct IncludeLibrary));
    NewLib->IncludeName = TableStrRegister(IncludeName);
    NewLib->SetupFunction = SetupFunction;
    NewLib->FuncList = FuncList;
    NewLib->SetupCSource = SetupCSource;
    NewLib->NextLib = IncludeLibList;
    IncludeLibList = NewLib;
}
#endif /* #if ((PICOC_OPTIMIZE_MEMORY == 0) && defined (BUILTIN_MINI_STDLIB)) */

/* include all of the system headers */
void PicocIncludeAllSystemHeaders(void)
{
#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined(BUILTIN_MINI_STDLIB))
    const struct IncludeLibrary *ThisInclude = &IncludeLibList[0];
    for (ThisInclude = &IncludeLibList[0]; ThisInclude->IncludeName != NULL; ThisInclude++)
        IncludeFile(TableStrRegister(ThisInclude->IncludeName));
#else
    struct IncludeLibrary *ThisInclude = IncludeLibList;
    for (; ThisInclude != NULL; ThisInclude = ThisInclude->NextLib)
        IncludeFile(ThisInclude->IncludeName);
#endif
}

/* include one of a number of predefined libraries, or perhaps an actual file */
void IncludeFile(const char *FileName)
{
    const struct IncludeLibrary *LInclude;
/* scan for the include file name to see if it's in our list of predefined includes */
#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
    for (LInclude = &IncludeLibList[0]; LInclude->IncludeName != NULL; LInclude++)
#else
    for (LInclude = IncludeLibList; LInclude != NULL; LInclude = LInclude->NextLib)
#endif
    {
        if (strcmp(LInclude->IncludeName, FileName) == 0)
        {
            /* found it - protect against multiple inclusion */
            if (!VariableDefined(FileName))
            {
                VariableDefine(NULL, (char *)FileName, NULL, &VoidType, FALSE);
                
                /* run an extra startup function if there is one */
                if (LInclude->SetupFunction != NULL)
                    (*LInclude->SetupFunction)();
                
                /* parse the setup C source code - may define types etc. */
                if (LInclude->SetupCSource != NULL)
                    PicocParse(FileName, LInclude->SetupCSource, strlen(LInclude->SetupCSource), TRUE, TRUE, FALSE);
                
                /* set up the library functions */
                if (LInclude->FuncList != NULL)
                    LibraryAdd(&GlobalTable, (char *)FileName, (struct LibraryFunction *)LInclude->FuncList);
            }
            
            return;
        }
    }
    
    /* not a predefined file, read a real file */
    PicocPlatformScanFile(FileName);
}

#endif /* NO_HASH_INCLUDE */
