// Module for interfacing with CPU
// Modified to include support for Alcor6L.

#if defined ALCOR_LANG_MYBASIC
# include "my_basic.h"
#elif defined ALCOR_LANG_PICOLISP
# include "pico.h"
#elif defined ALCOR_LANG_PICOC
# include "picoc.h"
# include "interpreter.h"
# include "picoc_mod.h"
# include "rotable.h"
#else
# include "lua.h"
# include "lualib.h"
# include "lauxlib.h"
# include "auxmods.h"
# include "lrotable.h"
#endif

#include <string.h> 
#include "platform.h"

#undef _C
#define _C(x) {#x, x}
#include "platform_conf.h"

// CPU constants list
typedef struct
{
  const char* name;
  u32 val;
} cpu_const_t;

#ifdef PLATFORM_CPU_CONSTANTS
static const cpu_const_t cpu_constants[] = 
{
  PLATFORM_CPU_CONSTANTS,
  { NULL, 0 }
};
#endif

#if defined ALCOR_LANG_MYBASIC

// ****************************************************************************
// CPU module for my-basic.

#elif defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// CPU module for picoLisp.

// helpers.
#define get_addr_data(a, d)			\
  x = cdr(ex);					\
  NeedNum(ex, y = EVAL(car(x)));		\
  a = unBox(y);					\
  x = cdr(x);					\
  NeedNum(ex, y = EVAL(car(x)));		\
  d = unBox(y)

#define get_addr(a)				\
  x = cdr(ex);					\
  NeedNum(ex, y = EVAL(car(x)));		\
  a = unBox(y)

// (cpu-w32 'addr 'data) -> num
any cpu_w32(any ex) {
  u32 addr, data;
  any x, y;

  get_addr_data(addr, data);
  *(U32 *)addr = data;
  return box(data);
}

// (cpu-r32 'addr) -> data
any cpu_r32(any ex) {
  u32 addr;
  any x, y;

  get_addr(addr);
  return box(*(u32 *)addr);
}

// (cpu-w16 'addr 'data) -> num
any cpu_w16(any ex) {
  u32 addr;
  u16 data;
  any x, y;
  
  get_addr_data(addr, data);
  *(u16 *)addr = data;
  return box(data);
}

// (cpu-r16 'addr) -> data
any cpu_r16(any ex) {
  u32 addr;
  any x, y;

  get_addr(addr);
  return box(*(u16 *)addr);
}

// (cpu-w8 'addr 'data) -> num
any cpu_w8(any ex) {
  u32 addr;
  u8 data;
  any x, y;

  get_addr_data(addr, data);
  *(u8 *)addr = data;
  return box(data);
}

// (cpu-r8 'addr) -> data 
any cpu_r8(any ex) {
  u32 addr;
  any x, y;
  
  get_addr(addr);
  return box(*(u8 *)addr);
}

// (cpu-clock) -> num
any cpu_clock(any ex) {
  return box(platform_cpu_get_frequency());
}

#elif defined ALCOR_LANG_PICOC

// ****************************************************************************
// CPU module for PicoC.

// PicoC: cpu_w32(address, data);
static void cpu_w32(pstate *p, val *r, val **param, int n)
{
  u32 addr, data;
 
  addr = param[0]->Val->UnsignedLongInteger;
  data = param[1]->Val->UnsignedLongInteger;
  *(u32 *)addr = data;
}

// PicoC: data = cpu_r32(address);
static void cpu_r32(pstate *p, val *r, val **param, int n)
{
  u32 addr;
  
  addr = param[0]->Val->UnsignedLongInteger;
  r->Val->UnsignedLongInteger = *(u32 *)addr;
}

// PicoC: cpu_w16(address, data);
static void cpu_w16(pstate *p, val *r, val **param, int n)
{
  u32 addr = param[0]->Val->UnsignedLongInteger;
  u16 data = param[1]->Val->UnsignedShortInteger;
  *(u16 *)addr = data;
}

// PicoC: data = cpu_r16(address);
static void cpu_r16(pstate *p, val *r, val **param, int n)
{
  u32 addr;

  addr = param[0]->Val->UnsignedLongInteger;
  r->Val->UnsignedShortInteger = *(u16 *)addr;
}

