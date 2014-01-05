// Module for interfacing with PIO
// Modified to include support for Alcor6L.

// Language specific includes.
//
#if defined ALCOR_LANG_TINYSCHEME
# include "scheme.h"
#endif

#if defined ALCOR_LANG_MYBASIC
# include "my_basic.h"
#endif

#if defined ALCOR_LANG_PICOLISP
# include "pico.h"
#endif

#if defined ALCOR_LANG_PICOC
# include "picoc.h"
# include "interpreter.h"
# include "picoc_mod.h"
# include "rotable.h"
#endif

#if defined ALCOR_LANG_LUA
# include "lua.h"
# include "lualib.h"
# include "lauxlib.h"
# include "auxmods.h"
# include "lrotable.h"
#endif

#include "platform.h"
#include "platform_conf.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// PIO public constants
#define PIO_DIR_OUTPUT      0
#define PIO_DIR_INPUT       1

// PIO private constants
#define PIO_PORT_OP         0
#define PIO_PIN_OP          1

// Local operation masks for all the ports
static pio_type pio_masks[ PLATFORM_IO_PORTS ];

// ****************************************************************************
// Generic helper functions

// Helper function: clear all masks
//
static void pioh_clear_masks(void) {
  int i;
  
  for (i = 0; i < PLATFORM_IO_PORTS; i++)
    pio_masks[i] = 0;
}

// Lua has a seperate set of helper functions.
// We need a set of generic helper functions
// for the other languages in ALcor6L.
//
#ifndef ALCOR_LANG_LUA

# define emit_pio_numeric_ident()\
  pio_value_parse(char *key)

#else

# define emit_pio_numeric_ident()\
  pio_mt_index(lua_State *L)

# define emit_static static
# define emit_ret_type int
# define emit_pio_decode()\
  pio_decode(lua_State *L)

#endif

#ifdef ALCOR_LANG_PICOLISP

# define emit_hfunc_set_pin()\
  plisp_pioh_set_pin(any ex, any y, int v, int op)

# define emit_hfunc_set_port()\
  plisp_pioh_set_port(any ex, any y, int v, int op, pio_type mask)

# define emit_hfunc_setdir()\
  plisp_pio_gen_setdir(any ex, any y, int v, int optype, int op)

# define emit_hfunc_setpull()\
  plisp_pio_gen_setpull(any ex, any y, int v, int optype, int op)

# define emit_hfunc_setval()\
  plisp_pio_gen_setval(any ex, any y, int v, int optype, pio_type val)

# define emit_hfunc_error(msg)\
  err(ex, y, msg);

# define emit_static
# define emit_ret_type any
# define emit_pio_decode()\
  plisp_pio_decode(any ex)

#endif // ALCOR_LANG_PICOLISP

#ifdef ALCOR_LANG_PICOC

# define emit_hfunc_set_pin()\
  picoc_pioh_set_pin(int v, int op)

# define emit_hfunc_set_port()\
  picoc_pioh_set_port(int v, int op, pio_type mask)

# define emit_hfunc_setdir()\
  picoc_pio_gen_setdir(int v, int optype, int op)

# define emit_hfunc_setpull()\
  picoc_pio_gen_setpull(int v, int optype, int op)

# define emit_hfunc_setval()\
  picoc_pio_gen_setval(int v, int optype, pio_type val)

# define emit_hfunc_error(msg)\
  pmod_error(msg);

# define emit_static static
# define emit_ret_type void
# define emit_pio_decode()\
  picoc_pio_decode(pstate *p, val *r, val **param, int n)

#endif // ALCOR_LANG_PICOC

#if !defined ALCOR_LANG_LUA &&\
    !defined ALCOR_LANG_MYBASIC &&\
    !defined ALCOR_LANG_TINYSCHEME

// helper macro.
#define PIO_CHECK(x)\
  if (!x)\
    emit_hfunc_error("Invalid port/pin");

static int emit_hfunc_set_pin() {
  pio_type pio_mask = 0;
  int port, pin;

  pioh_clear_masks();
  port = PLATFORM_IO_GET_PORT(v);
  pin = PLATFORM_IO_GET_PIN(v);

  if (PLATFORM_IO_IS_PORT(v) ||
      !platform_pio_has_port(port) ||
      !platform_pio_has_pin(port, pin))
    emit_hfunc_error("invalid pin");

  pio_mask |= 1 << pin;
  if (pio_mask)
    if (!platform_pio_op(port, pio_mask, op))
      emit_hfunc_error("invalid PIO operation");
  return 0;
}

