// Module for interfacing with platform data
// Modified to include support for Alcor6L.

#include "platform.h"

#define MACRO_NAME( x ) MACRO_AGAIN( x )
#define MACRO_AGAIN( x ) #x

#if defined ALCOR_LANG_MYBASIC

// ****************************************************************************
// Platform module for my-basic.

#include "my_basic.h"

// v = pd_platform()
int pd_platform(mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(ALCOR_PLATFORM));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(ALCOR_PLATFORM), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

// V = pd_cpu()
int pd_cpu(mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(ALCOR_CPU));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(ALCOR_CPU), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

// v = pd_board()
int pd_board(mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(MACRO_NAME(ALCOR_BOARD));
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, MACRO_NAME(ALCOR_BOARD), len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

#endif // ALCOR_LANG_MYBASIC

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// Platform module for picoLisp.

#include "pico.h"

// (pd-platform) -> sym
any pd_platform(any x) {
   return mkStr(MACRO_NAME(ALCOR_PLATFORM));
}

// (pd-cpu) -> sym
any pd_cpu(any x) {
   return mkStr(MACRO_NAME(ALCOR_CPU));
}

// (pd-board) -> sym
any pd_board(any x) {
   return mkStr(MACRO_NAME(ALCOR_BOARD));
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// Platform module for PicoC.

#include "interpreter.h"
#include "rotable.h"
#include "picoc_mod.h"

// picoc: platform = pd_platform();
static void pd_platform(pstate *p, val *r, val **param, int n)
{
  r->Val->Identifier = MACRO_NAME(ALCOR_PLATFORM);
}

// picoc: cpuname = pd_cpu();
static void pd_cpu(pstate *p, val *r, val **param, int n)
{
  r->Val->Identifier = MACRO_NAME(ALCOR_CPU);
}

// picoc: boardname = pd_board();
static void pd_board(pstate *p, val *r, val **param, int n)
{
  r->Val->Identifier = MACRO_NAME(ALCOR_BOARD);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

// list of all library functions and their prototypes
const PICOC_REG_TYPE pd_library[] = {
  {FUNC(pd_platform), PROTO("char *pd_platform(void);")},
  {FUNC(pd_board), PROTO("char *pd_board(void);")},
  {FUNC(pd_cpu), PROTO("char *pd_cpu(void);")},
  {NILFUNC, NILPROTO}
};

// init library
extern void pd_library_init(void)
{
  REGISTER("pd.h", NULL, &pd_library[0]);
}

#endif // ALCOR_LANG_PICOC

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// Platform module for eLua.

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "auxmods.h"
#include "lrotable.h"

// Lua: platform = platform()
static int pd_platform( lua_State* L )
{
  lua_pushstring( L, MACRO_NAME( ALCOR_PLATFORM ) );
  return 1;
}

// Lua: cpuname = cpu()
static int pd_cpu( lua_State* L )
{
  lua_pushstring( L, MACRO_NAME( ALCOR_CPU ) ); 
  return 1;
}

// Lua: boardname = board()
static int pd_board( lua_State* L )
{
  lua_pushstring( L, MACRO_NAME( ALCOR_BOARD ) );
  return 1;
}

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE pd_map[] = 
{
  { LSTRKEY( "platform" ), LFUNCVAL( pd_platform ) }, 
  { LSTRKEY( "cpu" ), LFUNCVAL( pd_cpu ) },
  { LSTRKEY( "board" ), LFUNCVAL( pd_board ) },
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_pd( lua_State* L )
{
  LREGISTER( L, AUXLIB_PD, pd_map );
}

#endif // #ifdef ALCOR_LANG_LUA
