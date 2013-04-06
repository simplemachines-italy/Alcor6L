// Module for interfacing with terminal functions

#include "platform.h"
#include "term.h"
#include <string.h>

#if defined (ALCOR_LANG_PICOC) && defined (BUILD_TERM)

// ****************************************************************************
// Terminal module for PicoC.

#include "interpreter.h"
#include "rotable.h"
#include "picoc_mod.h"
#include "platform_conf.h"

// Platform variables
const int NoWait = TERM_INPUT_DONT_WAIT;
const int Wait = TERM_INPUT_WAIT;

// Library setup function
extern void term_lib_setup_func(void)
{
#if ((PICOC_OPTIMIZE_MEMORY == 0) && defined (BUILTIN_MINI_STDLIB))
  VariableDefinePlatformVar(NULL, "WAIT", &IntType, (union AnyValue *)&Wait, FALSE);
  VariableDefinePlatformVar(NULL, "NOWAIT", &IntType, (union AnyValue *)&NoWait, FALSE);
#endif
}

// picoc: term_clrscr();
static void pterm_clrscr(pstate *p, val *r, val **param, int n)
{
  term_clrscr();
  r->Val->Integer = 0;
}

// picoc: term_clreol();
static void pterm_clreol(pstate *p, val *r, val **param, int n)
{
  term_clreol();
  r->Val->Integer = 0;
}

// picoc: term_moveto(x y);
static void pterm_moveto(pstate *p, val *r, val **param, int n)
{
  unsigned x, y;
  x = param[0]->Val->UnsignedInteger;
  y = param[1]->Val->UnsignedInteger;

  term_gotoxy(x, y);
  r->Val->Integer = 0;
}

// picoc: term_moveup(lines);
static void pterm_moveup(pstate *p, val *r, val **param, int n)
{
  unsigned delta;

  delta = param[0]->Val->UnsignedInteger;
  term_up(delta);
  r->Val->Integer = 0;
}

// picoc: movedown(lines);
static void pterm_movedown(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_down(delta);
  r->Val->Integer = 0;
}

// picoc: term_moveleft(cols);
static void pterm_moveleft(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_left(delta);
  r->Val->Integer = 0;
}

// picoc: term_moveright(cols);
static void pterm_moveright(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_right(delta);
  r->Val->Integer = 0;
}

// picoc: lines = term_getlines();
static void pterm_getlines(pstate *p, val *r, val **param, int n)
{
  r->Val->UnsignedInteger = term_get_lines();
}

// picoc: columns = term_getcols();
static void pterm_getcols(pstate *p, val *r, val **param, int n)
{
  r->Val->UnsignedInteger = term_get_cols();
}

// picoc: term_puts(x, y, string);
static void pterm_puts(pstate *p, val *r, val **param, int n)
{
  const char *buf = param[2]->Val->Identifier;
  size_t len = strlen(buf), i;

  term_gotoxy(param[0]->Val->UnsignedInteger,
              param[1]->Val->UnsignedInteger);

  for (i = 0; i < len; i++)
    term_putch(buf[i]);

  r->Val->UnsignedInteger = len;
}

// picoc: term_putch(ch);
static void pterm_putch(pstate *p, val *r, val **param, int n)
{
  term_putch(param[0]->Val->Character);
  r->Val->UnsignedInteger = 1;
}

// picoc: cursorx = term_getcx();
static void pterm_getcx(pstate *p, val *r, val **param, int n)
{
  r->Val->UnsignedInteger = term_get_cx();
}

// picoc: cursory = term_getcy();
static void pterm_getcy(pstate *p, val *r, val **param, int n)
{
  r->Val->UnsignedInteger = term_get_cy();
}

// picoc: key = term_getchar([mode]);
static void pterm_getchar(pstate *p, val *r, val **param, int n)
{
  int temp = TERM_INPUT_WAIT, res;
  temp = param[0]->Val->Integer;
  res = term_getch(temp);
  if (!res) {
    r->Val->Integer = -1;
    return;
  }
  r->Val->Integer = res;
}

#undef _D
#define _D(x) #x
static const char* term_key_names[] = {TERM_KEYCODES};

