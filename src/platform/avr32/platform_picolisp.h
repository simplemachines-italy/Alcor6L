
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
  PICOLISP_PROTO(lcd_reset)\
  PICOLISP_PROTO(lcd_setup)\
  PICOLISP_PROTO(lcd_clear)\
  PICOLISP_PROTO(lcd_home)\
  PICOLISP_PROTO(lcd_goto)\
  PICOLISP_PROTO(lcd_prinl)\
  PICOLISP_PROTO(lcd_getpos)\
  PICOLISP_PROTO(lcd_buttons)\
  PICOLISP_PROTO(lcd_cursor)\
  PICOLISP_PROTO(lcd_display)\
  PICOLISP_PROTO(plisp_lcd_definechar)

#define PICOLISP_TARGET_SPECIFIC_LIBS\
  PICOLISP_MOD_MIZAR32_LCD

#define PICOLISP_MOD_MIZAR32_LCD\
  PICOLISP_LIB_DEFINE(lcd_reset, mizar32-lcd-reset),\
  PICOLISP_LIB_DEFINE(lcd_setup, mizar32-lcd-setup),\
  PICOLISP_LIB_DEFINE(lcd_clear, mizar32-lcd-clear),\
  PICOLISP_LIB_DEFINE(lcd_home, mizar32-lcd-home),\
  PICOLISP_LIB_DEFINE(lcd_goto, mizar32-lcd-goto),\
  PICOLISP_LIB_DEFINE(lcd_prinl, mizar32-lcd-prinl),\
  PICOLISP_LIB_DEFINE(lcd_getpos, mizar32-lcd-getpos),\
  PICOLISP_LIB_DEFINE(lcd_buttons, mizar32-lcd-buttons),\
  PICOLISP_LIB_DEFINE(lcd_cursor, mizar32-lcd-cursor),\
  PICOLISP_LIB_DEFINE(lcd_display, mizar32-lcd-display),\
  PICOLISP_LIB_DEFINE(plisp_lcd_definechar, mizar32-lcd-definechar),
 
#endif // #ifndef __PLATFORM_PICOLISP_H__
