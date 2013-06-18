// Module for interfacing with Lua SPI code
// Modified to include support for PicoC.

#ifdef ALCOR_LANG_PICOC
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

#include "platform.h"

#ifdef ALCOR_LANG_PICOC

// ****************************************************************************
// SPI module for PicoC.

// Platform variables
const int master = PLATFORM_SPI_MASTER;
const int slave = PLATFORM_SPI_SLAVE;

// Library setup function
extern void spi_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("spi_MASTER", master);
  picoc_def_integer("spi_SLAVE", slave);
#endif
}

// PicoC: spi_sson(id);
static void spi_sson(pstate *p, val *r, val **param, int n)
{
  unsigned id = param[0]->Val->UnsignedInteger;

  MOD_CHECK_ID(spi, id);
  platform_spi_select(id, PLATFORM_SPI_SELECT_ON);
}

// PicoC: spi_ssoff(id);
static void spi_ssoff(pstate *p, val *r, val **param, int n)
{
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(spi, id);
  platform_spi_select(id, PLATFORM_SPI_SELECT_OFF);
}

// In PicoC:
// clock = setup(id, MASTER/SLAVE, clock, cpol, cpha, databits);
static void spi_setup(pstate *p, val *r, val **param, int n)
{
  unsigned id, cpol, cpha, is_master, databits;
  u32 clock, res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(spi, id);
  is_master = param[1]->Val->UnsignedInteger;
  if (!is_master)
    return pmod_error("invalid type (only spi_MASTER is supported)");
  clock = param[2]->Val->UnsignedLongInteger;
  cpol = param[3]->Val->UnsignedInteger;
  if ((cpol != 0) && (cpol != 1))
    return pmod_error("invalid clock polarity.");
  cpha = param[4]->Val->UnsignedInteger;
  if ((cpha != 0) && (cpha != 1))
    return pmod_error("invalid clock phase.");
  databits = param[5]->Val->UnsignedInteger;
  res = platform_spi_setup(id, is_master, clock, cpol, cpha, databits);
  r->Val->UnsignedLongInteger = res;
}

// TODO: The following two functions should be
// variadic functions.

// PicoC: spi_write_num(id, num);
static void spi_write_num(pstate *p, val *r, val **param, int n)	      
{
  unsigned id = param[0]->Val->UnsignedInteger;
  spi_data_type val = param[1]->Val->UnsignedLongInteger;

  platform_spi_send_recv(id, val);
  r->Val->Integer = 1;
}

// PicoC: len = spi_write_string(id, string, len);
static void spi_write_string(pstate *p, val *r, val **param, int n)
{
  unsigned int id = param[0]->Val->UnsignedInteger, i;
  char *str  = param[1]->Val->Identifier;
  unsigned int len = param[2]->Val->UnsignedInteger;

  for (i = 0; i < len; i++)
    platform_spi_send_recv(id, str[i]);

  r->Val->UnsignedInteger = len;
}

// TODO:
// static void spi_readwrite(pstate *p, val *r, val **param, int n)
// {}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE spi_variables[] = {
  {STRKEY("spi_MASTER"), INT(master)},
  {STRKEY("spi_SLAVE"), INT(slave)},
  {NILKEY, NILVAL}
};
#endif

