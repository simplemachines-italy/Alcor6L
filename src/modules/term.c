// Module for interfacing with terminal functions
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
#include "term.h"
#include <string.h>
#include "platform_conf.h"

// Common for all languages.
#undef _D
#define _D(x) #x
static const char* term_key_names[] = {TERM_KEYCODES};

#if defined ALCOR_LANG_PICOLISP && defined BUILD_TERM

// ****************************************************************************
// Terminal module for picoLisp.

// (term-clrscr) -> Nil
any plisp_term_clrscr(any x) {
  term_clrscr();
  return Nil;
}

// (term-clreol) -> Nil
any plisp_term_clreol(any x) {
  term_clreol();
  return Nil;
}

// (term-moveto 'num 'num) -> Nil
any plisp_term_moveto(any ex) { 
  any x, y;
  long n1, n2;
  
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n1 = unBox(y);
  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  n2 = unBox(y);
  term_gotoxy(n1, n2);
  return Nil;
}

// (term-moveup 'num) -> Nil
any plisp_term_moveup(any ex) {
  any x, y;
  long n;
  
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n = unBox(y);
  term_up(n);
  return Nil;
}

// (term-movedown 'num) -> Nil
any plisp_term_movedown(any ex) {
  any x, y;
  long n;
  
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n = unBox(y);
  term_down(n);
  return Nil;
}

// (term-moveleft 'num) -> Nil
any plisp_term_moveleft(any ex) {
  any x, y;
  long n;
  
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n = unBox(y);
  term_left(n);
  return Nil;
}

// (term-moveright 'num) -> Nil
any plisp_term_moveright(any ex) {
  any x, y;
  long n;
  
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n = unBox(y);
  term_right(n);
  return Nil;
}

// (term-getlines) -> num
any plisp_term_getlines(any x) {
  x = box(term_get_lines());
  return x;
}

// (term-getcols) -> num
any plisp_term_getcols(any x) {
  x = box(term_get_cols());
  return x;
}

// Helpers for picoLisp terminal print
// function.
static void outString_term(char *s) {
  while (*s)
    term_putch((u8)*s++);
}

static void outNum_term(long n) {
  char buf[BITS/2];

  bufNum(buf, n);
  outString_term(buf);
}

static void ptermh_prin(any x) {
  if (!isNil(x)) {
    if (isNum(x))
      outNum_term(unBox(x));
    else if (isSym(x)) {
      int i, c;
      word w;
      u8 byte;
      
      for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
        if (c != '^') {
          byte = c;
	  term_putch(byte);
	}
        else if (!(c = getByte(&i, &w, &x))) {
	  byte = '^';
          term_putch(byte);
        }
        else if (c == '?') {
          byte = 127;
	  term_putch(byte);
        }
        else {
          c &= 0x1F;
          byte = (u8)c;
	  term_putch(byte);
	}
      }
    }
    else {
      while (ptermh_prin(car(x)), !isNil(x = cdr(x))) {
	if (!isCell(x)) {
	  ptermh_prin(x);
          break;
	}
      }
    }
  }
}

// (term-prinl 'num 'num 'any ..) -> any
any plisp_term_prinl(any ex) {
  any x, y;
  long n1, n2;
  
  // get coordinates.
  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  n1 = unBox(y);
  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  n2 = unBox(y);
  term_gotoxy(n1, n2);

  // and now, print.
  x = cdr(x), y = EVAL(car(x));
  ptermh_prin(y);
  return y;
}

// (term-getcx) -> num
any plisp_term_getcx(any x) {
  x = box(term_get_cx());
  return x;
}

// (term-getcy) -> num
any plisp_term_getcy(any x) {
  x = box(term_get_cy());
  return x;
}

// (term-getchar ['sym]) -> num
any plisp_term_getchar(any ex) {
  any x, y;
  int temp = TERM_INPUT_WAIT, ret;

  // if number of args is > 0
  // get value; else getchar()
  // will wait.
  if (plen(ex) > 0) {
    x = cdr(ex);
    NeedNum(ex, y = EVAL(car(x)));
    return ((ret = term_getch(temp = unBox(y))) == -1?
	    Nil : box(ret));
  }
  return ((ret = term_getch(temp)) == -1?
	  Nil : box(ret));
}

