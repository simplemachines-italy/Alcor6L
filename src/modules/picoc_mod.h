
/**
 * PicoC's platform agnostic modules.
 * 
 * All of them are declared here; Platform specific
 * modules are defined in platform/xxx/picoc_platform.h
 */
#ifndef __PICOC_MOD_H__
#define __PICOC_MOD_H__

#include "interpreter.h"
#include "platform_picoc.h"

// Helper macros.

#define PICOC_PLAT_LIB_DEFINE(mod)\
  extern const PICOC_REG_TYPE mod ## _library[]

#define PICOC_CORE_LIB_DEFINE(mod)\
  extern const PICOC_REG_TYPE mod ## Functions[]

#define PICOC_VAR_DEFINE(var)\
  extern const picoc_roentry var ## _variables[]

#define PICOC_LIB_INIT(mod)\
  void mod ## _library_init(void)

#define PICOC_LIB_INIT_CALL(mod)\
  mod ## _library_init();

#define PICOC_LIB_SETUP_FUNC(mod)\
  void mod ## _lib_setup_func(void)

#define PICOC_CORE_SETUP_FUNC(mod)\
  void mod ## SetupFunc(void)

// Platform variable identifiers.
#define PICOC_PLAT_VAR_PD pd
#define PICOC_PLAT_VAR_ELUA elua
#define PICOC_PLAT_VAR_TMR tmr
#define PICOC_PLAT_VAR_PWM pwm
#define PICOC_PLAT_VAR_TERM term
#define PICOC_PLAT_VAR_CAN can
#define PICOC_PLAT_VAR_ADC adc
#define PICOC_PLAT_VAR_PIO pio
#define PICOC_PLAT_VAR_I2C i2c
#define PICOC_PLAT_VAR_SPI spi
#define PICOC_PLAT_VAR_UART uart
#define PICOC_PLAT_VAR_CPU cpu

// Header files for PicoC.
#define PICOC_PLAT_LIB_PD "pd.h"
#define PICOC_PLAT_LIB_ELUA "elua.h"
#define PICOC_PLAT_LIB_TMR "tmr.h"
#define PICOC_PLAT_LIB_PWM "pwm.h"
#define PICOC_PLAT_LIB_TERM "term.h"
#define PICOC_PLAT_LIB_CAN "can.h"
#define PICOC_PLAT_LIB_ADC "adc.h"
#define PICOC_PLAT_LIB_PIO "pio.h"
#define PICOC_PLAT_LIB_I2C "i2c.h"
#define PICOC_PLAT_LIB_SPI "spi.h"
#define PICOC_PLAT_LIB_UART "uart.h"
#define PICOC_PLAT_LIB_CPU "cpu.h"

// Init funtions.
PICOC_LIB_INIT(elua);
PICOC_LIB_INIT(pd);
PICOC_LIB_INIT(tmr);
PICOC_LIB_INIT(pwm);
PICOC_LIB_INIT(term);
PICOC_LIB_INIT(can);
PICOC_LIB_INIT(adc);
PICOC_LIB_INIT(pio);
PICOC_LIB_INIT(i2c);
PICOC_LIB_INIT(spi);
PICOC_LIB_INIT(uart);
PICOC_LIB_INIT(cpu);
PLATFORM_SPECIFIC_INIT_DEFINES;

// Setup funtions (callbacks)
PICOC_LIB_SETUP_FUNC(term);
PICOC_LIB_SETUP_FUNC(can);
PICOC_LIB_SETUP_FUNC(pio);
PICOC_LIB_SETUP_FUNC(i2c);
PICOC_LIB_SETUP_FUNC(spi);
PICOC_LIB_SETUP_FUNC(uart);
PLATFORM_SPECIFIC_SETUP_FUNC_DEFINES;

// Helper macros.

#define MOD_CHECK_ID(mod, id)\
  if (!platform_ ## mod ## _exists(id))\
    return ProgramFail(NULL, #mod " %d does not exist", (unsigned int)id)

#define MOD_CHECK_RES_ID(mod, id, resmod, resid)\
  if (!platform_ ## mod ## _check_ ## resmod ## _id(id, resid))\
    return ProgramFail(NULL, #resmod" %d not valid with " #mod " %d", (unsigned)resid, (unsigned)id)

#define MOD_CHECK_TIMER(id)\
  if (id == PLATFORM_TIMER_SYS_ID && !platform_timer_sys_available())\
    return ProgramFail(NULL, "the system timer is not available on this platform");\
  if (!platform_timer_exists(id))\
    return ProgramFail(NULL, "timer %d does not exist", (unsigned)id)\

#endif
