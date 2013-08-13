
/**
 * my-basic platform agnostic modules.
 * 
 * All of them are declared here; Platform specific
 * modules are defined in platform/xxx/mybasic_platform.h
 */
#ifndef __MYBASIC_MOD_H__
#define __MYBASIC_MOD_H__

// Helper macros.

#define MYBASIC_LIB_DEFINE(fname, fun)\
  {#fname, fun}

// Platform module.
#define MYBASIC_MOD_PD\
  MYBASIC_LIB_DEFINE(PD_PLATFORM, pd_platform),\
  MYBASIC_LIB_DEFINE(PD_CPU, pd_cpu),\
  MYBASIC_LIB_DEFINE(PD_BOARD, pd_board),

#define MYBASIC_MOD_ELUA\
  MYBASIC_LIB_DEFINE(ELUA_VERSION, elua_version),\
  MYBASIC_LIB_DEFINE(ELUA_SAVE_HISTORY, elua_save_history),\
  MYBASIC_LIB_DEFINE(ELUA_SHELL, elua_shell),

#endif