// Look for all KC_xxxx codes
// picoc: term_decode(str);
static void pterm_decode(pstate *p, val *r, val **param, int n)
{
  const char *key = param[0]->Val->Identifier;
  unsigned i, total = sizeof(term_key_names) / sizeof(char*);

  if (!key || *key != 'K') {
    r->Val->Integer = -1;
    return pmod_error("Invalid key.");
  }
  for (i = 0; i < total; i++)
    if (!strcmp(key, term_key_names[i]))
      break;
  if (i == total)
    r->Val->Integer = 0;
  else
    r->Val->Integer = i + TERM_FIRST_KEY;
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if ((PICOC_OPTIMIZE_MEMORY == 2) && !defined (BUILTIN_MINI_STDLIB))
/* rotable for platform variables */
const PICOC_RO_TYPE term_variables[] = {
  {STRKEY("TERM_WAIT"), INT(Wait)},
  {STRKEY("TERM_NOWAIT"), INT(NoWait)},
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE term_library[] = {
  {FUNC(pterm_clrscr), PROTO("int term_clrscr(void);")},
  {FUNC(pterm_clreol), PROTO("int term_clreol(void);")},
  {FUNC(pterm_moveto), PROTO("int term_moveto(unsigned int, unsigned int);")},
  {FUNC(pterm_moveup), PROTO("int term_moveup(unsigned int);")},
  {FUNC(pterm_movedown), PROTO("int term_movedown(unsigned int);")},
  {FUNC(pterm_moveleft), PROTO("int term_moveleft(unsigned int);")},
  {FUNC(pterm_moveright), PROTO("int term_moveright(unsigned int);")},
  {FUNC(pterm_getlines), PROTO("unsigned int term_getlines(void);")},
  {FUNC(pterm_getcols), PROTO("unsigned int term_getcols(void);")},
  {FUNC(pterm_puts), PROTO("unsigned int term_puts(unsigned int, unsigned int, char *);")},
  {FUNC(pterm_putch), PROTO("unsigned int term_putch(char);")},
  {FUNC(pterm_getcx), PROTO("unsigned int term_getcx(void);")},
  {FUNC(pterm_getcy), PROTO("unsigned int term_getcy(void);")},
  {FUNC(pterm_getchar), PROTO("int term_getchar(int);")},
  {FUNC(pterm_decode), PROTO("int term_decode(char *);")},
  {NILFUNC, NILPROTO}
};

// init library.
extern void term_library_init(void)
{
  REGISTER("term.h", &term_lib_setup_func, &term_library[0]);
}

#else

// ****************************************************************************
// Terminal module for Lua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"
#include "platform_conf.h"

// Lua: clrscr()
static int luaterm_clrscr( lua_State* L )
{
  term_clrscr();
  return 0;
}

// Lua: clreol()
static int luaterm_clreol( lua_State* L )
{
  term_clreol();
  return 0;
}

// Lua: moveto( x, y )
static int luaterm_moveto( lua_State* L )
{
  unsigned x, y;
  
  x = ( unsigned )luaL_checkinteger( L, 1 );
  y = ( unsigned )luaL_checkinteger( L, 2 );
  term_gotoxy( x, y );
  return 0;
}

// Lua: moveup( lines )
static int luaterm_moveup( lua_State* L )
{
  unsigned delta;
  
  delta = ( unsigned )luaL_checkinteger( L, 1 );
  term_up( delta );
  return 0;
}

// Lua: movedown( lines )
static int luaterm_movedown( lua_State* L )
{
  unsigned delta;
  
  delta = ( unsigned )luaL_checkinteger( L, 1 );
  term_down( delta );
  return 0;
}

// Lua: moveleft( cols )
static int luaterm_moveleft( lua_State* L )
{
  unsigned delta;
  
  delta = ( unsigned )luaL_checkinteger( L, 1 );
  term_left( delta );
  return 0;
}

// Lua: moveright( cols )
static int luaterm_moveright( lua_State* L )
{
  unsigned delta;
  
  delta = ( unsigned )luaL_checkinteger( L, 1 );
  term_right( delta );
  return 0;
}

// Lua: lines = getlines()
static int luaterm_getlines( lua_State* L )
{
  lua_pushinteger( L, term_get_lines() );
  return 1;
}

// Lua: columns = getcols()
static int luaterm_getcols( lua_State* L )
{
  lua_pushinteger( L, term_get_cols() );
  return 1;
}

// Lua: print( string1, string2, ... )
// or print( x, y, string1, string2, ... )
static int luaterm_print( lua_State* L )
{
  const char* buf;
  size_t len, i;
  int total = lua_gettop( L ), s = 1;
  int x = -1, y = -1;

  // Check if the function has integer arguments
  if( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) )
  {
    x = lua_tointeger( L, 1 );
    y = lua_tointeger( L, 2 );
    term_gotoxy( x, y );
    s = 3;
  } 
  for( ; s <= total; s ++ )
  {
    luaL_checktype( L, s, LUA_TSTRING );
    buf = lua_tolstring( L, s, &len );
    for( i = 0; i < len; i ++ )
      term_putch( buf[ i ] );
  }
  return 0;
}

// Lua: cursorx = getcx()
static int luaterm_getcx( lua_State* L )
{
  lua_pushinteger( L, term_get_cx() );
  return 1;
}

// Lua: cursory = getcy()
static int luaterm_getcy( lua_State* L )
{
  lua_pushinteger( L, term_get_cy() );
  return 1;
}

// Lua: key = getchar( [ mode ] )
static int luaterm_getchar( lua_State* L )
{
  int temp = TERM_INPUT_WAIT;
  
  if( lua_isnumber( L, 1 ) )
    temp = lua_tointeger( L, 1 );
  lua_pushinteger( L, term_getch( temp ) );
  return 1;
}

// Key codes by name
#undef _D
#define _D( x ) #x
static const char* term_key_names[] = { TERM_KEYCODES };

// __index metafunction for term
// Look for all KC_xxxx codes
static int term_mt_index( lua_State* L )
{
  const char *key = luaL_checkstring( L ,2 );
  unsigned i, total = sizeof( term_key_names ) / sizeof( char* );
  
  if( !key || *key != 'K' )
    return 0;
  for( i = 0; i < total; i ++ )
    if( !strcmp( key, term_key_names[ i ] ) )
      break;
  if( i == total )
    return 0;
  else
  {
    lua_pushinteger( L, i + TERM_FIRST_KEY );
    return 1; 
  }
}

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE term_map[] = 
{
  { LSTRKEY( "clrscr" ), LFUNCVAL( luaterm_clrscr ) },
  { LSTRKEY( "clreol" ), LFUNCVAL( luaterm_clreol ) },
  { LSTRKEY( "moveto" ), LFUNCVAL( luaterm_moveto ) },
  { LSTRKEY( "moveup" ), LFUNCVAL( luaterm_moveup ) },
  { LSTRKEY( "movedown" ), LFUNCVAL( luaterm_movedown ) },
  { LSTRKEY( "moveleft" ), LFUNCVAL( luaterm_moveleft ) },
  { LSTRKEY( "moveright" ), LFUNCVAL( luaterm_moveright ) },
  { LSTRKEY( "getlines" ), LFUNCVAL( luaterm_getlines ) },
  { LSTRKEY( "getcols" ), LFUNCVAL( luaterm_getcols ) },
  { LSTRKEY( "print" ), LFUNCVAL( luaterm_print ) },
  { LSTRKEY( "getcx" ), LFUNCVAL( luaterm_getcx ) },
  { LSTRKEY( "getcy" ), LFUNCVAL( luaterm_getcy ) },
  { LSTRKEY( "getchar" ), LFUNCVAL( luaterm_getchar ) },
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "__metatable" ), LROVAL( term_map ) },
  { LSTRKEY( "NOWAIT" ), LNUMVAL( TERM_INPUT_DONT_WAIT ) },
  { LSTRKEY( "WAIT" ), LNUMVAL( TERM_INPUT_WAIT ) },
#endif
  { LSTRKEY( "__index" ), LFUNCVAL( term_mt_index ) },
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_term( lua_State* L )
{
#ifdef BUILD_TERM
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  // Register methods
  luaL_register( L, AUXLIB_TERM, term_map );  
  
  // Set this table as itw own metatable
  lua_pushvalue( L, -1 );
  lua_setmetatable( L, -2 );  
  
  // Register the constants for "getch"
  lua_pushnumber( L, TERM_INPUT_DONT_WAIT );
  lua_setfield( L, -2, "NOWAIT" );  
  lua_pushnumber( L, TERM_INPUT_WAIT );
  lua_setfield( L, -2, "WAIT" );  
  
  return 1;
#endif // # if LUA_OPTIMIZE_MEMORY > 0
#else // #ifdef BUILD_TERM
  return 0;
#endif // #ifdef BUILD_TERM  
}

#endif // #ifdef ALCOR_LANG_PICOC
