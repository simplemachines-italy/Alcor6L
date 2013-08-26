// Interface with core services
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
# include "legc.h"
#endif

#include "platform.h"
#include "platform_conf.h"
#include "linenoise.h"
#include "shell.h"
#include <string.h>
#include <stdlib.h>

#if defined( USE_GIT_REVISION )
#include "git_version.h"
#else
#include "version.h"
#endif

#if defined ALCOR_LANG_MYBASIC

// ****************************************************************************
// eLua core module for my-basic.

// v = pd_platform()
int elua_version(mb_interpreter_t* s, void **l) {
  int result = MB_FUNC_OK;
  unsigned len = strlen(ELUA_STR_VERSION);
  char *str = (char *)mb_malloc(len + 1);
  memcpy(str, ELUA_STR_VERSION, len);
  str[len] = '\0';

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_attempt_close_bracket(s, l));
  mb_check(mb_push_string(s, l, str));
  return result;
}

// elua_save_history("fname")
int elua_save_history(mb_interpreter_t* s, void **l) {
  int fun_result = MB_FUNC_OK;
  char *fname = 0;

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_pop_string(s, l, &fname));
  mb_check(mb_attempt_close_bracket(s, l));
#ifdef BUILD_LINENOISE
  int res = linenoise_savehistory(LINENOISE_ID_LUA, fname);
  if (res == 0)
    printf("History saved to %s.\n", fname);
  else if (res == LINENOISE_HISTORY_NOT_ENABLED)
    printf("linenoise not enabled for MY-BASIC.\n");
  else if (res = LINENOISE_HISTORY_EMPTY)
    printf("History empty, nothing to save.\n");
  else
    printf("Unable to save history to %s.\n", fname);
  return fun_result;
#else
  printf("linenoise support not enabled.");
  return fun_result;
#endif
}

// elua_shell("str")
int elua_shell(mb_interpreter_t* s, void **l) {
  int res = MB_FUNC_OK;
  char *cmdcpy, *pcmd = 0;

  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_pop_string(s, l, &pcmd));
  mb_check(mb_attempt_close_bracket(s, l));

  // "+2" below comes from the string terminator (+1) and the '\n' 
  // that will be added by the shell code (+1)
  if ((cmdcpy = (char *)malloc(strlen(pcmd) + 2)) == NULL) {
    printf("not enough memory for elua-shell");
    return res;
  }

  strcpy(cmdcpy, pcmd);
  shellh_execute_command(cmdcpy, 0);
  free(cmdcpy);

  return res;
}

#elif defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// eLua core module for picoLisp.

any elua_version(any x) {
  return mkStr(ELUA_STR_VERSION);
}

any elua_save_history(any x) {
#ifdef BUILD_LINENOISE
  int res; // holds result.
  any y;   // cdr(x)
  
  y = cdr(x);
  y = EVAL(car(y));
  // holds file name.
  char fname[bufSize(y)];
  NeedSym(x, y);
  bufString(y, fname);
  res = linenoise_savehistory(LINENOISE_ID_LUA, fname);
  if (res == 0)
    printf("History saved to %s.\n", fname);
  else if (res == LINENOISE_HISTORY_NOT_ENABLED)
    outString("linenoise not enabled for picoLisp.\n");
  else if (res = LINENOISE_HISTORY_EMPTY)
    outString("History empty, nothing to save.\n");
  else
    printf("Unable to save history to %s.\n", fname);
  return y;
#else
  err(NULL, NULL, "linenoise support not enabled.");
#endif
}

// (elua-shell 'sym) -> sym|Nil
any elua_shell(any x) {
  any y = cdr(x);
  char *cmdcpy;
  const SHELL_COMMAND *t;

  y = EVAL(car(y));
  char pcmd[bufSize(y)];
  NeedSym(x, y);
  bufString(y, pcmd);
  // "+2" below comes from the string terminator (+1) and the '\n'
  // that will be added by the shell code (+1)
  if ((cmdcpy = (char *)malloc(strlen(pcmd) + 2)) == NULL)
    err(NULL, NULL, "not enough memory for elua-shell");
  strcpy(cmdcpy, pcmd);
  t = shellh_execute_command(cmdcpy, 0);
  free(cmdcpy);
  if (!t)
    return Nil;
  else
    return mkStr(t->cmd);
}

#elif defined ALCOR_LANG_PICOC

// ****************************************************************************
// eLua core module for PicoC.

// PicoC: elua_version();
static void elua_version(pstate *p, val *r, val **param, int n)
{
  r->Val->Identifier = ELUA_STR_VERSION;
}

// PicoC: elua_save_history(fname);
static void elua_save_history(pstate *p, val *r, val **param, int n)
{
#ifdef BUILD_LINENOISE
  const char *fname = param[0]->Val->Identifier;
  int res;
 
  res = linenoise_savehistory(LINENOISE_ID_LUA, fname);
  if (res == 0)
    printf("History saved to %s.\n", fname);
  else if (res == LINENOISE_HISTORY_NOT_ENABLED)
    printf( "linenoise not enabled for Lua.\n" );
  else if (res == LINENOISE_HISTORY_EMPTY)
    printf("History empty, nothing to save.\n");
  else
    printf("Unable to save history to %s.\n", fname);
#else
  return pmod_error("linenoise support not enabled.");
#endif
}