// PicoC: cpu_w8(address, data);
static void cpu_w8(pstate *p, val *r, val **param, int n)
{
  u32 addr;
  u8 data = param[1]->Val->Character;
  
  addr = param[0]->Val->UnsignedLongInteger;
  *(u8 *)addr = data;
}

// PicoC: data = cpu_r8(address);
static void cpu_r8(pstate *p, val *r, val **param, int n)
{
  u32 addr;
  
  addr = param[0]->Val->UnsignedLongInteger;
  r->Val->Character = *(u8 *)addr;
}

// PicoC: cpu_clock();
static void cpu_clock(pstate *p, val *r, val **param, int n)
{
  r->Val->UnsignedLongInteger =
    platform_cpu_get_frequency();
}

#ifdef PLATFORM_CPU_CONSTANTS

// Returns the interrupt assignment values.
// For use in future when interrupts are
// supported.
static int cpu_const_h(char *key)
{
  unsigned i = 0;
  while (cpu_constants[i].name != NULL) {
    if (!strcmp(cpu_constants[i].name, key))
      return cpu_constants[i].val;
    i++;
  }
  return -1;
}

// PicoC: val = cpu_const_getval(str);
static void cpu_const_getval(pstate *p, val *r, val **param, int n)
{
  r->Val->Integer =
    cpu_const_h(param[0]->Val->Identifier);
}

#endif // #ifdef PLATFORM_CPU_CONSTANTS

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

// List of all library functions and their prototypes
const PICOC_REG_TYPE cpu_library[] = {
  {FUNC(cpu_w32), PROTO("void cpu_w32(unsigned long, unsigned long);")},
  {FUNC(cpu_r32), PROTO("unsigned long cpu_r32(unsigned long);")},
  {FUNC(cpu_w16), PROTO("void cpu_w16(unsigned long, unsigned short);")},
  {FUNC(cpu_r16), PROTO("unsigned short cpu_r16(unsigned long);")},
  {FUNC(cpu_w8), PROTO("void cpu_w8(unsigned long, char);")},
  {FUNC(cpu_r8), PROTO("char cpu_r8(unsigned long);")},
  {FUNC(cpu_clock), PROTO("unsigned long cpu_clock(void);")},
#ifdef PLATFORM_CPU_CONSTANTS
  {FUNC(cpu_const_getval), PROTO("int cpu_const_getval(char *);")},
#endif
  {NILFUNC, NILPROTO}
};

// Init library. 
extern void cpu_library_init(void)
{
  REGISTER("cpu.h", NULL, &cpu_library[0]);
}

#else

// **************************************************************************** 
// CPU module for Lua.

