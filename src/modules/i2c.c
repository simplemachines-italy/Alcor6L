// Module for interfacing with the I2C interface
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
#include <string.h>
#include <ctype.h>

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// I2C module for picoLisp.

// (i2c-setup 'num 'num) -> num
any plisp_i2c_setup(any ex) {
  any x, y;
  unsigned id;
  s32 speed;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, i2c, id);

  x = cdr(x);
  NeedNum(x, y = EVAL(car(x)));
  speed = unBox(y);
  if (speed <= 0)
    err(ex, y, "frequency must be > 0");

  return box(platform_i2c_setup(id, (u32)speed));
}

// (i2c-start 'num) -> Nil
any plisp_i2c_start(any ex) {
  unsigned id;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, i2c, id);
  platform_i2c_send_start(id);

  return Nil;
}

// (i2c-stop 'num) -> Nil
any plisp_i2c_stop(any ex) {
  unsigned id;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, i2c, id);
  platform_i2c_send_stop(id);

  return Nil;
}

// (i2c-address 'num 'num 'num) -> num
any plisp_i2c_address(any ex) {
  unsigned id;
  int add, dir, ret;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.  
  MOD_CHECK_ID(ex, i2c, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  add = unBox(y); // get address.

  // check address.
  if (add < 0 || add > 127)
    err(ex, y, "slave address must be from 0 to 127");

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  dir = unBox(y); // get direction.
  
  ret = platform_i2c_send_address(id, (u16)add, dir);
  return box(ret);

}

// Helpers for picoLisp i2c 'write' function.

static void outString_i2c(unsigned id, char *s) {
  while (*s)
    platform_i2c_send_byte(id, *s++);
}

static void outNum_i2c(unsigned id, long n) {
  char buf[BITS/2];

  bufNum(buf, n);
  outString_i2c(id, buf);
}

static void i2ch_prin(unsigned id, any x) {
  if (!isNil(x)) {
    if (isNum(x))
      outNum_i2c(id, unBox(x));
    else if (isSym(x)) {
      int i, c;
      word w;
      u8 byte;
 
      for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
        if (c != '^') {
          byte = c;
          platform_i2c_send_byte(id, byte);
        }
        else if (!(c = getByte(&i, &w, &x))) {
          byte = '^';
	  platform_i2c_send_byte(id, byte);
	}
        else if (c == '?') {
          byte = 127;
	  platform_i2c_send_byte(id, byte);
        }
        else {
          c &= 0x1F;
          byte = (u8)c;
	  platform_i2c_send_byte(id, byte);
        }
      }
    }
    else {
      while (i2ch_prin(id, car(x)), !isNil(x = cdr(x))) {
        if (!isCell(x)) {
	  i2ch_prin(id, x);
          break;
	}
      }
    }
  }
}

// (i2c-write 'num 'any ..) -> any
any plisp_i2c_write(any ex) {
  unsigned id;
  any x, y = Nil;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, i2c, id);

  while (isCell(x = cdr(x)))
    i2ch_prin(id, y = EVAL(car(x)));
  return y;
}

// (i2c-read 'num 'num) -> sym
any plisp_i2c_read(any ex) {
  unsigned id;
  u32 size, i, count = 0;
  int data;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, i2c, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  size = unBox(y); // get size.
  char *b = malloc(size + 1);

  if (size == 0)
    return Nil;
  for (i = 0; i < size; i++) {
    if ((data = platform_i2c_recv_byte(id, i < size - 1)) == -1)
      break;
    else
      b[count++] = (char)data;
  }
  return mkStr(b);
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// I2C module for PicoC.

// A few platform constants.
static const int fast = PLATFORM_I2C_SPEED_FAST;
static const int slow = PLATFORM_I2C_SPEED_SLOW;
static const int trans = PLATFORM_I2C_DIRECTION_TRANSMITTER;
static const int rec = PLATFORM_I2C_DIRECTION_RECEIVER;

// Library setup function
extern void i2c_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("i2c_FAST", fast);
  picoc_def_integer("i2c_SLOW", slow);
  picoc_def_integer("i2c_TRANSMITTER", trans);
  picoc_def_integer("i2c_RECEIVER", rec);
#endif
}
                                                            
// PicoC: i2c_setup(id, speed);
static void i2c_setup(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;
  s32 speed = (s32)param[1]->Val->LongInteger;
  
  MOD_CHECK_ID(i2c, id);
  if (speed <= 0)
    return pmod_error("frequency must be > 0");

  r->Val->UnsignedLongInteger = platform_i2c_setup(id, (u32)speed);
}

// PicoC: i2c_start(id);
static void i2c_start(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;

  MOD_CHECK_ID(i2c, id);
  platform_i2c_send_start(id);
}

