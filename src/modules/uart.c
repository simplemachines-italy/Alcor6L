// Module for interfacing with UART
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

// Generic includes.
#include "platform.h"
#include "common.h"
#include "sermux.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "platform_conf.h"

// Modes for the UART read function
enum
{
  UART_READ_MODE_LINE,
  UART_READ_MODE_NUMBER,
  UART_READ_MODE_SPACE,
  UART_READ_MODE_MAXSIZE
};

#define UART_INFINITE_TIMEOUT PLATFORM_TIMER_INF_TIMEOUT

#if defined BUILD_SERMUX
# define MAX_VUART_NAME_LEN    6
# define MIN_VUART_NAME_LEN    6
#endif

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// UART module for picoLisp.

// (uart-setup 'num 'num 'num 'num 'num) -> num
any plisp_uart_setup(any ex) {
  unsigned id, databits, parity, stopbits;
  u32 baud, res;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get uart id.

  MOD_CHECK_ID(ex, uart, id);
  if (id >= SERMUX_SERVICE_ID_FIRST)
    err(ex, y, "uart-setup can't be called on virtual UARTs");

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  baud = unBox(y); // get baud rate.

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  databits = unBox(y); // get num of bits.

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  parity = unBox(y); // get parity.

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  stopbits = unBox(y); // get stopbits.

  res = platform_uart_setup(id, baud, databits, parity, stopbits);
  return res == 0 ? Nil : box(res);
}

// Helpers for picoLisp uart 'write' function.
//
static void outString_uart(unsigned id, char *s) {
  while (*s)
    platform_uart_send(id, *s++);
}

static void outNum_uart(unsigned id, long n) {
  char buf[BITS/2];

  bufNum(buf, n);
  outString_uart(id, buf);
}

static void uarth_prin(unsigned id, any x) {
  if (!isNil(x)) {
    if (isNum(x))
      outNum_uart(id, unBox(x));
    else if (isSym(x)) {
      int i, c;
      word w;
      u8 byte;

      for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
	if (c != '^') {
          byte = c;
          platform_uart_send(id, byte);
        }
        else if (!(c = getByte(&i, &w, &x))) {
          byte = '^';
          platform_uart_send(id, byte);
	}
        else if (c == '?') {
          byte = 127;
          platform_uart_send(id, byte);
        }
        else {
          c &= 0x1F;
          byte = (u8)c;
          platform_uart_send(id, byte);
	}
      }
    }
    else {
      while (uarth_prin(id, car(x)), !isNil(x = cdr(x))) {
	if (!isCell(x)) {
	  uarth_prin(id, x);
          break;
	}
      }
    }
  }
}

// (uart-write 'num 'any ..) -> any
any plisp_uart_write(any ex) {
  unsigned id;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.                                                                                               
  MOD_CHECK_ID(ex, uart, id);

  while (isCell(x = cdr(x)))
    uarth_prin(id, y = EVAL(car(x)));
  return y;
}

