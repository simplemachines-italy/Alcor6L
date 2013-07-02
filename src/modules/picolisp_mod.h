
/**
 * PicoLisp's platform agnostic modules.
 * 
 * All of them are declared here; Platform specific
 * modules are defined in platform/xxx/picolisp_platform.h
 */
#ifndef __PICOLISP_MOD_H__
#define __PICOLISP_MOD_H__

#include "pico.h"

// Helper macros.

#define PICOLISP_LIB_DEFINE(fun, mod)\
  {fun, #mod}

// Platform module.
#define PICOLISP_MOD_PD\
  PICOLISP_LIB_DEFINE(pd_platform, pd-platform),\
  PICOLISP_LIB_DEFINE(pd_cpu, pd-cpu),\
  PICOLISP_LIB_DEFINE(pd_board, pd-board),

// Terminal module.
#define PICOLISP_MOD_TERM\
  PICOLISP_LIB_DEFINE(plisp_term_clrscr, term-clrscr),\
  PICOLISP_LIB_DEFINE(plisp_term_clreol, term-clreol),\
  PICOLISP_LIB_DEFINE(plisp_term_moveto, term-moveto),\
  PICOLISP_LIB_DEFINE(plisp_term_moveup, term-moveup),\
  PICOLISP_LIB_DEFINE(plisp_term_movedown, term-movedown),\
  PICOLISP_LIB_DEFINE(plisp_term_moveleft, term-moveleft),\
  PICOLISP_LIB_DEFINE(plisp_term_moveright, term-moveright),\
  PICOLISP_LIB_DEFINE(plisp_term_getlines, term-getlines),\
  PICOLISP_LIB_DEFINE(plisp_term_getcols, term-getcols),\
  PICOLISP_LIB_DEFINE(plisp_term_print, term-print),\
  PICOLISP_LIB_DEFINE(plisp_term_getcx, term-getcx),\
  PICOLISP_LIB_DEFINE(plisp_term_getcy, term-getcy),\
  PICOLISP_LIB_DEFINE(plisp_term_getchar, term-getchar),\
  PICOLISP_LIB_DEFINE(plisp_term_getchar_nowait, term-getchar-nowait),\
  PICOLISP_LIB_DEFINE(plisp_term_decode, term-decode),

#define MOD_CHECK_ID(pvar, mod, id)					\
  if (!platform_ ## mod ## _exists(id))					\
    err(pvar, NULL, #mod " %d does not exist", (unsigned int)id)

#define MOD_CHECK_RES_ID(pvar, mod, id, resmod, resid)			\
  if (!platform_ ## mod ## _check_ ## resmod ## _id(id, resid))		\
    err(pvar, NULL, #resmod" %d not valid with " #mod " %d",		\
	(unsigned)resid, (unsigned)id)

#define MOD_CHECK_TIMER(pvar, id)					\
  if (id == PLATFORM_TIMER_SYS_ID && !platform_timer_sys_available())	\
    err(pvar, NULL, "the system timer is not available on this platform"); \
  if (!platform_timer_exists(id))					\
    err(pvar, NULL, "timer %d does not exist", (unsigned)id)		\

#endif