// PicoC: i2c_stop(id);
static void i2c_stop(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;

  MOD_CHECK_ID(i2c, id);
  platform_i2c_send_stop(id);
}

// PicoC: i2c_address(id, address, direction);
static void i2c_address(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;
  int add = param[1]->Val->Integer;
  int dir = param[2]->Val->Integer;

  MOD_CHECK_ID(i2c, id);
  if (add < 0 || add > 127)
    return pmod_error("slave address must be from 0 to 127");
  
  r->Val->UnsignedInteger = platform_i2c_send_address(id, (u16)add, dir);
}

// For PicoC, we split the write function into two
// seperate functions - one to write integers and
// the other to write strings.

// TODO: Write a variadic version for the write*
// functions below.

// PicoC: i2c_write_integer(id, val);
static void i2c_write_integer(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;
  int numdata;

  MOD_CHECK_ID(i2c, id);
  numdata = param[1]->Val->Integer;

  if (numdata < 0 || numdata > 255)
    return pmod_error("Numeric data can be between 0 and 255 only.");

  if (!platform_i2c_send_byte(id, numdata))
    return pmod_error("i2c_send_byte failed.");

  r->Val->UnsignedInteger = 1;
}

// PicoC: i2c_write_string(id, str);
static void i2c_write_string(pstate *p, val *r, val **param, int n)
{
  unsigned int id = param[0]->Val->UnsignedInteger;
  const char *pdata;
  size_t datalen, i;
  u32 wrote = 0;

  MOD_CHECK_ID(i2c, id);
  pdata = param[1]->Val->Identifier;
  datalen = strlen(pdata);
  for (i = 0; i < datalen; i++) {
    if (platform_i2c_send_byte(id, pdata[i]) == 0)
      break;
    wrote += 1;
  }

  r->Val->UnsignedLongInteger = wrote;
}

// PicoC: i2c_read(id, size);
static void i2c_read(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;
  u32 size = param[1]->Val->UnsignedLongInteger, i;
  char *b = HeapAllocMem(size + 1);
  unsigned int count = 0;
  int data;

  MOD_CHECK_ID(i2c, id);
  if (size == 0)
    return;
  for (i = 0; i < size; i++) {
    if ((data = platform_i2c_recv_byte(id, i < size - 1)) == -1) {
      break;
    } else {
      b[count] = (char)data;
      count++;
    }
  }
  r->Val->Identifier = b;
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE i2c_variables[] = {
  {STRKEY("i2c_FAST"), INT(fast)},
  {STRKEY("i2c_SLOW"), INT(slow)},
  {STRKEY("i2c_TRANSMITTER"), INT(trans)},
  {STRKEY("i2c_RECEIVER"), INT(rec)},
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE i2c_library[] = {
  {FUNC(i2c_setup), PROTO("unsigned long i2c_setup(unsigned int, long);")},
  {FUNC(i2c_start), PROTO("void i2c_start(unsigned int);")},
  {FUNC(i2c_stop), PROTO("void i2c_stop(unsigned int);")},
  {FUNC(i2c_address), PROTO("unsigned int i2c_address(unsigned int, int, int);")},
  {FUNC(i2c_write_integer), PROTO("unsigned int i2c_write_integer(unsigned int, int);")},
  {FUNC(i2c_write_string), PROTO("unsigned long i2c_write_string(unsigned int, char *);")},
  {FUNC(i2c_read), PROTO("char *i2c_read(unsigned int, unsigned long);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void i2c_library_init(void)
{
  REGISTER("i2c.h", &i2c_lib_setup_func, &i2c_library[0]);
}

#endif // ALCOR_LANG_PICOC

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// I2C module for Lua.

// Lua: speed = i2c.setup( id, speed )
static int i2c_setup( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );
  s32 speed = ( s32 )luaL_checkinteger( L, 2 );

  MOD_CHECK_ID( i2c, id );
  if (speed <= 0)
    return luaL_error( L, "frequency must be > 0" );
  lua_pushinteger( L, platform_i2c_setup( id, (u32)speed ) );
  return 1;
}

// Lua: i2c.start( id )
static int i2c_start( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );

  MOD_CHECK_ID( i2c, id );
  platform_i2c_send_start( id );
  return 0;
}

// Lua: i2c.stop( id )
static int i2c_stop( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );

  MOD_CHECK_ID( i2c, id );
  platform_i2c_send_stop( id );
  return 0;
}

// Lua: i2c.address( id, address, direction )
static int i2c_address( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );
  int address = luaL_checkinteger( L, 2 );
  int direction = luaL_checkinteger( L, 3 );

  MOD_CHECK_ID( i2c, id );
  if ( address < 0 || address > 127 )
    return luaL_error( L, "slave address must be from 0 to 127" );
  lua_pushboolean( L, platform_i2c_send_address( id, (u16)address, direction ) );
  return 1;
}

// Lua: wrote = i2c.write( id, data1, [data2], ..., [datan] )
// data can be either a string, a table or an 8-bit number
static int i2c_write( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );
  const char *pdata;
  size_t datalen, i;
  int numdata;
  u32 wrote = 0;
  unsigned argn;

  MOD_CHECK_ID( i2c, id );
  if( lua_gettop( L ) < 2 )
    return luaL_error( L, "invalid number of arguments" );
  for( argn = 2; argn <= lua_gettop( L ); argn ++ )
  {
    // lua_isnumber() would silently convert a string of digits to an integer
    // whereas here strings are handled separately.
    if( lua_type( L, argn ) == LUA_TNUMBER )
    {
      numdata = ( int )luaL_checkinteger( L, argn );
      if( numdata < 0 || numdata > 255 )
        return luaL_error( L, "numeric data must be from 0 to 255" );
      if( platform_i2c_send_byte( id, numdata ) != 1 )
        break;
      wrote ++;
    }
    else if( lua_istable( L, argn ) )
    {
      datalen = lua_objlen( L, argn );
      for( i = 0; i < datalen; i ++ )
      {
        lua_rawgeti( L, argn, i + 1 );
        numdata = ( int )luaL_checkinteger( L, -1 );
        lua_pop( L, 1 );
        if( numdata < 0 || numdata > 255 )
          return luaL_error( L, "numeric data must be from 0 to 255" );
        if( platform_i2c_send_byte( id, numdata ) == 0 )
          break;
      }
      wrote += i;
      if( i < datalen )
        break;
    }
    else
    {
      pdata = luaL_checklstring( L, argn, &datalen );
      for( i = 0; i < datalen; i ++ )
        if( platform_i2c_send_byte( id, pdata[ i ] ) == 0 )
          break;
      wrote += i;
      if( i < datalen )
        break;
    }
  }
  lua_pushinteger( L, wrote );
  return 1;
}