// (uart-read 'num 'sym ['num] ['num])
any plisp_uart_read(any ex) {
  int id, res, mode, issign;
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  s32 maxsize = 0, count = 0;
  char *buffer = NULL;
  char cres;
  timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, uart, id);

  // Check format.
  x = cdr(x), y = EVAL(car(x));
  if (isNum(y)) {
    if ((maxsize = unBox(y)) < 0)
      err(ex, y, "invalid max size");
    mode = UART_READ_MODE_MAXSIZE;
  } else {
    if (equal(mkStr("*l"), y))
      mode = UART_READ_MODE_LINE;
    else if (equal(mkStr("*n"), y))
      mode = UART_READ_MODE_NUMBER;
    else if (equal(mkStr("*s"), y))
      mode = UART_READ_MODE_SPACE;
    else
      err(ex, y, "invalid format");
  }
  // Check if we have optional parameters.
  // Collect them if we do. Supplying optional
  // arguments means that we have atleast >
  // 2 parameters.
  if (plen(ex) > 2) {
    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    timeout = unBox(y); // get timeout.
    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    timer_id = unBox(y); // get timer id.
  }

  // Read data
  while (1) {
    buffer = realloc(buffer, 1);
    if ((res = platform_uart_recv(id, timer_id, timeout)) == -1)
      break; 
    cres = (char)res;
    count++;
    issign = (count == 1) && ((res == '-') || (res == '+'));
    // [TODO] this only works for lines that actually end with '\n',
    // other line endings are not supported.
    if ((cres == '\n') && (mode == UART_READ_MODE_LINE))
      break;
    if (!isdigit(cres) && !issign && (mode == UART_READ_MODE_NUMBER))
      break;
    if (isspace(cres) && (mode == UART_READ_MODE_SPACE))
      break;
    buffer[count - 1] = cres;
    if ((count == maxsize) && (mode == UART_READ_MODE_MAXSIZE))
      break;
  }
  buffer[count] = '\0';

  // Return an integer if needed
  if (mode == UART_READ_MODE_NUMBER)
    return box(res);
  else
    return mkStr(buffer);
}

// (uart-set-buffer 'num 'num) -> Nil
any plisp_uart_set_buffer(any ex) {
  int id; u32 size;  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, uart, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  size = (u32)unBox(y); // get size.
  if (size && (size & (size - 1)))
    err(ex, y, "the buffer size must be a power of 2 or 0");
  if (size == 0 && id >= SERMUX_SERVICE_ID_FIRST)
    err(ex, y, "disabling buffers on virtual UARTs is not allowed");

  if (platform_uart_set_buffer(id, intlog2(size) == PLATFORM_ERR))
    err(ex, NULL, "unable to set UART buffer");

  return Nil;
}

// (uart-set-flow-control 'num 'num) -> Nil
any plisp_uart_set_flow_control(any ex) {
  any x, y;
  int id, type;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, uart, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  type = unBox(y); // get type.

  if (platform_uart_set_flow_control(id, type) != PLATFORM_OK)
    err(ex, y, "unable to set the flow control on this interface.");

  return Nil;
}

// (uart-getchar 'num ['num] ['num]) -> Nil | sym
any plisp_uart_getchar(any ex) {
  int id, res;
  char cres[2];
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get uart id.
  MOD_CHECK_ID(ex, uart, id);

  if (plen(ex) > 1) {
    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    timeout = unBox(y); // get timeout.

    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    timer_id = unBox(y); // get timer id.
  }
  res = platform_uart_recv(id, timer_id, timeout);
  if (res) {
    cres[0] = (char)res;
    cres[1] = '\0';
    return mkStr(cres);
  }
  // In case -1 is returned, return Nil. If an empty
  // string is returned, picoLisp will throw an output
  // which might look a bit like this.
  // -> 
  // Nil provides more meaning.
  return Nil;
}

#if defined BUILD_SERMUX

// Look for all VUARTx timer identifiers.
//
// (uart-vuart-tmr-ident 'sym) -> Nil | num
any plisp_uart_vuart_tmr_ident(any ex) {
  char* pend;
  long res;
  any x, y;

  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  char key[bufSize(y)];
  bufString(y, key); // get key.

  if (strlen(key) > MAX_VUART_NAME_LEN ||
      strlen(key) < MIN_VUART_NAME_LEN)
    return Nil;

  if (strncmp(key, "VUART", 5))
    return Nil;

  res = strtol(key + 5, &pend, 10);
  if (*pend != '\0')
    return Nil;

  if (res >= SERMUX_NUM_VUART)
    return Nil;

  return box(SERMUX_SERVICE_ID_FIRST + res);
}

#else

// If SERMUX is not enabled, we have a dummy
// function which just reports the same.
any plisp_uart_vuart_tmr_ident(any ex) {
  err(NULL, NULL, "SERMUX support not enabled in this build.");
  return Nil;
}

