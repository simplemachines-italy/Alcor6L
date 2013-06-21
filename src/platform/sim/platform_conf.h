// eLua platform configuration
// Modified to include support for Alcor6L.

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

#ifdef ALCOR_LANG_PICOC
# include "picoc_mod.h"
#else
# include "auxmods.h"
#endif

#include "type.h"
#include "stacks.h"
#include "buf.h"

// *****************************************************************************
// Define here what components you want for this platform

#define BUILD_SHELL
#define BUILD_ADVANCED_SHELL
#define BUILD_ROMFS
#define BUILD_CON_GENERIC
#define BUILD_TERM
//#define BUILD_RFS
#define BUILD_WOFS
#define BUILD_MMCFS

#define TERM_LINES    25
#define TERM_COLS     80

#define PLATFORM_HAS_SYSTIMER

// Auxiliary libraries that will be compiled for this platform

#ifdef ALCOR_LANG_PICOC

// *****************************************************************************
// Language configurations: PicoC.

#ifndef NO_FP
# define MATHLINE _ROM(PICOC_CORE_LIB_MATH, &MathSetupFunc, &MathFunctions[0], NULL)
#else
# define MATHLINE
#endif

/* core library functions */
#define PICOC_CORE_LIBS_ROM\
  MATHLINE\
  _ROM(PICOC_CORE_LIB_STDIO, &StdioSetupFunc, &StdioFunctions[0], StdioDefs)\
  _ROM(PICOC_CORE_LIB_CTYPE, NULL, &StdCtypeFunctions[0], NULL)\
  _ROM(PICOC_CORE_LIB_STDBOOL, &StdboolSetupFunc, NULL, StdboolDefs)\
  _ROM(PICOC_CORE_LIB_STDLIB, &StdlibSetupFunc, &StdlibFunctions[0], NULL)\
  _ROM(PICOC_CORE_LIB_STRING, &StringSetupFunc, &StringFunctions[0], NULL)\
  _ROM(PICOC_CORE_LIB_ERRNO, &StdErrnoSetupFunc, NULL, NULL)

/* platform library functions */
#define PICOC_PLATFORM_LIBS_ROM\
  _ROM(PICOC_PLAT_LIB_PD, NULL, &pd_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_TERM, NULL, &term_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_ELUA, NULL, &elua_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_TMR, NULL, &tmr_library[0], NULL)

#ifndef NO_FP
# define MATHLINE_VAR _ROM(PICOC_CORE_VAR_MATH, &math_variables[0])
#else
# define MATHLINE_VAR
#endif

/* core system variables */
#define PICOC_CORE_VARS_ROM\
  _ROM(PICOC_CORE_VAR_ERRNO, &errno_variables[0])\
  MATHLINE_VAR\
  _ROM(PICOC_CORE_VAR_STDBOOL, &stdbool_variables[0])\
  _ROM(PICOC_CORE_VAR_STDIO, &stdio_variables[0])

#ifdef BUILD_TERM
# define TERMLINE_VAR _ROM(PICOC_PLAT_VAR_TERM, &term_variables[0])
#else
# define TERMLINE_VAR
#endif

/* platform variables */
#define PICOC_PLATFORM_VARS_ROM\
  TERMLINE_VAR\
  _ROM(PICOC_PLAT_VAR_TMR, &tmr_variables[0])

// PicoC stack and heap configurations.
// Needs validation.
#define PICOC_STACK_SIZE      (16*1024)

#else

// *****************************************************************************
// Language configurations: Lua.

#define LUA_PLATFORM_LIBS_ROM\
  _ROM( AUXLIB_PD, luaopen_pd, pd_map )\
  _ROM( LUA_MATHLIBNAME, luaopen_math, math_map )\
  _ROM( AUXLIB_TERM, luaopen_term, term_map )\
  _ROM( AUXLIB_ELUA, luaopen_elua, elua_map )\
  _ROM( AUXLIB_TMR, luaopen_tmr, tmr_map )\

#endif // #ifdef ALCOR_LANG_PICOC

// Bogus defines for common.c
#define CON_UART_ID           0
#define CON_UART_SPEED        0

// *****************************************************************************
// Configuration data

// Virtual timers (0 if not used)
#define VTMR_NUM_TIMERS       0

// Number of resources (0 if not available/not implemented)
#define NUM_PIO               0
#define NUM_SPI               0
#define NUM_UART              0
#define NUM_TIMER             0
#define NUM_PWM               0
#define NUM_ADC               0
#define NUM_CAN               0

// CPU frequency (needed by the CPU module and MMCFS code, 0 if not used)
#define CPU_FREQUENCY         0

// PIO prefix ('0' for P0, P1, ... or 'A' for PA, PB, ...)
#define PIO_PREFIX            'A'
// Pins per port configuration:
// #define PIO_PINS_PER_PORT (n) if each port has the same number of pins, or
// #define PIO_PIN_ARRAY { n1, n2, ... } to define pins per port in an array
// Use #define PIO_PINS_PER_PORT 0 if this isn't needed
#define PIO_PINS_PER_PORT     0

// Allocator data: define your free memory zones here in two arrays
// (start address and end address)
extern void *memory_start_address;
extern void *memory_end_address;
#define MEM_LENGTH (1024 * 1024)
#define MEM_START_ADDRESS     { ( void* )memory_start_address }
#define MEM_END_ADDRESS       { ( void* )memory_end_address }

// RFS configuration
#define RFS_TIMEOUT           0 // dummy, always blocking by implementation
#define RFS_BUFFER_SIZE       BUF_SIZE_512

#endif // #ifndef __PLATFORM_CONF_H__