// Lua: read = i2c.read( id, size )
static int i2c_read( lua_State *L )
{
  unsigned id = luaL_checkinteger( L, 1 );
  u32 size = ( u32 )luaL_checkinteger( L, 2 ), i;
  luaL_Buffer b;
  int data;

  MOD_CHECK_ID( i2c, id );
  if( size == 0 )
    return 0;
  luaL_buffinit( L, &b );
  for( i = 0; i < size; i ++ )
    if( ( data = platform_i2c_recv_byte( id, i < size - 1 ) ) == -1 )
      break;
    else
      luaL_addchar( &b, ( char )data );
  luaL_pushresult( &b );
  return 1;
}

// Module function map
#define MIN_OPT_LEVEL   2
#include "lrodefs.h"
const LUA_REG_TYPE i2c_map[] = 
{
  { LSTRKEY( "setup" ),  LFUNCVAL( i2c_setup ) },
  { LSTRKEY( "start" ), LFUNCVAL( i2c_start ) },
  { LSTRKEY( "stop" ), LFUNCVAL( i2c_stop ) },
  { LSTRKEY( "address" ), LFUNCVAL( i2c_address ) },
  { LSTRKEY( "write" ), LFUNCVAL( i2c_write ) },
  { LSTRKEY( "read" ), LFUNCVAL( i2c_read ) },
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "FAST" ), LNUMVAL( PLATFORM_I2C_SPEED_FAST ) },
  { LSTRKEY( "SLOW" ), LNUMVAL( PLATFORM_I2C_SPEED_SLOW ) },
  { LSTRKEY( "TRANSMITTER" ), LNUMVAL( PLATFORM_I2C_DIRECTION_TRANSMITTER ) },
  { LSTRKEY( "RECEIVER" ), LNUMVAL( PLATFORM_I2C_DIRECTION_RECEIVER ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_i2c( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_I2C, i2c_map );
  
  // Add the stop bits and parity constants (for uart.setup)
  MOD_REG_NUMBER( L, "FAST", PLATFORM_I2C_SPEED_FAST );
  MOD_REG_NUMBER( L, "SLOW", PLATFORM_I2C_SPEED_SLOW ); 
  MOD_REG_NUMBER( L, "TRANSMITTER", PLATFORM_I2C_DIRECTION_TRANSMITTER );
  MOD_REG_NUMBER( L, "RECEIVER", PLATFORM_I2C_DIRECTION_RECEIVER );
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0
}

#endif // #ifdef ALCOR_LANG_LUA
