
#ifndef __PLATFORM_PICOC_H__
#define __PLATFORM_PICOC_H__

#include "picoc_mod.h"

// Define all PicoC specific platform
// components here.

// Define all headers.
#define PICOC_PLAT_LIB_ENC "enc.h"
#define PICOC_PLAT_LIB_LCD "lcd.h"

#define PICOC_PLAT_VAR_ENC enc
#define PICOC_PLAT_VAR_LCD lcd

#define PLATFORM_SPECIFIC_LIB_DEFINES\
  PICOC_PLAT_LIB_DEFINE(enc);\
  PICOC_PLAT_LIB_DEFINE(lcd)

#define PLATFORM_SPECIFIC_INIT_DEFINES\
  PICOC_LIB_INIT(enc);\
  PICOC_LIB_INIT(lcd)

#define PLATFORM_SPECIFIC_INIT_CALLS

#define PLATFORM_SPECIFIC_VAR_DEFINES\
  PICOC_VAR_DEFINE(lcd)

#define PLATFORM_SPECIFIC_SETUP_FUNC_DEFINES\
  PICOC_LIB_SETUP_FUNC(lcd)

#endif // #ifndef __PLATFORM_PICOC_H__
