// Module for interfacing Lua code with a Controller Area Network (CAN)
// Modified to include support for Alcor6L.

#include "platform.h"

#if defined ALCOR_LANG_PICOLISP

#include "pico.h"

// ****************************************************************************
// CAN for picoLisp.

// (can-setup id clock) -> clock
any can_setup(any ex) {
  unsigned id;
  u32 clock;
  any x, y;

  // get id.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, can, id);
  
  // get clock value.
  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  clock = unBox(y);

  return box(platform_can_setup(id, clock));
}

// (can-send id canid canidtype 'message) -> message
any can_send(any ex) {
  size_t len;
  int id, canid, idtype;
  any x, y;

  static const char const *args[] = {
    "id-ext",
    "id-std",
    NULL
  };

  // get id.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, can, id);

  // get can id.
  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  canid = unBox(y);

  // get id type (picoLisp symbol).
  x = cdr(x), y = EVAL(car(x));
  if (equal(mkStr(args[0]), y))
    idtype = ELUA_CAN_ID_EXT;
  else if (equal(mkStr(args[1]), y))
    idtype = ELUA_CAN_ID_STD;
  else
    err(NULL, x, "invalid CAN idtype");

  // get can data.
  x = cdr(x);
  len = bufSize(y = EVAL(car(x)));
  char data[len];
  NeedSym(ex, y);
  bufString(y, data);
  
  if (len > PLATFORM_CAN_MAXLEN)
    err(NULL, x, "message exceeds max length");

  platform_can_send(id, canid, idtype, len, (const u8 *)data);
  return mkStr(data);
}

// (can-recv id) -> str
any can_recv(any ex) {
  u8 len, idtype, data[8];
  int id;
  u32 canid;
  any x, y;
  cell c1;

  // get id.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y);
  MOD_CHECK_ID(ex, can, id);

  if (platform_can_recv(id,
			&canid,
			&idtype,
			&len,
			data) == PLATFORM_OK) {
    Push(c1, y = cons(mkStr((char *)data), Nil));
    Push(c1, y = cons(box(idtype), y));
    Push(c1, y = cons(box(canid), y));
    return Pop(c1);
  } else {
    return Nil;
  }
}

#elif defined ALCOR_LANG_PICOC

// ****************************************************************************
// CAN for PicoC.

#include "interpreter.h"
#include "picoc_mod.h"
#include "rotable.h"

// Platform variables
static const int id_std = ELUA_CAN_ID_STD;
static const int id_ext = ELUA_CAN_ID_EXT;

// Library setup function */
extern void can_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("can_ID_STD", id_std);
  picoc_def_integer("can_ID_EXT", id_ext);
#endif
}

// picoc: can_setup(id, clock);
static void can_setup(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  u32 clock, res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(can, id);
  clock = param[1]->Val->UnsignedLongInteger;
  res = platform_can_setup(id, clock);

  r->Val->UnsignedLongInteger = res;
}

// picoc: can_send(id, canid, canidtype, message);
static void can_send(pstate *p, val *r, val **param, int n)
{
  size_t len;
  int id, canid, idtype;
  const char *data;

  id = param[0]->Val->Integer;
  MOD_CHECK_ID(can, id);
  canid = param[1]->Val->Integer;
  idtype = param[2]->Val->Integer;
  data = param[3]->Val->Identifier;
  len = strlen(data);

  if (len > PLATFORM_CAN_MAXLEN)
    return pmod_error("message exceeds max length");

  platform_can_send(id, canid, idtype, len, (const u8 *)data);
  r->Val->UnsignedInteger = len;
}

// picoc: can_recv(id, &message);
static void can_recv(pstate *p, val *r, val **param, int n)
{
  u8 len, idtype, data[8];
  int id;
  u32 canid;

  id = param[0]->Val->Integer;
  MOD_CHECK_ID(can, id);

  if (platform_can_recv(id, &canid, &idtype, &len, data) == PLATFORM_OK) {
    param[1]->Val->Identifier = (char *)data;
    r->Val->Integer = len;
  } else {
    r->Val->Integer = -1;
  }
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE can_variables[] = {
  {STRKEY("can_ID_STD"), INT(id_std)},
  {STRKEY("can_ID_EXT"), INT(id_ext)},
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE can_library[] = {
  {FUNC(can_setup), PROTO("unsigned long can_setup(unsigned int, unsigned long);")},
  {FUNC(can_send), PROTO("unsigned int can_send(int, int, int, char *);")},
  {FUNC(can_recv), PROTO("int can_recv(int, char *);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void can_library_init(void)
{
  REGISTER("can.h", &can_lib_setup_func, &can_library[0]);
}

#else

// ****************************************************************************
// CAN for PicoC.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"

// Lua: setup( id, clock )
static int can_setup( lua_State* L )
{
  unsigned id;
  u32 clock, res;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( can, id );
  clock = luaL_checkinteger( L, 2 );
  res = platform_can_setup( id, clock );
  lua_pushinteger( L, res );
  return 1;
}

// Lua: send( id, canid, canidtype, message )
static int can_send( lua_State* L )
{
  size_t len;
  int id, canid, idtype;
  const char *data;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( can, id );
  canid = luaL_checkinteger( L, 2 );
  idtype = luaL_checkinteger( L, 3 );
  data = luaL_checklstring (L, 4, &len);
  if ( len > PLATFORM_CAN_MAXLEN )
    return luaL_error( L, "message exceeds max length" );
  
  platform_can_send( id, canid, idtype, len, ( const u8 * )data );
  
  return 0;
}

// Lua: canid, canidtype, message = recv( id )
static int can_recv( lua_State* L )
{
  u8 len;
  int id;
  u32 canid;
  u8  idtype, data[ 8 ];
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( can, id );
  
  if( platform_can_recv( id, &canid, &idtype, &len, data ) == PLATFORM_OK )
  {
    lua_pushinteger( L, canid );
    lua_pushinteger( L, idtype );
    lua_pushlstring( L, ( const char * )data, ( size_t )len );
  
    return 3;
  }
  else
    return 0;
}


// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE can_map[] = 
{
  { LSTRKEY( "setup" ),  LFUNCVAL( can_setup ) },
  { LSTRKEY( "send" ),  LFUNCVAL( can_send ) },  
  { LSTRKEY( "recv" ),  LFUNCVAL( can_recv ) },
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "ID_STD" ), LNUMVAL( ELUA_CAN_ID_STD ) },
  { LSTRKEY( "ID_EXT" ), LNUMVAL( ELUA_CAN_ID_EXT ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_can( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_CAN, can_map );
  
  // Module constants  
  MOD_REG_NUMBER( L, "ID_STD", ELUA_CAN_ID_STD );
  MOD_REG_NUMBER( L, "ID_EXT", ELUA_CAN_ID_EXT );
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0  
}

#endif // #ifdef ALCOR_LANG_PICOLISP