static int emit_hfunc_set_port() {
  int port;
  u32 port_mask = 0;

  port = PLATFORM_IO_GET_PORT(v);
  if (!PLATFORM_IO_IS_PORT(v) ||
      !platform_pio_has_port(port))
    emit_hfunc_error("invalid port");

  port_mask |= (1ULL << port);
  if (port_mask & (1ULL << port))
    if (!platform_pio_op(port, mask, op))
      emit_hfunc_error("invalid PIO operation");
  return 0;
}

static int emit_hfunc_setdir() {
  if (op == PIO_DIR_INPUT)
    op = optype == PIO_PIN_OP ? PLATFORM_IO_PIN_DIR_INPUT :
      PLATFORM_IO_PORT_DIR_INPUT;
  else if (op == PIO_DIR_OUTPUT)
    op = optype == PIO_PIN_OP ? PLATFORM_IO_PIN_DIR_OUTPUT :
      PLATFORM_IO_PORT_DIR_OUTPUT;
  else
    emit_hfunc_error("invalid direction");

  // Language specific function calls start
  // here.
  //

  // Check if pin operation.
  if (optype == PIO_PIN_OP) {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_pin(v, op);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_pin(ex, y, v, op);
#endif
  // else, we have a port operation.
  } else {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_port(v, op, PLATFORM_IO_ALL_PINS);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_port(ex, y, v, op, PLATFORM_IO_ALL_PINS);
#endif
  }
  return 0;
}

static int emit_hfunc_setpull() {
  if ((op != PLATFORM_IO_PIN_PULLUP) &&
      (op != PLATFORM_IO_PIN_PULLDOWN) &&
      (op != PLATFORM_IO_PIN_NOPULL))
    emit_hfunc_error("invalid pull type");

  // Check if pin operation.
  if (optype == PIO_PIN_OP) {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_pin(v, op);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_pin(ex, y, v, op);
#endif
  // else, we have a port operation.
  } else {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_port(v, op, PLATFORM_IO_ALL_PINS);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_port(ex, y, v, op, PLATFORM_IO_ALL_PINS);
#endif
  }

  return 0;
}

static int emit_hfunc_setval() {
  if ((optype == PIO_PIN_OP) &&
      (val != 1) &&
      (val != 0))
    emit_hfunc_error("invalid pin value");
 
  // Check if a pin operation.
  if (optype == PIO_PIN_OP) {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_pin(v, val == 1 ?
		       PLATFORM_IO_PIN_SET :
		       PLATFORM_IO_PIN_CLEAR);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_pin(ex, y, v, val == 1 ?
		       PLATFORM_IO_PIN_SET :
		       PLATFORM_IO_PIN_CLEAR);
#endif
  // else, we have a port operation.
  } else {
#ifdef ALCOR_LANG_PICOC
    picoc_pioh_set_port(v, val == 1 ?
			PLATFORM_IO_PIN_SET :
			PLATFORM_IO_PIN_CLEAR, val);
#endif
#ifdef ALCOR_LANG_PICOLISP
    plisp_pioh_set_port(ex, y, v, val == 1 ?
			PLATFORM_IO_PIN_SET :
			PLATFORM_IO_PIN_CLEAR, val);
#endif
  }
  return 0;
}

#endif

#if defined ALCOR_LANG_PICOLISP ||\
    defined ALCOR_LANG_PICOC ||\
    defined ALCOR_LANG_LUA

