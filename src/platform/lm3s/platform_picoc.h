
#ifndef __PLATFORM_PICOC_H__
#define __PLATFORM_PICOC_H__

#include "picoc_mod.h"

// Define all PicoC specific platform
// components here.

// Define all headers.
#define PICOC_PLAT_LIB_DISP "disp.h"

#define PLATFORM_SPECIFIC_LIB_DEFINES\
  PICOC_PLAT_LIB_DEFINE(lm3s_disp)

#define PLATFORM_SPECIFIC_INIT_DEFINES\
  PICOC_LIB_INIT(lm3s_disp);

#define PLATFORM_SPECIFIC_INIT_CALLS

#define PLATFORM_SPECIFIC_VAR_DEFINES
  //  PICOC_VAR_DEFINE(disp)

#define PLATFORM_SPECIFIC_SETUP_FUNC_DEFINES
//  PICOC_LIB_SETUP_FUNC(disp)

#endif // #ifndef __PLATFORM_PICOC_H__