#endif // #if defined BUILD_SERMUX

#endif // #if defiind ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// UART module for PicoC.

static const int par_even = PLATFORM_UART_PARITY_EVEN;
static const int par_odd = PLATFORM_UART_PARITY_ODD;
static const int par_none = PLATFORM_UART_PARITY_NONE;
static const int stop_1 = PLATFORM_UART_STOPBITS_1;
static const int stop_1_5 = PLATFORM_UART_STOPBITS_1_5;
static const int stop_2 = PLATFORM_UART_STOPBITS_2;
static const int no_timeout = 0;
static const int inf_timeout = UART_INFINITE_TIMEOUT;
static const int flow_none = PLATFORM_UART_FLOW_NONE;
static const int flow_rts = PLATFORM_UART_FLOW_RTS;
static const int flow_cts = PLATFORM_UART_FLOW_CTS;

// Library setup function
extern void uart_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("uart_PAR_EVEN", par_even);
  picoc_def_integer("uart_PAR_ODD", par_odd);
  picoc_def_integer("uart_PAR_NONE", par_none);
  picoc_def_integer("uart_STOP_1", stop_1);
  picoc_def_integer("uart_STOP_1_5", stop_1_5);
  picoc_def_integer("uart_STOP_2", stop_2);
  picoc_def_integer("uart_NO_TIMEOUT", no_timeout);
  picoc_def_integer("uart_INF_TIMEOUT", inf_timeout);
  picoc_def_integer("uart_FLOW_NONE", flow_none);
  picoc_def_integer("uart_FLOW_RTS", flow_rts);
  picoc_def_integer("uart_FLOW_CTS", flow_cts);
#endif
}

// Helper to get timeout and timer_id values.
static void uart_get_timeout_data(timer_data_type *timeout,
				  unsigned *timer_id,
				  val **param,
				  int timeout_index,
				  int timer_id_index)
{
  *timeout = param[timeout_index]->Val->UnsignedLongInteger;
  if (*timeout < 0 || *timeout > PLATFORM_TIMER_INF_TIMEOUT)
    return pmod_error("invalid timeout value");
  *timer_id = param[timer_id_index]->Val->UnsignedInteger;
  if (*timer_id == PLATFORM_TIMER_SYS_ID &&
      !platform_timer_sys_available())
    return pmod_error("the system timer is not implemented on this platform");
}

// PicoC: uart_setup(id, baud, databits, parity, stopbits);
static void uart_setup(pstate *p, val *r, val **param, int n)
{
  unsigned id, databits, parity, stopbits;
  u32 baud, res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(uart, id);
  if (id >= SERMUX_SERVICE_ID_FIRST)
    return pmod_error("uart_setup can't be called on virtual UARTs");
  baud = param[1]->Val->UnsignedLongInteger;
  databits = param[2]->Val->UnsignedInteger;
  parity = param[3]->Val->UnsignedInteger;
   stopbits = param[4]->Val->UnsignedInteger;
  res = platform_uart_setup(id, baud, databits, parity, stopbits);
  r->Val->UnsignedLongInteger = res;
}

// PicoC: uart_write_num(id, ival);
static void uart_write_num(pstate *p, val *r, val **param, int n)
{
  unsigned id, nval;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(uart, id);
  nval = param[1]->Val->UnsignedInteger;
  if ((n < 0) || (n > 255))
    return pmod_error("invalid number");
  platform_uart_send(id, (u8)nval);
}

// PicoC: uart_write_str(id, str, len);
static void uart_write_str(pstate *p, val *r, val **param, int n)
{
  unsigned id, len, i;
  char *buf;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(uart, id);
  buf = param[1]->Val->Identifier;
  len = param[2]->Val->UnsignedInteger;

  for (i = 0; i < len; i++)
    platform_uart_send(id, buf[i]);
}