// Helper function.
// returns pin/port numeric identifiers.
int emit_pio_numeric_ident() {
  int port = 0xFFFF, pin = 0xFFFF, isport = 0, sz;
#ifdef ALCOR_LANG_LUA
  const char *key = luaL_checkstring(L, 2);
#endif
  if (!key || *key != 'P')
    return 0;
  if (isupper(key[1])) { // PA, PB, ...
    if (PIO_PREFIX != 'A')
      return 0;
    port = key[1] - 'A';
    if (key[2] == '\0')
      isport = 1;
    else if (key[2] == '_') {
      if (sscanf(key + 3, "%d%n", &pin, &sz) != 1 || sz != strlen(key) - 3)
        return 0;

#ifdef ALCOR_PLATFORM_AVR32
      /* AVR32UC3A0 has a bizarre "port" called "PX" with 40 pins which map to
       * random areas of hardware ports 2 and 3:
       * PX00-PX10 = GPIO100-GPIO90     //Port 3 pins 04-00; port 2 pins 31-26
       * PX11-PX14 = GPIO109-GPIO106    //Port 3 pins 13-10
       * PX15-PX34 = GPIO89-GPIO70      //Port 2 pins 25-06
       * PX35-PX39 = GPIO105-GPIO101    //Port 3 pins 09-05
       * Then port = trunc(GPIO/32) and pin = GPIO % 32
       *
       * This "Port X" exists in EVK1100 and MIZAR32 but not on EVK1101, which
       * only has ports A and B. On EXK1101, the PC and PX syntax will still be
       * accepted but will return nil thanks to the checks against NUM_PIO. 
       */

      // Disallow "PC_06-PC_31" as aliases for PX pins
      if (key[1] == 'C' && pin > 5)
        return 0;

      // Disallow "PD_nn" as aliases for PX pins
      if (key[1] == 'D')
        return 0;

      // Map PX pins 00-39 to their ports/pins in the hardware register layout.
      if (key[1] == 'X') {
        unsigned gpio;

        // You cannot perform port operations on port X because it
        // doesn't exist in hardware.
        if (pin == 0xFFFF)
          return 0;

        // Map PX pin numbers to GPIO pin numbers
        if (pin < 0) return 0;
        if (pin <= 10) gpio = 100 - pin;
        else if (pin <= 14) gpio = 109 - (pin - 11);
        else if (pin <= 34) gpio = 89 - (pin - 15);
        else if (pin <= 39) gpio = 105 - (pin - 35);
        else return 0;

        port = gpio >> 5;
        pin = gpio & 0x1F;
      }
#endif
    }
  } else { // P0, P1, ...
    if (PIO_PREFIX != '0')
      return 0;
    if (!strchr(key, '_')) {  // parse port
      if (sscanf(key + 1, "%d%n", &port, &sz) != 1  || sz != strlen(key) - 1)
        return 0;
      isport = 1;
    } else {   // parse port_pin
      if (sscanf(key + 1, "%d_%d%n", &port, &pin, &sz) != 2 || sz != strlen(key) - 1)
        return 0;
    }
  }
  sz = -1;
  if (isport) {
    if (platform_pio_has_port(port))
      sz = PLATFORM_IO_ENCODE(port, 0, 1);
  } else {
    if (platform_pio_has_port(port) && platform_pio_has_pin(port, pin))
      sz = PLATFORM_IO_ENCODE(port, pin, 0);
  }
  if (sz == -1) {
    return 0;
  } else {
#ifdef ALCOR_LANG_LUA
    lua_pushinteger(L, sz);
    return 1;
#else
    return sz;
#endif
  }
}

// The 'decode' function returns a port/pin pair
// from a platform code.
//
emit_static emit_ret_type emit_pio_decode() {
  int code, port, pin;

  // Lua specific code here.
#ifdef ALCOR_LANG_LUA
  code = (int)luaL_checkinteger(L, 1);
#endif

  // PicoC specific code here.
#ifdef ALCOR_LANG_PICOC
  code = param[0]->Val->Integer;
#endif

  // picoLisp specific code here.
#ifdef ALCOR_LANG_PICOLISP
  any x, y;
  cell c1;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  code = unBox(y); // get code.
#endif

  // finally, something common for all
  // languages.
  //
  // get pin and port values.
  port = PLATFORM_IO_GET_PORT(code);
  pin = PLATFORM_IO_GET_PIN(code);

#ifdef ALCOR_PLATFORM_AVR32
  /* AVR32UC3A0 has a bizarre "port" called "PX" with 40 pins which map to
   * random areas of hardware ports 2 and 3:
   * PX00-PX04 = GPIO100-GPIO96     //Port 3 pins 04-00
   * PX05-PX10 = GPIO95-GPIO90      //Port 2 pins 31-26
   * PX11-PX14 = GPIO109-GPIO106    //Port 3 pins 13-10                       
   * PX15-PX34 = GPIO89-GPIO70      //Port 2 pins 25-06
   * PX35-PX39 = GPIO105-GPIO101    //Port 3 pins 09-05
   *
   * Here, we reverse the decode the hardware port/pins to the PX pin names.
   * This is the inverse of the code above in pio_mt_index().
   */
  if ((port == 2 && pin >= 6) ||
      (port == 3 && pin <= 13)) {
    switch (port) {
    case 2:
      if (pin >= 26)
        pin = (26 + 10) - pin;  // PX05-PX10
      else
        pin = (25 + 15) - pin;  // PX15-PX34
      break;
    case 3:
      if (pin <= 4)
        pin = 4 - pin;          // PX00-PX04
      else if (pin <= 9)
        pin = (35 + 9) - pin;   // PX35-PX39
      else /* 10-13 */
        pin = (13 + 11) - pin;  // PX11-PX14
      break;
    }
    port = 'X' - 'A';   // 'A','B','C' are returned as 0,1,2 so 'X' is 23
  }
#endif

#ifdef ALCOR_LANG_PICOC
  *((int *)param[1]->Val->Pointer) = port;
  *((int *)param[2]->Val->Pointer) = pin;
#endif

#ifdef ALCOR_LANG_PICOLISP
  Push(c1, y = cons(box(port), Nil));
  Push(c1, y = cons(box(pin), y));
  return Pop(c1);
#endif

#ifdef ALCOR_LANG_LUA
  lua_pushinteger(L, port);
  lua_pushinteger(L, pin);
  return 2;
#endif
}