// Lua: w32( address, data )
static int cpu_w32( lua_State *L )
{
  u32 addr, data;
  
  luaL_checkinteger( L, 1 );
  luaL_checkinteger( L, 2 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  data = ( u32 )luaL_checknumber( L, 2 );
  *( u32* )addr = data;
  return 0;
}

// Lua: data = r32( address )
static int cpu_r32( lua_State *L )
{
  u32 addr;

  luaL_checkinteger( L, 1 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  lua_pushnumber( L, ( lua_Number )( *( u32* )addr ) );  
  return 1;
}

// Lua: w16( address, data )
static int cpu_w16( lua_State *L )
{
  u32 addr;
  u16 data = ( u16 )luaL_checkinteger( L, 2 );
  
  luaL_checkinteger( L, 1 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  *( u16* )addr = data;
  return 0;
}

// Lua: data = r16( address )
static int cpu_r16( lua_State *L )
{
  u32 addr;

  luaL_checkinteger( L, 1 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  lua_pushnumber( L, ( lua_Number )( *( u16* )addr ) );  
  return 1;
}

// Lua: w8( address, data )
static int cpu_w8( lua_State *L )
{
  u32 addr;
  u8 data = ( u8 )luaL_checkinteger( L, 2 );
  
  luaL_checkinteger( L, 1 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  *( u8* )addr = data;
  return 0;
}

// Lua: data = r8( address )
static int cpu_r8( lua_State *L )
{
  u32 addr;

  luaL_checkinteger( L, 1 );
  addr = ( u32 )luaL_checknumber( L, 1 );
  lua_pushnumber( L, ( lua_Number )( *( u8* )addr ) );  
  return 1;
}

// Either disables or enables the given interrupt(s)
static int cpuh_int_helper( lua_State *L, int mode )
{
#ifdef BUILD_LUA_INT_HANDLERS
  unsigned i;
  elua_int_id id;
  elua_int_resnum resnum;
  int res;

  if( lua_gettop( L ) > 0 )
  {
    id = ( elua_int_id )luaL_checkinteger( L, 1 );
    if( id < ELUA_INT_FIRST_ID || id > INT_ELUA_LAST )
      return luaL_error( L, "invalid interrupt ID" );
    for( i = 2; i <= lua_gettop( L ); i ++ )
    {
      resnum = ( elua_int_resnum )luaL_checkinteger( L, i );
      res = platform_cpu_set_interrupt( id, resnum, mode );
      if( res == PLATFORM_INT_INVALID )
        return luaL_error( L, "%d is not a valid interrupt ID", ( int )id );
      else if( res == PLATFORM_INT_NOT_HANDLED )
        return luaL_error( L, "'%s' not implemented for interrupt %d with resource %d", mode == PLATFORM_CPU_ENABLE ? "sei" : "cli", ( int )id, ( int )resnum );
      else if( res == PLATFORM_INT_BAD_RESNUM )
        return luaL_error( L, "resource %d not valid for interrupt %d", ( int )resnum, ( int )id );
    }
  }
  else
#else // #ifdef BUILD_LUA_INT_HANDLERS
  if( lua_gettop( L ) > 0 )
    return luaL_error( L, "Lua interrupt support not available." );
#endif // #ifdef BUILD_LUA_INT_HANDLERS
  platform_cpu_set_global_interrupts( mode );
  return 0;

}

// Lua: cpu.cli( id, resnum1, [resnum2], ... [resnumn] )
static int cpu_cli( lua_State *L )
{
  return cpuh_int_helper( L, PLATFORM_CPU_DISABLE );
}

// Lua: cpu.sei( id, resnum1, [resnum2], ... [resnumn] )
static int cpu_sei( lua_State *L )
{
  return cpuh_int_helper( L, PLATFORM_CPU_ENABLE );
}

// Lua: frequency = clock()
static int cpu_clock( lua_State *L )
{
  lua_pushinteger( L, platform_cpu_get_frequency() );
  return 1;
}

#ifdef PLATFORM_CPU_CONSTANTS
static int cpu_mt_index( lua_State *L )
{
  const char *key = luaL_checkstring( L, 2 );
  unsigned i = 0;
  
  while( cpu_constants[ i ].name != NULL )
  {
    if( !strcmp( cpu_constants[ i ].name, key ) )
    {
      lua_pushnumber( L, cpu_constants[ i ].val );
      return 1;
    }
    i ++;
  }
  return 0;
}
#endif

#ifdef BUILD_LUA_INT_HANDLERS

// Lua: prevhandler = cpu.set_int_handler( id, f )
static int cpu_set_int_handler( lua_State *L )
{
  int id = ( int )luaL_checkinteger( L, 1 );

  if( id < ELUA_INT_FIRST_ID || id > INT_ELUA_LAST )
    return luaL_error( L, "invalid interrupt ID" );
  if( lua_type( L, 2 ) == LUA_TFUNCTION || lua_type( L, 2 ) == LUA_TLIGHTFUNCTION || lua_type( L, 2 ) == LUA_TNIL )
  {
    if( lua_type( L, 2 ) == LUA_TNIL )
      elua_int_disable( id );
    else
      elua_int_enable( id );
    lua_settop( L, 2 ); // id f
    lua_rawgeti( L, LUA_REGISTRYINDEX, LUA_INT_HANDLER_KEY ); // id f inttable
    lua_rawgeti( L, -1, id ); // id f inttable prevf
    lua_replace( L, 1 ); // prevf f inttable
    lua_pushvalue( L, 2 ); // prevf f inttable f
    lua_rawseti( L, -2, id ); // prevf f inttable
    lua_pop( L, 2 ); // prevf
    return 1;
  }
  else
    return luaL_error( L, "invalid handler type (must be a function or nil)" );
  return 0;
}

// Lua: handler = cpu.get_int_handler( id )
static int cpu_get_int_handler( lua_State *L )
{
  int id = ( int )luaL_checkinteger( L, 1 );

  if( id < ELUA_INT_FIRST_ID || id > INT_ELUA_LAST )
    return luaL_error( L, "invalid interrupt ID" );
  lua_rawgeti( L, LUA_REGISTRYINDEX, LUA_INT_HANDLER_KEY );
  lua_rawgeti( L, -1, id );
  return 1;
}

// Lua: flag = get_int_flag( id, resnum, [clear] )
// 'clear' default to true if not specified
static int cpu_get_int_flag( lua_State *L )
{
  elua_int_id id;
  elua_int_resnum resnum;  
  int clear = 1;
  int res;

  id = ( elua_int_id )luaL_checkinteger( L, 1 );
  resnum = ( elua_int_resnum )luaL_checkinteger( L, 2 );
  if( lua_gettop( L ) >= 3 )
  {
    if( lua_isboolean( L, 3 ) )
      clear = lua_toboolean( L, 3 );
    else
      return luaL_error( L, "expected a bool as the 3rd argument of this function" );
  }
  res = platform_cpu_get_interrupt_flag( id, resnum, clear );
  if( res == PLATFORM_INT_INVALID )
    return luaL_error( L, "%d is not a valid interrupt ID", ( int )id );
  else if( res == PLATFORM_INT_NOT_HANDLED )
    return luaL_error( L, "get flag operation not implemented for interrupt %d with resource %d", ( int )id, ( int )resnum );
  else if( res == PLATFORM_INT_BAD_RESNUM )
    return luaL_error( L, "resource %d not valid for interrupt %d", ( int )resnum, ( int )id );
  lua_pushinteger( L, res );
  return 1;
}
#endif // #ifdef BUILD_LUA_INT_HANDLERS

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE cpu_map[] = 
{
  { LSTRKEY( "w32" ), LFUNCVAL( cpu_w32 ) },
  { LSTRKEY( "r32" ), LFUNCVAL( cpu_r32 ) },
  { LSTRKEY( "w16" ), LFUNCVAL( cpu_w16 ) },
  { LSTRKEY( "r16" ), LFUNCVAL( cpu_r16 ) },
  { LSTRKEY( "w8" ), LFUNCVAL( cpu_w8 ) },
  { LSTRKEY( "r8" ), LFUNCVAL( cpu_r8 ) },
  { LSTRKEY( "cli" ), LFUNCVAL( cpu_cli ) },
  { LSTRKEY( "sei" ), LFUNCVAL( cpu_sei ) },
  { LSTRKEY( "clock" ), LFUNCVAL( cpu_clock ) },
#ifdef BUILD_LUA_INT_HANDLERS
  { LSTRKEY( "set_int_handler" ), LFUNCVAL( cpu_set_int_handler ) },
  { LSTRKEY( "get_int_handler" ), LFUNCVAL( cpu_get_int_handler ) },
  { LSTRKEY( "get_int_flag" ), LFUNCVAL( cpu_get_int_flag) },
#endif
#if defined( PLATFORM_CPU_CONSTANTS ) && LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "__metatable" ), LROVAL( cpu_map ) },
#endif
#ifdef PLATFORM_CPU_CONSTANTS
  { LSTRKEY( "__index" ), LFUNCVAL( cpu_mt_index ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_cpu( lua_State *L )
{
#ifdef BUILD_LUA_INT_HANDLERS
  // Create interrupt table
  lua_newtable( L );
  lua_rawseti( L, LUA_REGISTRYINDEX, LUA_INT_HANDLER_KEY );
#endif //#ifdef BUILD_LUA_INT_HANDLERS

#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  // Register methods
  luaL_register( L, AUXLIB_CPU, cpu_map );
  
#ifdef PLATFORM_CPU_CONSTANTS
  // Set table as its own metatable
  lua_pushvalue( L, -1 );
  lua_setmetatable( L, -2 );
#endif // #ifdef PLATFORM_CPU_CONSTANTS
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0
}

#endif // #ifdef ALCOR_LANG_PICOLISP
