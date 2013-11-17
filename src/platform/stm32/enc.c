// eLua Module for STM32 timer encoder mode support
// enc is a platform-dependent (STM32) module, that binds to Lua the basic API
// from ST
// Modified to include support for PicoC.

#ifdef ALCOR_LANG_PICOLISP
# include "pico.h"
#endif

#ifdef ALCOR_LANG_PICOC
# include "picoc.h"
# include "interpreter.h"
# include "picoc_mod.h"
# include "rotable.h"
#endif

#ifdef ALCOR_LANG_LUA
# include "lua.h"
# include "lualib.h"
# include "lauxlib.h"
# include "lrotable.h"
# include "auxmods.h"
#endif

#include "platform.h"
#include "platform_conf.h"
#include "elua_int.h"
#include "enc.h"

static elua_int_c_handler prev_handler;
static elua_int_resnum index_resnum;
static int index_tmr_id;
static u16 index_count;
static void index_handler( elua_int_resnum resnum );

#ifdef ALCOR_LANG_PICOC

// ****************************************************************************
// Timer encoder module for PicoC.

// PicoC: stm32_enc_init(id);
static void enc_init(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(timer, id);
  stm32_enc_init(id);
}

// PicoC: stm32_enc_setcounter(id, count);
static void enc_set_counter(pstate *p, val *r, val **param, int n)
{
  unsigned id, count;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(timer, id);
  count = param[1]->Val->UnsignedInteger;

  stm32_enc_set_counter(id, count);
}

// PicoC: stm32_enc_setidxtrig(id, resnum, tmr_id, count);
static void enc_set_index_handler(pstate *p, val *r, val **param, int n)
{
  elua_int_id id;
 
  id = param[0]->Val->Character;
  if (id < ELUA_INT_FIRST_ID || id > INT_ELUA_LAST)
    return pmod_error("invalid interpreter ID");

  index_resnum = param[1]->Val->Character;
  index_tmr_id = param[2]->Val->Integer;
  MOD_CHECK_ID(timer, index_tmr_id);
  index_count = param[3]->Val->UnsignedShortInteger;

  platform_cpu_set_interrupt(id, index_resnum, PLATFORM_CPU_ENABLE);
  prev_handler = elua_int_set_c_handler(id, index_handler);
}

static void index_handler(elua_int_resnum resnum)
{
  if (prev_handler)
    prev_handler;

  if (resnum != index_resnum)
    return;

  stm32_enc_set_counter(index_tmr_id, index_count);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"  

const PICOC_REG_TYPE enc_library[] = {
  {FUNC(enc_init), PROTO("void stm32_enc_init(unsigned int);")},
  {FUNC(enc_set_counter), PROTO("void stm32_enc_setcounter(unsigned int, unsigned int);")},
  {FUNC(enc_set_index_handler), PROTO("void stm32_enc_setidxtrig(char, char, int, unsigned short);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void enc_library_init(void)
{
  REGISTER("enc.h", NULL, &enc_library);
}

#endif

#ifdef ALCOR_LANG_LUA

// ****************************************************************************
// Timer encoder module for Lua.

//Lua: init(id)
static int enc_init( lua_State *L )
{
  unsigned id;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( timer, id );
  stm32_enc_init( id );
  return 0;
}

//Lua: setcounter(id, count)
static int enc_set_counter( lua_State *L )
{
  unsigned id, count;

  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( timer, id );
  count = luaL_checkinteger( L, 2 );

  stm32_enc_set_counter( id, count );
  return 0;
}

//Lua: setidxtrig( id, resnum, tmr_id, count )
static int enc_set_index_handler( lua_State *L )
{
  elua_int_id id;
  
  id = ( elua_int_id )luaL_checkinteger( L, 1 );
  if( id < ELUA_INT_FIRST_ID || id > INT_ELUA_LAST )
    return luaL_error( L, "invalid interrupt ID" );
  index_resnum = ( elua_int_resnum )luaL_checkinteger( L, 2 );
  index_tmr_id = luaL_checkinteger( L, 3 );
  MOD_CHECK_ID( timer, index_tmr_id );
  index_count = ( u16 )luaL_checkinteger( L, 4 );

  platform_cpu_set_interrupt( id, index_resnum, PLATFORM_CPU_ENABLE );
  prev_handler = elua_int_set_c_handler( id, index_handler );
}

static void index_handler( elua_int_resnum resnum )
{
  if( prev_handler )
    prev_handler;

  if( resnum != index_resnum )
    return;

  stm32_enc_set_counter( index_tmr_id, index_count );
}


#define MIN_OPT_LEVEL 2
#include "lrodefs.h"  

// Module function map
const LUA_REG_TYPE enc_map[] =
{ 
  { LSTRKEY( "init" ),  LFUNCVAL( enc_init ) },
  { LSTRKEY( "setcounter" ),  LFUNCVAL( enc_set_counter ) },
  { LSTRKEY( "setidxtrig" ),  LFUNCVAL( enc_set_index_handler ) },
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_enc( lua_State *L )
{
  LREGISTER( L, AUXLIB_ENC, enc_map );
}  

#endif // #ifdef ALCOR_LANG_LUA