#endif

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// PIO module for picoLisp.
//
// Pin operations.

// (pio-pin-setdir 'sym 'num) -> Nil
any plisp_pio_pin_setdir(any ex) {
  any x, y;
  int ret, dir;

  // get dir.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  dir = unBox(y);

  // get symbol.
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setdir(ex, NULL, ret, PIO_PIN_OP, dir);
  return Nil;
}

// (pio-pin-setpull 'sym 'num) -> Nil
any plisp_pio_pin_setpull(any ex) {
  any x, y;
  int ret, dir;

  // get dir.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  dir = unBox(y);

  // get symbol.
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);
  
  plisp_pio_gen_setpull(ex, NULL, ret, PIO_PIN_OP, dir);
  return Nil;
}

// (pio-pin-setval 'sym 'num) -> Nil
any plisp_pio_pin_setval(any ex) {
  any x, y;
  int ret; pio_type val;

  // get value to set first.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  val = unBox(y);
  
  // get symbol. 
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PIN_OP, val);
  return Nil;
}

// (pio-pin-sethigh 'sym) -> Nil
any plisp_pio_pin_sethigh(any ex) {
  any x, y;
  int ret;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PIN_OP, 1);
  return Nil;
}

// (pio-pin-setlow 'sym) -> Nil
any plisp_pio_pin_setlow(any ex) {
  any x, y;
  int ret;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PIN_OP, 0);
  return Nil;
}

// (pio-pin-getval 'sym) -> num
any plisp_pio_pin_getval(any ex) {
  pio_type value;
  int v, port, pin;
  any x, y;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  v = pio_value_parse(s);
  PIO_CHECK(v);

  port = PLATFORM_IO_GET_PORT(v);
  pin = PLATFORM_IO_GET_PIN(v);

  if (PLATFORM_IO_IS_PORT(v) ||
      !platform_pio_has_port(port) ||
      !platform_pio_has_pin(port, pin)) {
    emit_hfunc_error("invalid pin");
  } else {
    value = platform_pio_op(port,
			    1 << pin,
			    PLATFORM_IO_PIN_GET);
    return box(value);
  }
}

// ****************************************************************************
// Port operations

// (pio-port-setdir 'sym 'num) -> Nil
any plisp_pio_port_setdir(any ex) {
  any x, y;
  int ret, dir;

  // get direction first.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  dir = unBox(y);

  // get symbol.
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setdir(ex, NULL, ret, PIO_PORT_OP, dir);
  return Nil;
}

// (pio-port-setpull 'sym 'num) -> Nil 
any plisp_pio_port_setpull(any ex) {
  any x, y;
  int ret, dir;

  // get pull value first.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  dir = unBox(y);

  // get symbol.
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setpull(ex, NULL, ret, PIO_PORT_OP, dir);
  return Nil;
}

// (pio-port-setval 'sym) -> Nil
any plisp_pio_port_setval(any ex) {
  any x, y;
  int ret; pio_type val;

  // get set value first.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  val = (pio_type)unBox(y);

  // get symbol.
  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PORT_OP, val);
  return Nil;
}

// (pio-port-sethigh 'sym) -> Nil
any plisp_pio_port_sethigh(any ex) {
  any x, y;
  int ret;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PORT_OP, 1);
  return Nil;
}

// (pio-port-setlow 'sym) -> Nil
any plisp_pio_port_setlow(any ex) {
  any x, y;
  int ret;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  ret = pio_value_parse(s);
  PIO_CHECK(ret);

  plisp_pio_gen_setval(ex, NULL, ret, PIO_PORT_OP, 0);
  return Nil;
}