// PicoC: elua_shell(shell_command);
static void elua_shell(pstate *p, val *r, val **param , int n)
{
  const char *pcmd = param[0]->Val->Identifier;
  char *cmdcpy;

  // "+2" below comes from the string terminator (+1) and the '\n'
  // that will be added by the shell code (+1)
  if ((cmdcpy = (char *)malloc(strlen(pcmd) + 2)) == NULL)
    return pmod_error("not enough memory for elua_shell");
  strcpy(cmdcpy, pcmd);
  shellh_execute_command(cmdcpy, 0);
  free(cmdcpy);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

// List of all library functions and their prototypes
const PICOC_REG_TYPE elua_library[] = {
  {FUNC(elua_version), PROTO("char *elua_version(void);")},
  {FUNC(elua_save_history), PROTO("void elua_save_history(char *);")},
  {FUNC(elua_shell), PROTO("void elua_shell(char *);")},
  {NILFUNC, NILPROTO}
};

// Init library.
extern void elua_library_init(void)
{
  REGISTER("elua.h", NULL, &elua_library[0]);
}

#else

// ****************************************************************************
// eLua core module for Lua.

// Lua: elua.egc_setup( mode, [ memlimit ] )
static int elua_egc_setup( lua_State *L )
{
  int mode = luaL_checkinteger( L, 1 );
  unsigned memlimit = 0;

  if( lua_gettop( L ) >= 2 )
    memlimit = ( unsigned )luaL_checkinteger( L, 2 );
  legc_set_mode( L, mode, memlimit );
  return 0;
}

// Lua: elua.version()
static int elua_version( lua_State *L )
{
  lua_pushstring( L, ELUA_STR_VERSION );
  return 1;
}

// Lua: elua.save_history( filename )
// Only available if linenoise support is enabled
static int elua_save_history( lua_State *L )
{
#ifdef BUILD_LINENOISE
  const char* fname = luaL_checkstring( L, 1 );
  int res;

  res = linenoise_savehistory( LINENOISE_ID_LUA, fname );
  if( res == 0 )
    printf( "History saved to %s.\n", fname );
  else if( res == LINENOISE_HISTORY_NOT_ENABLED )
    printf( "linenoise not enabled for Lua.\n" );
  else if( res == LINENOISE_HISTORY_EMPTY )
    printf( "History empty, nothing to save.\n" );
  else
    printf( "Unable to save history to %s.\n", fname );
  return 0;
#else // #ifdef BUILD_LINENOISE
  return luaL_error( L, "linenoise support not enabled." );
#endif // #ifdef BUILD_LINENOISE
}

// Lua: elua.shell( <shell_command> )
static int elua_shell( lua_State *L )
{
  const char *pcmd = luaL_checkstring( L, 1 );
  char *cmdcpy;

  // "+2" below comes from the string terminator (+1) and the '\n'
  // that will be added by the shell code (+1)
  if( ( cmdcpy = ( char* )malloc( strlen( pcmd ) + 2 ) ) == NULL )
    return luaL_error( L, "not enough memory for elua_shell" );
  strcpy( cmdcpy, pcmd );
  shellh_execute_command( cmdcpy, 0 );
  free( cmdcpy );
  return 0;
}

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE elua_map[] =
{
  { LSTRKEY( "egc_setup" ), LFUNCVAL( elua_egc_setup ) },
  { LSTRKEY( "version" ), LFUNCVAL( elua_version ) },
  { LSTRKEY( "save_history" ), LFUNCVAL( elua_save_history ) },
  { LSTRKEY( "shell" ), LFUNCVAL( elua_shell ) },
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "EGC_NOT_ACTIVE" ), LNUMVAL( EGC_NOT_ACTIVE ) },
  { LSTRKEY( "EGC_ON_ALLOC_FAILURE" ), LNUMVAL( EGC_ON_ALLOC_FAILURE ) },
  { LSTRKEY( "EGC_ON_MEM_LIMIT" ), LNUMVAL( EGC_ON_MEM_LIMIT ) },
  { LSTRKEY( "EGC_ALWAYS" ), LNUMVAL( EGC_ALWAYS ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_elua( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else
  luaL_register( L, AUXLIB_ELUA, elua_map );
  MOD_REG_NUMBER( L, "EGC_NOT_ACTIVE", EGC_NOT_ACTIVE );
  MOD_REG_NUMBER( L, "EGC_ON_ALLOC_FAILURE", EGC_ON_ALLOC_FAILURE );
  MOD_REG_NUMBER( L, "EGC_ON_MEM_LIMIT", EGC_ON_MEM_LIMIT );
  MOD_REG_NUMBER( L, "EGC_ALWAYS", EGC_ALWAYS );
  return 1;
#endif
}

#endif // #ifdef ALCOR_LANG_PICOLISP
