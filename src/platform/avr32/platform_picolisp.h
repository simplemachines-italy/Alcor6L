
#ifndef __PLATFORM_PICOLISP_H__
#define __PLATFORM_PICOLISP_H__

#include "picolisp_mod.h"

// Define all picoLisp specific platform
// components here.

#define PICOLISP_TARGET_SPECIFIC_PROTOS\
  PICOLISP_PROTO_MIZAR32_LCD

#define PICOLISP_PROTO(fname)\
  any fname(any x);

// List all prototypes here.
#define PICOLISP_PROTO_MIZAR32_LCD\
  PICOLISP_PROTO(plisp_lcd_reset)\
  PICOLISP_PROTO(plisp_lcd_setup)\
  PICOLISP_PROTO(plisp_lcd_clear)\
  PICOLISP_PROTO(plisp_lcd_home)\
  PICOLISP_PROTO(plisp_lcd_goto)\
  PICOLISP_PROTO(plisp_lcd_prinl)\
  PICOLISP_PROTO(plisp_lcd_getpos)\
  PICOLISP_PROTO(plisp_lcd_buttons)\
  PICOLISP_PROTO(plisp_lcd_cursor)\
  PICOLISP_PROTO(plisp_lcd_display)\
  PICOLISP_PROTO(plisp_lcd_definechar)

#define PICOLISP_TARGET_SPECIFIC_LIBS\
  PICOLISP_MOD_MIZAR32_LCD

#define PICOLISP_MOD_MIZAR32_LCD\
  PICOLISP_LIB_DEFINE(plisp_lcd_reset, mizar32-lcd-reset),\
  PICOLISP_LIB_DEFINE(plisp_lcd_setup, mizar32-lcd-setup),\
  PICOLISP_LIB_DEFINE(plisp_lcd_clear, mizar32-lcd-clear),\
  PICOLISP_LIB_DEFINE(plisp_lcd_home, mizar32-lcd-home),\
  PICOLISP_LIB_DEFINE(plisp_lcd_goto, mizar32-lcd-goto),\
  PICOLISP_LIB_DEFINE(plisp_lcd_prinl, mizar32-lcd-prinl),\
  PICOLISP_LIB_DEFINE(plisp_lcd_getpos, mizar32-lcd-getpos),\
  PICOLISP_LIB_DEFINE(plisp_lcd_buttons, mizar32-lcd-buttons),\
  PICOLISP_LIB_DEFINE(plisp_lcd_cursor, mizar32-lcd-cursor),\
  PICOLISP_LIB_DEFINE(plisp_lcd_display, mizar32-lcd-display),\
  PICOLISP_LIB_DEFINE(plisp_lcd_definechar, mizar32-lcd-definechar),

// platform specific symbol defines

#define DECLARE_SYM_VARIABLE(name)\
  any name;

// The order offsets of the fields in the DS1337 register set
// TODO: These are also defined in the platform/avr32/rtc.c
// Get rid of redundant definitions.
#define SEC   0
#define MIN   1
#define HOUR  2
#define WDAY  3
#define DAY   4
#define MONTH 5
#define YEAR  6

#define PICOLISP_SYMBOL_VARIABLE_DECLARATIONS\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_sec)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_min)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_hour)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_day)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_month)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_year)\
  DECLARE_SYM_VARIABLE(plisp_mizar32_rtc_wday)
  
#define PICOLISP_DEFINE_SYM(var, anyval, lispsym)\
  var = initSym(anyval, lispsym);

#define PICOLISP_TARGET_SPECIFIC_SYM_DEFINITIONS\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_sec, box(SEC), "*mizar32-rtc-sec*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_min, box(MIN), "*mizar32-rtc-min*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_hour, box(HOUR), "*mizar32-rtc-hour*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_day, box(DAY), "*mizar32-rtc-day*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_month, box(MONTH), "*mizar32-rtc-month*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_year, box(YEAR), "*mizar32-rtc-year*")\
  PICOLISP_DEFINE_SYM(plisp_mizar32_rtc_wday, box(WDAY), "*mizar32-rtc-wday*")
 
#endif // #ifndef __PLATFORM_PICOLISP_H__