// (pio-port-getval 'sym) -> Nil
any plisp_pio_port_getval(any ex) {
  pio_type value;
  int v, port;
  any x, y;

  // get symbol.
  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char s[bufSize(y)];
  bufString(y, s);
  v = pio_value_parse(s);
  PIO_CHECK(v);
  port = PLATFORM_IO_GET_PORT(v);

  if (!PLATFORM_IO_IS_PORT(v) ||
      !platform_pio_has_port(port)) {
    emit_hfunc_error("invalid port");
  } else {
    value = platform_pio_op(port,
			    PLATFORM_IO_ALL_PINS,
			    PLATFORM_IO_PORT_GET_VALUE);
  }
  return box(value);
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// PIO module for PicoC.

// Platform variables.
static const int pio_input = PIO_DIR_INPUT;
static const int pio_output = PIO_DIR_OUTPUT;
static const int pio_pull_up = PLATFORM_IO_PIN_PULLUP;
static const int pio_pull_down = PLATFORM_IO_PIN_PULLDOWN;
static const int pio_no_pull = PLATFORM_IO_PIN_NOPULL;

extern void pio_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("pio_OUTPUT", pio_output);
  picoc_def_integer("pio_INPUT", pio_input);
  picoc_def_integer("pio_PULLUP", pio_pull_up);
  picoc_def_integer("pio_PULLDOWN", pio_pull_down);
  picoc_def_integer("pio_NOPULL", pio_no_pull);
#endif
}

// ****************************************************************************
// Pin operations

// PicoC: pin_setdir(dir, "pin");
static void pio_pin_setdir(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  int dir = param[0]->Val->Integer;

  r->Val->Integer = picoc_pio_gen_setdir(ret, PIO_PIN_OP, dir);
}

// PicoC: pio_pin_setpull(pull, "pin");
static void pio_pin_setpull(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  int dir = param[0]->Val->Integer;

  r->Val->Integer = picoc_pio_gen_setpull(ret, PIO_PIN_OP, dir);
}

// PicoC: pio_pin_setval(val, "pin");
static void pio_pin_setval(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  pio_type val = (pio_type)param[0]->Val->UnsignedInteger;

  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PIN_OP, val);
}

// PicoC: pio_pin_sethigh("pin");
static void pio_pin_sethigh(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(ret);

  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PIN_OP, 1);
}

// PicoC: pio_pin_setlow("pin");
static void pio_pin_setlow(pstate *p , val *r, val **param, int n)
{
  int ret = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(ret);

  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PIN_OP, 0);
}

// PicoC: pin_val = pio_pin_getval("pin");
static void pio_pin_getval(pstate *p, val *r, val **param, int n)
{
  pio_type value;
  int v, port, pin;
  
  v = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(v);
  port = PLATFORM_IO_GET_PORT(v);
  pin = PLATFORM_IO_GET_PIN(v);
  
  if (PLATFORM_IO_IS_PORT(v) || !platform_pio_has_port(port) ||
      !platform_pio_has_pin(port, pin)) {
    return pmod_error("Invalid pin");
  } else {
    value = platform_pio_op(port, 1 << pin, PLATFORM_IO_PIN_GET);
    r->Val->UnsignedInteger = value;
  }
}

// ****************************************************************************
// Port operations

// PicoC: pio_port_setdir(dir, "port");
static void pio_port_setdir(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  int dir = param[0]->Val->Integer;

  r->Val->Integer = picoc_pio_gen_setdir(ret, PIO_PORT_OP, dir);
}

// PicoC: pio_port_setpull(pull, "port");
static void pio_port_setpull(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  int dir = param[0]->Val->Integer;
  
  r->Val->Integer = picoc_pio_gen_setpull(ret, PIO_PORT_OP, dir);
}

// PicoC: pio_port_setval(val, "port");
static void pio_port_setval(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[1]->Val->Identifier);
  PIO_CHECK(ret);
  pio_type val = (pio_type)param[0]->Val->UnsignedInteger;
  
  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PORT_OP, val);
}

// PicoC: pio_port_sethigh("port");
static void pio_port_sethigh(pstate *p, val *r, val **param, int n)
{
  int ret = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(ret);

  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PORT_OP, 1);
}

// PicoC: pio_port_setlow("port");
static void pio_port_setlow(pstate *p , val *r, val **param, int n)
{
  int ret = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(ret);

  r->Val->Integer = picoc_pio_gen_setval(ret, PIO_PORT_OP, 0);
}

// PicoC: val = pio_port_getval("port");
static void pio_port_getval(pstate *p, val *r, val **param, int n)
{
  pio_type value;
  int v, port;

  v = pio_value_parse(param[0]->Val->Identifier);
  PIO_CHECK(v);
  port = PLATFORM_IO_GET_PORT(v);
  
  if (!PLATFORM_IO_IS_PORT(v) || !platform_pio_has_port(port)) {
    pmod_error("Invalid port");
  } else {
    value = platform_pio_op(port, PLATFORM_IO_ALL_PINS, PLATFORM_IO_PORT_GET_VALUE);
    r->Val->UnsignedInteger = value;
  }
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

// Rotable for platform variables.
#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE pio_variables[] = {
  {STRKEY("pio_INPUT"), INT(pio_input)},
  {STRKEY("pio_OUTPUT"), INT(pio_output)},
  {STRKEY("pio_PULLUP"), INT(pio_pull_up)},
  {STRKEY("pio_PULLDOWN"), INT(pio_pull_down)},
  {STRKEY("pio_NOPULL"), INT(pio_no_pull)},
  {NILKEY, NILVAL}
};
#endif