// PicoC: integer = uart_read_num(id, timeout, timer_id);
static void uart_read_num(pstate *p, val *r, val **param, int n)
{
  int id, res, issign;
  unsigned timer_id;
  s32 count = 0;
  char cres;
  timer_data_type timeout;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(uart, id);

  // Get timeout and timer id.
  uart_get_timeout_data(&timeout, &timer_id, param, 1, 2);
 
  // Read data.
  while (1) {
    if ((res = platform_uart_recv(id, timer_id, timeout)) == -1)
      break;
    cres = (char)res;
    count++;
    issign = (count == 1) && ((res == '-') || (res == '+'));
    if (!isdigit(cres) && !issign)
      break;
  }
  r->Val->Integer = res;
}

// PicoC: data = uart_readn(id, max_num, timeout, timer_id);
static void uart_readn(pstate *p, val *r, val **param, int n)
{
  int id, res;
  unsigned timer_id;
  s32 maxsize = 0;
  char cres; char *buf;
  timer_data_type timeout;
  FILE *buf_stream;
  size_t buf_size;

  buf_stream = open_memstream(&buf, &buf_size);

  id = param[0]->Val->Integer;
  MOD_CHECK_ID(uart, id);

  // Get maxsize and check if it is valid.
  maxsize = param[1]->Val->LongInteger;
  if (maxsize < 0)
    return pmod_error("invalid max size");

  // Get timer and timeout information.
  uart_get_timeout_data(&timeout, &timer_id, param, 2, 3);

  // Read data.
  while (1) {
    if ((res = platform_uart_recv(id, timer_id, timeout)) == -1)
      break;
    cres = (char)res;
    fprintf(buf_stream, "%c", cres);
    if (buf_size == maxsize)
      break;
  }
  fflush(buf_stream);
  fclose(buf_stream);
  r->Val->Identifier = buf;
}

// PicoC: data = uart_read_space(id, timeout, timer_id);
static void uart_read_space(pstate *p, val *r, val **param, int n)
{
  int id, res;
  unsigned timer_id;
  timer_data_type timeout;
  char cres = 0;

  id = param[0]->Val->Integer;
  MOD_CHECK_ID(uart, id);
  uart_get_timeout_data(&timeout, &timer_id, param, 1, 2);

  while (1) {
    if ((res = platform_uart_recv(id, timer_id, timeout)) == -1)
      break;
    cres = (char)res;
    if (isspace(cres))
      break;
  }
  r->Val->Character = cres;
}

// PicoC: line = uart_read_line(id, timeout, timer_id);
static void uart_read_line(pstate *p, val *r, val **param, int n)
{
  int id, res;
  unsigned timer_id;
  char cres; char *buf;
  timer_data_type timeout;
  FILE *buf_stream;
  size_t buf_size;

  buf_stream = open_memstream(&buf, &buf_size);

  id = param[0]->Val->Integer;
  MOD_CHECK_ID(uart, id);
  uart_get_timeout_data(&timeout, &timer_id, param, 1, 2);

  while (1) {
    if ((res = platform_uart_recv(id, timer_id, timeout)) == -1)
      break;
    cres = (char)res;
    // [TODO] this only works for lines that actually end
    // with '\n', other line endings are not supported.
    if (cres == '\n')
      break;
    fprintf(buf_stream, "%c", cres);
  }
  fflush(buf_stream);
  fclose(buf_stream);
  r->Val->Identifier = buf;
}

// PicoC: data = uart_getchar(id, timeout, timer_id);
static void uart_getchar(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  int res;
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  timer_data_type timeout;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(uart, id);
  uart_get_timeout_data(&timeout, &timer_id, param, 1, 2);

  res = platform_uart_recv(id, timer_id, timeout);
  if (res == -1)
    r->Val->Character = 0;
  else
    r->Val->Character = res;
}

