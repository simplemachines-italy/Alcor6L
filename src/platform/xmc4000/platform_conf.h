
// Alcor6L platform configuration.

#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__

#if defined ALCOR_LANG_TINYSCHEME
#include "scheme.h"
#elif defined ALCOR_LANG_MYBASIC
# include "my_basic.h"
#elif defined ALCOR_LANG_PICOLISP
# include "picolisp_mod.h"
#elif defined ALCOR_LANG_PICOC
# include "picoc_mod.h"
#else
# include "auxmods.h"
#endif

#include "type.h"
#include "stacks.h"

#define BUILD_CON_GENERIC
#define BUILD_SHELL
#define BUILD_TERM
#define BUILD_ROMFS
#define BUILD_XMODEM
#define BUILD_LINENOISE
#define BUILD_EDITOR_IV

// *****************************************************************************
// UART/Timer IDs configuration data (used in main.c)

#define CON_UART_ID           0
#define CON_UART_SPEED        115200
#define TERM_LINES            25
#define TERM_COLS             80

#define PLATFORM_HAS_SYSTIMER

#if defined ALCOR_LANG_TINYSCHEME

// *****************************************************************************
// Language configurations: tiny-scheme.

#elif defined ALCOR_LANG_MYBASIC

// *****************************************************************************
// Language configurations: my-basic.

#elif defined ALCOR_LANG_PICOLISP

// *****************************************************************************
// Language configurations: picoLisp.

// platform library functions
#define PICOLISP_PLATFORM_LIBS_ROM\
  _ROM(PD)\
  _ROM(TERM)\
  _ROM(ELUA)\
  _ROM(CPU)\
  _ROM(TIMER)\
  _ROM(PIO)

#elif defined ALCOR_LANG_PICOC

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

#ifdef BUILD_ADC
# define ADCLINE _ROM(PICOC_PLAT_LIB_ADC, NULL, &adc_library[0], NULL)
#else
# define ADCLINE
#endif

/* platform library functions */
#define PICOC_PLATFORM_LIBS_ROM\
  _ROM(PICOC_PLAT_LIB_PD, NULL, &pd_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_TERM, NULL, &term_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_CPU, NULL, &cpu_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_ELUA, NULL, &elua_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_UART, NULL, &uart_library[0], NULL)\
  _ROM(PICOC_PLAT_LIB_TMR, NULL, &tmr_library[0], NULL)\
  ADCLINE\
  _ROM(PICOC_PLAT_LIB_PIO, NULL, &pio_library[0], NULL)

  // _ROM(PICOC_PLAT_LIB_I2C, NULL, &i2c_library[0], NULL)
  // _ROM(PICOC_PLAT_LIB_PWM, NULL, &pwm_library[0], NULL)
  // _ROM(PICOC_PLAT_LIB_SPI, NULL, &spi_library[0], NULL)


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
  _ROM(PICOC_PLAT_VAR_UART, &uart_variables[0])\
  _ROM(PICOC_PLAT_VAR_I2C, &i2c_variables[0])\
  _ROM(PICOC_PLAT_VAR_SPI, &spi_variables[0])\
  _ROM(PICOC_PLAT_VAR_TMR, &tmr_variables[0])\
  _ROM(PICOC_PLAT_VAR_PIO, &pio_variables[0])

// PicoC stack configuration.
#define PICOC_STACK_SIZE      (32*1024)

#else

// *****************************************************************************
// Auxiliary libraries that will be compiled for this platform: Lua language.

#define LUA_PLATFORM_LIBS_ROM\
  _ROM( AUXLIB_PIO, luaopen_pio, pio_map )\
  _ROM( AUXLIB_PD, luaopen_pd, pd_map )\
  _ROM( AUXLIB_UART, luaopen_uart, uart_map )\
  _ROM( AUXLIB_TERM, luaopen_term, term_map )\
  _ROM( AUXLIB_PACK, luaopen_pack, pack_map )\
  _ROM( AUXLIB_BIT, luaopen_bit, bit_map )\
  _ROM( AUXLIB_CPU, luaopen_cpu, cpu_map )\
  _ROM( AUXLIB_ELUA, luaopen_elua, elua_map )\
  _ROM( AUXLIB_TMR, luaopen_tmr, tmr_map )\
  _ROM( LUA_MATHLIBNAME, luaopen_math, math_map )

#endif

// *****************************************************************************
// Configuration data

#define EGC_INITIAL_MODE      1

// Number of resources (0 if not available/not implemented)
#define NUM_PIO               16
#define NUM_SPI               0
#define NUM_UART              1
#define NUM_TIMER             0
#define NUM_PHYS_TIMER        0
#define NUM_PWM               0
#define NUM_ADC               0
#define NUM_CAN               0

// PIO prefix ('0' for P0, P1, ... or 'A' for PA, PB, ...)
#define PIO_PREFIX            '0'
// Pins per port configuration:
// #define PIO_PINS_PER_PORT (n) if each port has the same number of pins, or
// #define PIO_PIN_ARRAY { n1, n2, ... } to define pins per port in an array
// Use #define PIO_PINS_PER_PORT 0 if this isn't needed
#define PIO_PIN_ARRAY         { 16, 16, 16, 16, 8, 12, 7, 0, 0, 0, 0, 0, 0, 0, 14, 12 }

#define LINENOISE_HISTORY_SIZE_LUA    50
#define LINENOISE_HISTORY_SIZE_SHELL  10

// Interrupt queue size
#define PLATFORM_INT_QUEUE_LOG_SIZE 5

// Allocator data: define your free memory zones here in two arrays
// (start address and end address)
#define DSRAM1_SIZE           ( 64 * 1024 )
#define DSRAM1_BASE           0x20000000
#define DSRAM2_SIZE           ( 32 * 1024 )
#define DSRAM2_BASE           0x30000000
#define PSRAM_SIZE            ( 64 * 1024 )
#define PSRAM_BASE            0x10000000
#define MEM_START_ADDRESS     { ( void* )end, ( void* )DSRAM2_BASE, ( void* )PSRAM_BASE }
#define MEM_END_ADDRESS       { ( void* )( DSRAM1_BASE + DSRAM1_SIZE - STACK_SIZE_TOTAL - 1 ), ( void* )( DSRAM2_BASE + DSRAM2_SIZE - 1 ), ( void* )( PSRAM_BASE + PSRAM_SIZE - 1 ) }

#define CPU_FREQUENCY         120000000

#endif // #ifndef __PLATFORM_CONF_H__