// A list of all library functions and their prototypes.
const PICOC_REG_TYPE pio_library[] = {
  // pin functions.
  {FUNC(pio_pin_setdir), PROTO("int pio_pin_setdir(int, char *);")},
  {FUNC(pio_pin_setpull), PROTO("int pio_pin_setpull(int, char *);")},
  {FUNC(pio_pin_setval), PROTO("int pio_pin_setval(unsigned int, char *);")},
  {FUNC(pio_pin_sethigh), PROTO("int pio_pin_sethigh(char *);")},
  {FUNC(pio_pin_setlow), PROTO("int pio_pin_setlow(char *);")},
  {FUNC(pio_pin_getval), PROTO("unsigned int pio_pin_getval(char *);")},

  // port functions.
  {FUNC(pio_port_setdir), PROTO("int pio_port_setdir(int, char *);")},
  {FUNC(pio_port_setpull), PROTO("int pio_port_setpull(int, char *);")},
  {FUNC(pio_port_setval), PROTO("int pio_port_setval(unsigned int, char *);")},
  {FUNC(pio_port_sethigh), PROTO("int pio_port_sethigh(char *);")},
  {FUNC(pio_port_setlow), PROTO("int pio_port_setlow(char *);")},
  {FUNC(pio_port_getval), PROTO("unsigned int pio_port_getval(char *);")},

  // decode function.
  {FUNC(picoc_pio_decode), PROTO("int pio_decode(int, int *, int *);")},
  {NILFUNC, NILPROTO}
};

/* init library */
extern void pio_library_init(void)
{
  REGISTER("pio.h", &pio_lib_setup_func, &pio_library[0]);
}

#endif // ALCOR_LANG_PICOC

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// PIO module for Lua.

// Helper function: pin operations
// Gets the stack index of the first pin and the operation
static int pioh_set_pins( lua_State* L, int stackidx, int op )
{
  int total = lua_gettop( L );
  int i, v, port, pin;
  
  pioh_clear_masks();
  
  // Get all masks
  for( i = stackidx; i <= total; i ++ )
  {
    v = luaL_checkinteger( L, i );
    port = PLATFORM_IO_GET_PORT( v );
    pin = PLATFORM_IO_GET_PIN( v );
    if( PLATFORM_IO_IS_PORT( v ) || !platform_pio_has_port( port ) || !platform_pio_has_pin( port, pin ) )
      return luaL_error( L, "invalid pin" );
    pio_masks[ port ] |= 1 << pin;
  }
  
  // Ask platform to execute the given operation
  for( i = 0; i < PLATFORM_IO_PORTS; i ++ )
    if( pio_masks[ i ] )
      if( !platform_pio_op( i, pio_masks[ i ], op ) )
        return luaL_error( L, "invalid PIO operation" );
  return 0;
}

// Helper function: port operations
// Gets the stack index of the first port and the operation (also the mask)
static int pioh_set_ports( lua_State* L, int stackidx, int op, pio_type mask )
{
  int total = lua_gettop( L );
  int i, v, port;
  u32 port_mask = 0;

  // Get all masks
  for( i = stackidx; i <= total; i ++ )
  {
    v = luaL_checkinteger( L, i );
    port = PLATFORM_IO_GET_PORT( v );
    if( !PLATFORM_IO_IS_PORT( v ) || !platform_pio_has_port( port ) )
      return luaL_error( L, "invalid port" );
    port_mask |= ( 1ULL << port );
  }
  
  // Ask platform to execute the given operation
  for( i = 0; i < PLATFORM_IO_PORTS; i ++ )
    if( port_mask & ( 1ULL << i ) )
      if( !platform_pio_op( i, mask, op ) )
        return luaL_error( L, "invalid PIO operation" );
  return 0;
}

// ****************************************************************************
// Pin/port helper functions

static int pio_gen_setdir( lua_State *L, int optype )
{
  int op = luaL_checkinteger( L, 1 );

  if( op == PIO_DIR_INPUT )
    op = optype == PIO_PIN_OP ? PLATFORM_IO_PIN_DIR_INPUT : PLATFORM_IO_PORT_DIR_INPUT;
  else if( op == PIO_DIR_OUTPUT )
    op = optype == PIO_PIN_OP ? PLATFORM_IO_PIN_DIR_OUTPUT : PLATFORM_IO_PORT_DIR_OUTPUT;
  else
    return luaL_error( L, "invalid direction" );
  if( optype == PIO_PIN_OP )
    pioh_set_pins( L, 2, op );
  else
    pioh_set_ports( L, 2, op, PLATFORM_IO_ALL_PINS );
  return 0;
}