// PicoC: uart_set_buffer(id, size);
static void uart_set_buffer(pstate *p, val *r, val **param, int n)
{
  int id = param[0]->Val->Integer;
  u32 size = param[1]->Val->UnsignedLongInteger;
  
  MOD_CHECK_ID(uart, id);
  if (size && (size & (size - 1)))
    return pmod_error("the buffer size must be a power of 2 or 0");
  if (size == 0 && id >= SERMUX_SERVICE_ID_FIRST)
    return pmod_error("disabling buffers on virtual UARTs is not allowed");
  if (platform_uart_set_buffer(id, intlog2(size) == PLATFORM_ERR))
    return pmod_error("unable to set UART buffer");
}

// PicoC: uart_set_flow_control(id, type);
static void uart_set_flow_control(pstate *p, val *r, val **param, int n)
{
  int id = param[0]->Val->Integer;
  int type = param[1]->Val->Integer;
  
  MOD_CHECK_ID(uart, id);
  if (platform_uart_set_flow_control(id, type) != PLATFORM_OK)
    return ProgramFail(NULL, "unable to set the flow control on interface %d", id);
}

#ifdef BUILD_SERMUX

#define MAX_VUART_NAME_LEN    6
#define MIN_VUART_NAME_LEN    6

static void uart_decode(pstate *p, val *r, val **param, int n)
{
  char *key = param[0]->Val->Identifier;
  char *pend;
  long res;

  if (strlen(key) > MAX_VUART_NAME_LEN || strlen(key) < MIN_VUART_NAME_LEN) {
    r->Val->LongInteger = 0;
    return;
  }
  if (strncmp(key, "VUART", 5)) {
    r->Val->LongInteger = 0;
    return;
  }
  res = strtol(key + 5, &pend, 10);
  if (*pend != '\0') {
    r->Val->LongInteger = 0;
    return;
  }
  if (res >= SERMUX_NUM_VUART) {
    r->Val->LongInteger = 0;
    return;
  }
  r->Val->LongInteger = SERMUX_SERVICE_ID_FIRST + res;
}