// (term-decode 'sym) -> num | Nil
any plisp_term_decode(any ex) {
  any x, y;
  unsigned i, total = sizeof(term_key_names) / sizeof(char*);

  x = cdr(ex), y = EVAL(car(x));
  NeedSymb(ex, y);
  char key[bufSize(y)];
  bufString(y, key);

  if (*key != 'K')
    return Nil;

  for (i = 0; i < total; i++)
    if (!strcmp(key, term_key_names[i]))
      break;

  if (i == total)
    return Nil;
  else
    return box(i + TERM_FIRST_KEY);
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC && defined BUILD_TERM

// ****************************************************************************
// Terminal module for PicoC.

// Platform variables
static const int no_wait = TERM_INPUT_DONT_WAIT;
static const int wait = TERM_INPUT_WAIT;

// Library setup function
extern void term_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("term_WAIT", wait);
  picoc_def_integer("term_NOWAIT", no_wait);
#endif
}

// picoc: term_clrscr();
static void pterm_clrscr(pstate *p, val *r, val **param, int n)
{
  term_clrscr();
}

// picoc: term_clreol();
static void pterm_clreol(pstate *p, val *r, val **param, int n)
{
  term_clreol();
}

// picoc: term_moveto(x y);
static void pterm_moveto(pstate *p, val *r, val **param, int n)
{
  unsigned x, y;
  x = param[0]->Val->UnsignedInteger;
  y = param[1]->Val->UnsignedInteger;

  term_gotoxy(x, y);
}

// picoc: term_moveup(lines);
static void pterm_moveup(pstate *p, val *r, val **param, int n)
{
  unsigned delta;

  delta = param[0]->Val->UnsignedInteger;
  term_up(delta);
}

// picoc: movedown(lines);
static void pterm_movedown(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_down(delta);
}

// picoc: term_moveleft(cols);
static void pterm_moveleft(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_left(delta);
}

// picoc: term_moveright(cols);
static void pterm_moveright(pstate *p, val *r, val **param, int n)
{
  unsigned delta;
  delta = param[0]->Val->UnsignedInteger;

  term_right(delta);
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

  r->Val->UnsignedLongInteger = len;
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

// picoc: key = term_getchar(mode);
static void pterm_getchar(pstate *p, val *r, val **param, int n)
{
  int temp, res;

  temp = param[0]->Val->Integer;
  res = term_getch(temp);
  if (!res) {
    r->Val->Integer = -1;
    return;
  }
  r->Val->Integer = res;
}

// Look for all KC_xxxx codes
// picoc: term_decode(str);
static void pterm_decode(pstate *p, val *r, val **param, int n)
{
  const char *key = param[0]->Val->Identifier;
  unsigned i, total = sizeof(term_key_names) / sizeof(char*);

  if (!key || *key != 'K') {
    r->Val->Integer = 0;
    return;
  }
  for (i = 0; i < total; i++)
    if (!strcmp(key, term_key_names[i]))
      break;
  if (i == total) {
    r->Val->Integer = 0;
    return;
  } else {
    r->Val->Integer = i + TERM_FIRST_KEY;
    return;
  }
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE term_variables[] = {
  {STRKEY("term_WAIT"), INT(wait)},
  {STRKEY("term_NOWAIT"), INT(no_wait)},
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE term_library[] = {
  {FUNC(pterm_clrscr), PROTO("void term_clrscr(void);")},
  {FUNC(pterm_clreol), PROTO("void term_clreol(void);")},
  {FUNC(pterm_moveto), PROTO("void term_moveto(unsigned int, unsigned int);")},
  {FUNC(pterm_moveup), PROTO("void term_moveup(unsigned int);")},
  {FUNC(pterm_movedown), PROTO("void term_movedown(unsigned int);")},
  {FUNC(pterm_moveleft), PROTO("void term_moveleft(unsigned int);")},
  {FUNC(pterm_moveright), PROTO("void term_moveright(unsigned int);")},
  {FUNC(pterm_getlines), PROTO("unsigned int term_getlines(void);")},
  {FUNC(pterm_getcols), PROTO("unsigned int term_getcols(void);")},
  {FUNC(pterm_puts), PROTO("unsigned long term_puts(unsigned int, unsigned int, char *);")},
  {FUNC(pterm_putch), PROTO("unsigned int term_putch(char);")},
  {FUNC(pterm_getcx), PROTO("unsigned int term_getcx(void);")},
  {FUNC(pterm_getcy), PROTO("unsigned int term_getcy(void);")},
  {FUNC(pterm_getchar), PROTO("int term_getchar(int);")},
  {FUNC(pterm_decode), PROTO("int term_decode(char *);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void term_library_init(void)
{
  REGISTER("term.h", &term_lib_setup_func,
	   &term_library[0]);
}

#endif // ALCOR_LANG_PICOC

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// Terminal module for Lua.

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

#endif // #if defined ALCOR_LANG_LUA && defined BUILD_TERM