static int pio_gen_setpull( lua_State *L, int optype )
{
  int op = luaL_checkinteger( L, 1 );

  if( ( op != PLATFORM_IO_PIN_PULLUP ) &&
      ( op != PLATFORM_IO_PIN_PULLDOWN ) &&
      ( op != PLATFORM_IO_PIN_NOPULL ) )
    return luaL_error( L, "invalid pull type" );
  if( optype == PIO_PIN_OP )
    pioh_set_pins( L, 2, op );
  else
    pioh_set_ports( L, 2, op, PLATFORM_IO_ALL_PINS );
  return 0;
}

static int pio_gen_setval( lua_State *L, int optype, pio_type val, int stackidx )
{
  if( ( optype == PIO_PIN_OP ) && ( val != 1 ) && ( val != 0 ) ) 
    return luaL_error( L, "invalid pin value" );
  if( optype == PIO_PIN_OP )
    pioh_set_pins( L, stackidx, val == 1 ? PLATFORM_IO_PIN_SET : PLATFORM_IO_PIN_CLEAR );
  else
    pioh_set_ports( L, stackidx, PLATFORM_IO_PORT_SET_VALUE, val );
  return 0;
}

// ****************************************************************************
// Pin operations

// Lua: pio.pin.setdir( pio.INPUT | pio.OUTPUT, pin1, pin2, ..., pinn )
static int pio_pin_setdir( lua_State *L )
{
  return pio_gen_setdir( L, PIO_PIN_OP );
}

// Lua: pio.pin.setpull( pio.PULLUP | pio.PULLDOWN | pio.NOPULL, pin1, pin2, ..., pinn )
static int pio_pin_setpull( lua_State *L )
{
  return pio_gen_setpull( L, PIO_PIN_OP );
}

// Lua: pio.pin.setval( 0|1, pin1, pin2, ..., pinn )
static int pio_pin_setval( lua_State *L )
{
  pio_type val = ( pio_type )luaL_checkinteger( L, 1 );

  return pio_gen_setval( L, PIO_PIN_OP, val, 2 );
}

// Lua: pio.pin.sethigh( pin1, pin2, ..., pinn )
static int pio_pin_sethigh( lua_State *L )
{
  return pio_gen_setval( L, PIO_PIN_OP, 1, 1 );
}

// Lua: pio.pin.setlow( pin1, pin2, ..., pinn )
static int pio_pin_setlow( lua_State *L )
{
  return pio_gen_setval( L, PIO_PIN_OP, 0, 1 );
}

// Lua: pin1, pin2, ..., pinn = pio.pin.getval( pin1, pin2, ..., pinn )
static int pio_pin_getval( lua_State *L )
{
  pio_type value;
  int v, i, port, pin;
  int total = lua_gettop( L );
  
  for( i = 1; i <= total; i ++ )
  {
    v = luaL_checkinteger( L, i );  
    port = PLATFORM_IO_GET_PORT( v );
    pin = PLATFORM_IO_GET_PIN( v );
    if( PLATFORM_IO_IS_PORT( v ) || !platform_pio_has_port( port ) || !platform_pio_has_pin( port, pin ) )
      return luaL_error( L, "invalid pin" );
    else
    {
      value = platform_pio_op( port, 1 << pin, PLATFORM_IO_PIN_GET );
      lua_pushinteger( L, value );
    }
  }
  return total;
}

// ****************************************************************************
// Port operations

// Lua: pio.port.setdir( pio.INPUT | pio.OUTPUT, port1, port2, ..., portn )
static int pio_port_setdir( lua_State *L )
{
  return pio_gen_setdir( L, PIO_PORT_OP );
}

// Lua: pio.port.setpull( pio.PULLUP | pio.PULLDOWN | pio.NOPULL, port1, port2, ..., portn )
static int pio_port_setpull( lua_State *L )
{
  return pio_gen_setpull( L, PIO_PORT_OP );
}

// Lua: pio.port.setval( value, port1, port2, ..., portn )
static int pio_port_setval( lua_State *L )
{
  pio_type val = ( pio_type )luaL_checkinteger( L, 1 );

  return pio_gen_setval( L, PIO_PORT_OP, val, 2 );
}

// Lua: pio.port.sethigh( port1, port2, ..., portn )
static int pio_port_sethigh( lua_State *L )
{
  return pio_gen_setval( L, PIO_PORT_OP, PLATFORM_IO_ALL_PINS, 1 );
}