#endif // #ifdef BUILD_SERMUX

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE uart_variables[] = {
  {STRKEY("uart_PAR_EVEN"), INT(par_even)},
  {STRKEY("uart_PAR_ODD"), INT(par_odd)},
  {STRKEY("uart_PAR_NONE"), INT(par_none)},
  {STRKEY("uart_STOP_1"), INT(stop_1)},
  {STRKEY("uart_STOP_1_5"), INT(stop_1_5)},
  {STRKEY("uart_STOP_2"), INT(stop_2)},
  {STRKEY("uart_NO_TIMEOUT"), INT(no_timeout)},
  {STRKEY("uart_INF_TIMEOUT"), INT(inf_timeout)},
  {STRKEY("uart_FLOW_NONE"), INT(flow_none)},
  {STRKEY("uart_FLOW_RTS"), INT(flow_rts)},
  {STRKEY("uart_FLOW_CTS"), INT(flow_cts)},
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE uart_library[] = {
  {FUNC(uart_setup), PROTO("unsigned long uart_setup(unsigned int, unsigned long,"
			   "unsigned int, unsigned int, unsigned int);")},
  {FUNC(uart_write_num), PROTO("void uart_write_num(unsigned int, unsigned int);")},
  {FUNC(uart_write_str), PROTO("void uart_write_str(unsigned int, char *, unsigned int);")},
  {FUNC(uart_read_num), PROTO("int uart_read_num(int, unsigned long, unsigned int);")},
  {FUNC(uart_readn), PROTO("char *uart_readn(int, long, unsigned long, unsigned int);")},
  {FUNC(uart_read_space), PROTO("char uart_read_space(int, unsigned long, unsigned int);")},
  {FUNC(uart_read_line), PROTO("char *uart_read_line(int, unsigned long, unsigned int);")},
  {FUNC(uart_getchar), PROTO("char uart_getchar(unsigned int, unsigned long, unsigned int);")},
  {FUNC(uart_set_buffer), PROTO("void uart_set_buffer(unsigned int, unsigned long);")},
  {FUNC(uart_set_flow_control), PROTO("void uart_set_flow_control(int, int);")},
#ifdef BUILD_SERMUX
  {FUNC(uart_decode), PROTO("unsigned long uart_decode(char *);")},
#endif
  {NILFUNC, NILPROTO}
};

// Init library.
extern void uart_library_init(void)
{
  REGISTER("uart.h", uart_lib_setup_func,
	   &uart_library[0]);
}

#endif

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// UART module for Lua.

// Helper function, the same as cmn_get_timeout_data() but with the
// parameters in the order required by the uart module.

static void uart_get_timeout_data( lua_State *L, int pidx, timer_data_type *ptimeout, unsigned *pid )
{
  lua_Number tempn;

  *ptimeout = PLATFORM_TIMER_INF_TIMEOUT;
  if( lua_type( L, pidx ) == LUA_TNUMBER )
  {
    tempn = lua_tonumber( L, pidx );
    if( tempn < 0 || tempn > PLATFORM_TIMER_INF_TIMEOUT )
      luaL_error( L, "invalid timeout value" );
    *ptimeout = ( timer_data_type )tempn;
  }
  *pid = ( unsigned )luaL_optinteger( L, pidx + 1, PLATFORM_TIMER_SYS_ID );
  if( *pid == PLATFORM_TIMER_SYS_ID && !platform_timer_sys_available() )
    luaL_error( L, "the system timer is not implemented on this platform" );
}


// Lua: actualbaud = setup( id, baud, databits, parity, stopbits )
static int uart_setup( lua_State* L )
{
  unsigned id, databits, parity, stopbits;
  u32 baud, res;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( uart, id );
  if( id >= SERMUX_SERVICE_ID_FIRST )
    return luaL_error( L, "uart.setup can't be called on virtual UARTs" );
  baud = luaL_checkinteger( L, 2 );
  databits = luaL_checkinteger( L, 3 );
  parity = luaL_checkinteger( L, 4 );
  stopbits = luaL_checkinteger( L, 5 );
  res = platform_uart_setup( id, baud, databits, parity, stopbits );
  lua_pushinteger( L, res );
  return 1;
}

// Lua: write( id, string1, [string2], ..., [stringn] )
static int uart_write( lua_State* L )
{
  int id;
  const char* buf;
  size_t len, i;
  int total = lua_gettop( L ), s;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( uart, id );
  for( s = 2; s <= total; s ++ )
  {
    if( lua_type( L, s ) == LUA_TNUMBER )
    {
      len = lua_tointeger( L, s );
      if( ( len < 0 ) || ( len > 255 ) )
        return luaL_error( L, "invalid number" );
      platform_uart_send( id, ( u8 )len );
    }
    else
    {
      luaL_checktype( L, s, LUA_TSTRING );
      buf = lua_tolstring( L, s, &len );
      for( i = 0; i < len; i ++ )
        platform_uart_send( id, buf[ i ] );
    }
  }
  return 0;
}

// Lua: uart.read( id, format, [timeout], [timer_id] )
static int uart_read( lua_State* L )
{
  int id, res, mode, issign;
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  s32 maxsize = 0, count = 0;
  const char *fmt;
  luaL_Buffer b;
  char cres;
  timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( uart, id );

  // Check format
  if( lua_isnumber( L, 2 ) )
  {
    if( ( maxsize = ( s32 )lua_tointeger( L, 2 ) ) < 0 )
      return luaL_error( L, "invalid max size" );
    mode = UART_READ_MODE_MAXSIZE;
  }
  else
  {
    fmt = luaL_checkstring( L, 2 );
    if( !strcmp( fmt, "*l" ) )
      mode = UART_READ_MODE_LINE;
    else if( !strcmp( fmt, "*n" ) )
      mode = UART_READ_MODE_NUMBER;
    else if( !strcmp( fmt, "*s" ) )
      mode = UART_READ_MODE_SPACE;
    else
      return luaL_error( L, "invalid format" );
  }

  // Check timeout and timer id
  uart_get_timeout_data( L, 3, &timeout, &timer_id );

  // Read data
  luaL_buffinit( L, &b );
  while( 1 )
  {
    if( ( res = platform_uart_recv( id, timer_id, timeout ) ) == -1 )
      break; 
    cres = ( char )res;
    count ++;
    issign = ( count == 1 ) && ( ( res == '-' ) || ( res == '+' ) );
    // [TODO] this only works for lines that actually end with '\n', other line endings
    // are not supported.
    if( ( cres == '\n' ) && ( mode == UART_READ_MODE_LINE ) )
      break;
    if( !isdigit( cres ) && !issign && ( mode == UART_READ_MODE_NUMBER ) )
      break;
    if( isspace( cres ) && ( mode == UART_READ_MODE_SPACE ) )
      break;
    luaL_putchar( &b, cres );
    if( ( count == maxsize ) && ( mode == UART_READ_MODE_MAXSIZE ) )
      break;
  }
  luaL_pushresult( &b );

  // Return an integer if needed
  if( mode == UART_READ_MODE_NUMBER )
  {
    res = lua_tointeger( L, -1 );
    lua_pop( L, 1 );
    lua_pushinteger( L, res );
  }
  return 1;  
}

// Lua: data = getchar( id, [ timeout ], [ timer_id ] )
static int uart_getchar( lua_State* L )
{
  int id, res;
  char cres;
  unsigned timer_id = PLATFORM_TIMER_SYS_ID;
  timer_data_type timeout = PLATFORM_TIMER_INF_TIMEOUT;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( uart, id );
  // Check timeout and timer id
  uart_get_timeout_data( L, 2, &timeout, &timer_id );
  res = platform_uart_recv( id, timer_id, timeout );
  if( res == -1 )
    lua_pushstring( L, "" );
  else
  {
    cres = ( char )res;
    lua_pushlstring( L, &cres, 1 );
  }
  return 1;  
}


// Lua: uart.set_buffer( id, size )
static int uart_set_buffer( lua_State *L )
{
  int id = luaL_checkinteger( L, 1 );
  u32 size = ( u32 )luaL_checkinteger( L, 2 );
  
  MOD_CHECK_ID( uart, id );
  if( size && ( size & ( size - 1 ) ) )
    return luaL_error( L, "the buffer size must be a power of 2 or 0" );
  if( size == 0 && id >= SERMUX_SERVICE_ID_FIRST )
    return luaL_error( L, "disabling buffers on virtual UARTs is not allowed" );
  if( platform_uart_set_buffer( id, intlog2( size ) ) == PLATFORM_ERR )
    return luaL_error( L, "unable to set UART buffer" );
  return 0;
}

// Lua: uart.set_flow_control( id, type )
static int uart_set_flow_control( lua_State *L )
{
  int id = luaL_checkinteger( L, 1 );
  int type = luaL_checkinteger( L, 2 );

  MOD_CHECK_ID( uart, id );
  if( platform_uart_set_flow_control( id, type ) != PLATFORM_OK )
    return luaL_error( L, "unable to set the flow control on interface %d", id );
  return 0;
}

#ifdef BUILD_SERMUX

#define MAX_VUART_NAME_LEN    6
#define MIN_VUART_NAME_LEN    6

// __index metafunction for UART
// Look for all VUARTx timer identifiers
static int uart_mt_index( lua_State* L )
{
  const char *key = luaL_checkstring( L ,2 );
  char* pend;
  long res;
  
  if( strlen( key ) > MAX_VUART_NAME_LEN || strlen( key ) < MIN_VUART_NAME_LEN )
    return 0;
  if( strncmp( key, "VUART", 5 ) )
    return 0;  
  res = strtol( key + 5, &pend, 10 );
  if( *pend != '\0' )
    return 0;
  if( res >= SERMUX_NUM_VUART )
    return 0;
  lua_pushinteger( L, SERMUX_SERVICE_ID_FIRST + res );
  return 1;
}
#endif // #ifdef BUILD_SERMUX

// Module function map
#define MIN_OPT_LEVEL   2
#include "lrodefs.h"
const LUA_REG_TYPE uart_map[] = 
{
  { LSTRKEY( "setup" ),  LFUNCVAL( uart_setup ) },
  { LSTRKEY( "write" ), LFUNCVAL( uart_write ) },
  { LSTRKEY( "read" ), LFUNCVAL( uart_read ) },
  { LSTRKEY( "getchar" ), LFUNCVAL( uart_getchar ) },
  { LSTRKEY( "set_buffer" ), LFUNCVAL( uart_set_buffer ) },
  { LSTRKEY( "set_flow_control" ), LFUNCVAL( uart_set_flow_control ) },
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "PAR_EVEN" ), LNUMVAL( PLATFORM_UART_PARITY_EVEN ) },
  { LSTRKEY( "PAR_ODD" ), LNUMVAL( PLATFORM_UART_PARITY_ODD ) },
  { LSTRKEY( "PAR_NONE" ), LNUMVAL( PLATFORM_UART_PARITY_NONE ) },
  { LSTRKEY( "STOP_1" ), LNUMVAL( PLATFORM_UART_STOPBITS_1 ) },
  { LSTRKEY( "STOP_1_5" ), LNUMVAL( PLATFORM_UART_STOPBITS_1_5 ) },
  { LSTRKEY( "STOP_2" ), LNUMVAL( PLATFORM_UART_STOPBITS_2 ) },
  { LSTRKEY( "NO_TIMEOUT" ), LNUMVAL( 0 ) },
  { LSTRKEY( "INF_TIMEOUT" ), LNUMVAL( UART_INFINITE_TIMEOUT ) },
  { LSTRKEY( "FLOW_NONE" ), LNUMVAL( PLATFORM_UART_FLOW_NONE ) },
  { LSTRKEY( "FLOW_RTS" ), LNUMVAL( PLATFORM_UART_FLOW_RTS ) },
  { LSTRKEY( "FLOW_CTS" ), LNUMVAL( PLATFORM_UART_FLOW_CTS ) },
