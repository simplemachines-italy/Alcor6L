/**
 * picoc modules. All of them are declared here,
 * then each platform decides what module(s) to
 * register in the src/platform/xxxxx/platform_conf.h
 * file.
 */
#ifndef __PICOC_MOD_H__
#define __PICOC_MOD_H__

#include "interpreter.h"

/* init protos */

/* board & cpu functions */
#define PICOCVAR_PD pd
#define PICOCLIB_PD "pd.h"
void pd_library_init(void);

/* timer */
#define PICOCVAR_TMR tmr
#define PICOCLIB_TMR "tmr.h"
void tmr_library_init(void);

/* pwm */
#define PICOCVAR_PWM pwm
#define PICOCLIB_PWM "pwm.h"
void pwm_library_init(void);

/* terminal functions */
#define PICOCVAR_TERM term
#define PICOCLIB_TERM "term.h"
extern void term_library_init(void);
extern void term_lib_setup_func(void);

/* can */
#define PICOCVAR_CAN can
#define PICOCLIB_CAN "can.h"
extern void can_library_init(void);
extern void can_lib_setup_func(void);

/* adc */
#define PICOCVAR_ADC adc
#define PICOCLIB_ADC "adc.h"
extern void adc_library_init(void);

/* pio */
#define PICOCVAR_PIO pio
#define PICOCLIB_PIO "pio.h"
extern void pio_library_init(void);
extern void pio_lib_setup_func(void);

/* i2c */
#define PICOCVAR_I2C i2c
#define PICOCLIB_I2C "i2c.h"
void i2c_library_init(void);
void i2c_lib_setup_func(void);

/* platform specific functions */
#define PICOCVAR_PLATFORM_LIBS picoc
#define PICOCLIB_PLATFORM_LIBS "picoc.h"
void picoc_platform_library_init(void);

/* spi */
#define PICOCVAR_SPI spi
#define PICOCLIB_SPI "spi.h"
void spi_library_init(void);
void spi_lib_setup_func(void);

/* uart */
#define PICOCVAR_UART uart
#define PICOCLIB_UART "uart.h"
void uart_library_init(void);
void uart_lib_setup_func(void);

#ifdef ENABLE_DISP
#define PICOCVAR_DISP disp
#define PICOCLIB_DISP "disp.h"
void disp_library_init(void);
#endif

/* cpu */
#define PICOCVAR_CPU cpu
#define PICOCLIB_CPU "cpu.h"
void cpu_library_init(void);

/* Helper macros */
#define MOD_CHECK_ID(mod, id)\
	if (!platform_ ## mod ## _exists(id))\
		return ProgramFail(NULL, #mod " %d does not exist", (unsigned int)id)

#define MOD_CHECK_RES_ID(mod, id, resmod, resid)\
	if (!platform_ ## mod ## _check_ ## resmod ## _id(id, resid))\
		return ProgramFail(NULL, #resmod" %d not valid with " #mod " %d", (unsigned)resid, (unsigned)id)

#endif