// Lua: pio.port.setlow( port1, port2, ..., portn )
static int pio_port_setlow( lua_State *L )
{
  return pio_gen_setval( L, PIO_PORT_OP, 0, 1 );
}

// Lua: val1, val2, ..., valn = pio.port.getval( port1, port2, ..., portn )
static int pio_port_getval( lua_State *L )
{
  pio_type value;
  int v, i, port;
  int total = lua_gettop( L );
  
  for( i = 1; i <= total; i ++ )
  {
    v = luaL_checkinteger( L, i );  
    port = PLATFORM_IO_GET_PORT( v );
    if( !PLATFORM_IO_IS_PORT( v ) || !platform_pio_has_port( port ) )
      return luaL_error( L, "invalid port" );
    else
    {
      value = platform_pio_op( port, PLATFORM_IO_ALL_PINS, PLATFORM_IO_PORT_GET_VALUE );
      lua_pushinteger( L, value );
    }
  }
  return total;
}

// *****************************************************************************
// Pin function map

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
static const LUA_REG_TYPE pio_pin_map[] =
{
  { LSTRKEY( "setdir" ), LFUNCVAL ( pio_pin_setdir ) },
  { LSTRKEY( "setpull" ), LFUNCVAL( pio_pin_setpull ) },
  { LSTRKEY( "setval" ), LFUNCVAL( pio_pin_setval ) },
  { LSTRKEY( "sethigh" ), LFUNCVAL( pio_pin_sethigh ) },
  { LSTRKEY( "setlow" ), LFUNCVAL( pio_pin_setlow ) },
  { LSTRKEY( "getval" ), LFUNCVAL( pio_pin_getval ) },
  { LNILKEY, LNILVAL }
};

static const LUA_REG_TYPE pio_port_map[] =
{
  { LSTRKEY( "setdir" ), LFUNCVAL ( pio_port_setdir ) },
  { LSTRKEY( "setpull" ), LFUNCVAL( pio_port_setpull ) },
  { LSTRKEY( "setval" ), LFUNCVAL( pio_port_setval ) },
  { LSTRKEY( "sethigh" ), LFUNCVAL( pio_port_sethigh ) },
  { LSTRKEY( "setlow" ), LFUNCVAL( pio_port_setlow ) },
  { LSTRKEY( "getval" ), LFUNCVAL( pio_port_getval ) },
  { LNILKEY, LNILVAL }
};

const LUA_REG_TYPE pio_map[] =
{
  // The 'decode' functions returns a port/pin
  // pair from a platform code.
  { LSTRKEY( "decode" ), LFUNCVAL( pio_decode ) },  
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "pin" ), LROVAL( pio_pin_map ) },
  { LSTRKEY( "port" ), LROVAL( pio_port_map ) },
  { LSTRKEY( "INPUT" ), LNUMVAL( PIO_DIR_INPUT ) },
  { LSTRKEY( "OUTPUT" ), LNUMVAL( PIO_DIR_OUTPUT ) },
  { LSTRKEY( "PULLUP" ), LNUMVAL( PLATFORM_IO_PIN_PULLUP ) },
  { LSTRKEY( "PULLDOWN" ), LNUMVAL( PLATFORM_IO_PIN_PULLDOWN ) },
  { LSTRKEY( "NOPULL" ), LNUMVAL( PLATFORM_IO_PIN_NOPULL ) },
  { LSTRKEY( "__metatable" ), LROVAL( pio_map ) },
#endif
  // The __index metamethod will return pin/port
  // numeric identifiers
  { LSTRKEY( "__index" ), LFUNCVAL( pio_mt_index ) },
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_pio( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_PIO, pio_map );

  // Set it as its own metatable
  lua_pushvalue( L, -1 );
  lua_setmetatable( L, -2 );
  
  // Set constants for direction/pullups
  MOD_REG_NUMBER( L, "INPUT", PIO_DIR_INPUT );
  MOD_REG_NUMBER( L, "OUTPUT", PIO_DIR_OUTPUT );
  MOD_REG_NUMBER( L, "PULLUP", PLATFORM_IO_PIN_PULLUP );
  MOD_REG_NUMBER( L, "PULLDOWN", PLATFORM_IO_PIN_PULLDOWN );
  MOD_REG_NUMBER( L, "NOPULL", PLATFORM_IO_PIN_NOPULL );

  // Setup the new tables (pin and port) inside pio
  lua_newtable( L );
  luaL_register( L, NULL, pio_pin_map );
  lua_setfield( L, -2, "pin" );

  lua_newtable( L );
  luaL_register( L, NULL, pio_port_map );
  lua_setfield( L, -2, "port" );

  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0
}

#endif // ALCOR_LANG_LUA