#endif
#if LUA_OPTIMIZE_MEMORY > 0 && defined( BUILD_SERMUX )
  { LSTRKEY( "__metatable" ), LROVAL( uart_map ) },
  { LSTRKEY( "__index" ), LFUNCVAL( uart_mt_index ) },  
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_uart( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_UART, uart_map );
  
  MOD_REG_NUMBER( L, "PAR_EVEN", PLATFORM_UART_PARITY_EVEN );
  MOD_REG_NUMBER( L, "PAR_ODD", PLATFORM_UART_PARITY_ODD );
  MOD_REG_NUMBER( L, "PAR_NONE", PLATFORM_UART_PARITY_NONE );
  MOD_REG_NUMBER( L, "STOP_1", PLATFORM_UART_STOPBITS_1 );
  MOD_REG_NUMBER( L, "STOP_1_5", PLATFORM_UART_STOPBITS_1_5 );
  MOD_REG_NUMBER( L, "STOP_2", PLATFORM_UART_STOPBITS_2 );
  MOD_REG_NUMBER( L, "NO_TIMEOUT", 0 );
  MOD_REG_NUMBER( L, "INF_TIMEOUT", UART_INFINITE_TIMEOUT );
  MOD_REG_NUMBER( L, "FLOW_NONE", PLATFORM_UART_FLOW_NONE );
  MOD_REG_NUMBER( L, "FLOW_RTS", PLATFORM_UART_FLOW_RTS );
  MOD_REG_NUMBER( L, "FLOW_CTS", PLATFORM_UART_FLOW_CTS );
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0
}

#endif // #ifdef ALCOR_LANG_LUA