// Library functions.
const PICOC_REG_TYPE spi_library[] = {
  {FUNC(spi_sson), PROTO("void spi_sson(unsigned int);")},
  {FUNC(spi_ssoff), PROTO("void spi_ssoff(unsigned int);")},
  {FUNC(spi_setup), PROTO("unsigned long spi_setup(unsigned int, unsigned int,\
                           unsigned long, unsigned int, unsigned int,\
                           unsigned int);")},
  {FUNC(spi_write_num), PROTO("int spi_write_num(unsigned int, unsigned long);")},
  {FUNC(spi_write_string), PROTO("unsigned int spi_write_string(unsigned int,\
                                  char *, unsigned int);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void spi_library_init(void)
{
  REGISTER("spi.h", &spi_lib_setup_func, &spi_library[0]);
}

#else

// ****************************************************************************
// SPI module for Lua.

// Lua: sson( id )
static int spi_sson( lua_State* L )
{
  unsigned id;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( spi, id );
  platform_spi_select( id, PLATFORM_SPI_SELECT_ON );
  return 0;
}

// Lua: ssoff( id )
static int spi_ssoff( lua_State* L )
{
  unsigned id;
    
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( spi, id );
  platform_spi_select( id, PLATFORM_SPI_SELECT_OFF );
  return 0;
}

// Lua: clock = setup( id, MASTER/SLAVE, clock, cpol, cpha, databits )
static int spi_setup( lua_State* L )
{
  unsigned id, cpol, cpha, is_master, databits;
  u32 clock, res;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( spi, id );
  is_master = luaL_checkinteger( L, 2 ) == PLATFORM_SPI_MASTER;
  if( !is_master )
    return luaL_error( L, "invalid type (only spi.MASTER is supported)" );
  clock = luaL_checkinteger( L, 3 );
  cpol = luaL_checkinteger( L, 4 );
  if( ( cpol != 0 ) && ( cpol != 1 ) )
    return luaL_error( L, "invalid clock polarity." );
  cpha = luaL_checkinteger( L, 5 );
  if( ( cpha != 0 ) && ( cpha != 1 ) )
    return luaL_error( L, "invalid clock phase." );
  databits = luaL_checkinteger( L, 6 );
  res = platform_spi_setup( id, is_master, clock, cpol, cpha, databits );
  lua_pushinteger( L, res );
  return 1;
}

// Helper function: generic write/readwrite
static int spi_rw_helper( lua_State *L, int withread )
{
  spi_data_type value;
  const char *sval; 
  int total = lua_gettop( L ), i, j, id;
  size_t len, residx = 1;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( spi, id );
  if( withread )
    lua_newtable( L );
  for( i = 2; i <= total; i ++ )
  {
    if( lua_isnumber( L, i ) )
    {
      value = platform_spi_send_recv( id, lua_tointeger( L, i ) );
      if( withread )
      {
        lua_pushnumber( L, value );
        lua_rawseti( L, -2, residx ++ );
      }
    }
    else if( lua_isstring( L, i ) )
    {
      sval = lua_tolstring( L, i, &len );
      for( j = 0; j < len; j ++ )
      {
        value = platform_spi_send_recv( id, sval[ j ] );
        if( withread )
        {
          lua_pushnumber( L, value );
          lua_rawseti( L, -2, residx ++ );
        }
      }
    }
  }
  return withread ? 1 : 0;
}

// Lua: write( id, out1, out2, ... )
static int spi_write( lua_State* L )
{
  return spi_rw_helper( L, 0 );
}

// Lua: restable = readwrite( id, out1, out2, ... )
static int spi_readwrite( lua_State* L )
{
  return spi_rw_helper( L, 1 );
}

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE spi_map[] = 
{
  { LSTRKEY( "setup" ),  LFUNCVAL( spi_setup ) },
  { LSTRKEY( "sson" ),  LFUNCVAL( spi_sson ) },
  { LSTRKEY( "ssoff" ),  LFUNCVAL( spi_ssoff ) },
  { LSTRKEY( "write" ),  LFUNCVAL( spi_write ) },  
  { LSTRKEY( "readwrite" ),  LFUNCVAL( spi_readwrite ) },    
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "MASTER" ), LNUMVAL( PLATFORM_SPI_MASTER ) } ,
  { LSTRKEY( "SLAVE" ), LNUMVAL( PLATFORM_SPI_SLAVE ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_spi( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_SPI, spi_map );
  
  // Add the MASTER and SLAVE constants (for spi.setup)
  MOD_REG_NUMBER( L, "MASTER", PLATFORM_SPI_MASTER );
  MOD_REG_NUMBER( L, "SLAVE", PLATFORM_SPI_SLAVE );  
  
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0  
}

#endif // #ifdef ALCOR_LANG_PICOC
