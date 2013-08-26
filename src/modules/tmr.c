// Module for interfacing with timers
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

#include "platform.h"
#include "platform_conf.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VTIMER_NAME_LEN     6
#define MIN_VTIMER_NAME_LEN     5

#if defined( BUILD_LUA_INT_HANDLERS ) && defined( INT_TMR_MATCH )
#define HAS_TMR_MATCH_INT_LUA
#endif

#if defined( BUILD_PICOC_INT_HANDLERS ) && defined( INT_TMR_MATCH )
#define HAS_TMR_MATCH_INT_PICOC
#endif

#if defined ALCOR_LANG_MYBASIC

// ****************************************************************************
// Timer module for my-basic.

#elif defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// Timer module for picoLisp.

// (tmr-delay ['num] 'num) -> Nil
any tmr_delay(any ex) {
  timer_data_type period;
  unsigned id = PLATFORM_TIMER_SYS_ID;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) == 1) {
    // We only have 1 parameter. Assume
    // *sys-timer* and get the time period.
    NeedNum(ex, y);
    period = (timer_data_type)unBox(y);
  } else {
    // Minimum 2 args required here - the
    // id and the period. Ignore the others.
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);
    x = cdr(x), y = EVAL(car(x));
    NeedNum(ex, y);
    period = unBox(y);
  }
  platform_timer_delay(id, period);
  return Nil;
}

// (tmr-read ['num]) -> num
any tmr_read(any ex) {
  unsigned id = PLATFORM_TIMER_SYS_ID;
  timer_data_type res;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) > 0) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);      
  }

  res = platform_timer_op(id, PLATFORM_TIMER_OP_READ, 0);
  return box(res);
}

// (tmr-start ['num]) -> num
any tmr_start(any ex) {
  unsigned id = PLATFORM_TIMER_SYS_ID;
  timer_data_type res;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) > 0) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);
  }

  res = platform_timer_op(id, PLATFORM_TIMER_OP_START, 0);
  return box(res);
}

// (tmr-gettimediff 'num 'num 'num) -> num
any tmr_gettimediff(any ex) {
  timer_data_type start, end, res;
  unsigned id = PLATFORM_TIMER_SYS_ID;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  id = unBox(y); // get id.
  MOD_CHECK_TIMER(ex, id);

  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  start = unBox(y); // get start.

  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  end = unBox(y); // get end.

  res = platform_timer_get_diff_us(id, start, end);
  return box(res);
}

// (tmr-getdiffnow 'num 'num) -> num
any tmr_getdiffnow(any ex) {
  timer_data_type start, res;
  unsigned id;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  id = unBox(y); // get id.
  MOD_CHECK_TIMER(ex, id);

  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  start = unBox(y); // get start.
  res = platform_timer_get_diff_crt(id, start);
  return box(res);
}

// (tmr-getmindelay ['num]) -> num
any tmr_getmindelay(any ex) {
  timer_data_type res;
  unsigned id = PLATFORM_TIMER_SYS_ID;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) > 0) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);
  }

  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_MIN_DELAY, 0);
  return box(res);
}

// (tmr-getmaxdelay ['num]) -> num
any tmr_getmaxdelay(any ex) {
  timer_data_type res;
  unsigned id = PLATFORM_TIMER_SYS_ID;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) > 0) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);
  }

  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_MAX_DELAY, 0);
  return box(res);
}

// (tmr-setclock 'num 'num) -> num
any tmr_setclock(any ex) {
  u32 clock;
  unsigned id;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  NeedNum(ex, y);
  id = unBox(y); // get id.
  MOD_CHECK_TIMER(ex, id);

  x = cdr(x), y = EVAL(car(x));
  NeedNum(ex, y);
  clock = unBox(y); // get clock.

  clock = platform_timer_op(id, PLATFORM_TIMER_OP_SET_CLOCK, clock);
  return box(clock);
}

// (tmr-getclock ['num]) -> num
any tmr_getclock(any ex) {
  timer_data_type res;
  unsigned id = PLATFORM_TIMER_SYS_ID;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  if (plen(ex) > 0) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_TIMER(ex, id);
  }
  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_CLOCK, 0);
  return box(res);
}

// Look for all VIRTx timer identifiers.
#if VTMR_NUM_TIMERS > 0
// (tmr-decode 'sym) -> num
any tmr_decode(any ex) {
  char* pend;
  long res;
  any x, y;

  x = cdr(ex), y = EVAL(car(x));
  NeedSym(ex, y);
  char key[bufSize(y)];
  bufString(y, key);
  if (strlen(key) > MAX_VTIMER_NAME_LEN ||
      strlen(key) < MIN_VTIMER_NAME_LEN)
    return box(0);
  if (strncmp(key, "VIRT", 4))
    return box(0);
  res = strtol(key + 4, &pend, 10);
  if (*pend != '\0')
    return box(0);
  if (res >= VTMR_NUM_TIMERS)
    return box(0);

  return box(res + VTMR_FIRST_ID);
}

#endif // #if VTMR_NUM_TIMERS > 0

#elif defined ALCOR_LANG_PICOC

// ****************************************************************************
// Timer module for PicoC.

// A few helper functions.

static void get_timer_data(timer_data_type *d, val **param, int i)
{
  *d = param[i]->Val->UnsignedLongInteger;
}

static void set_timer_data(val *r, timer_data_type res)
{
  r->Val->UnsignedLongInteger = res;
}

// globals.
static const int sysid = PLATFORM_TIMER_SYS_ID;
#ifdef HAS_TMR_MATCH_INT_PICOC
static const int oneshot = PLATFORM_TIMER_INT_ONESHOT;
static const int cyclic = PLATFORM_TIMER_INT_CYCLIC;
#endif

// Library setup function
extern void tmr_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("tmr_SYS_TIMER", sysid);
#ifdef HAS_TMR_MATCH_INT_PICOC
  picoc_def_integer("tmr_INT_ONESHOT", oneshot);
  picoc_def_integer("tmr_INT_CYCLIC", cyclic);
#endif // #ifdef HAS_TMR_MATCH_INT_PICOC
#endif
}

// PicoC: tmr_delay(id, period);
static void tmr_delay(pstate *p, val *r, val **param, int n)
{
  timer_data_type period;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  get_timer_data(&period, param, 1);
  platform_timer_delay(id, period);
}

// PicoC: timervalue = tmr_read(id);
static void tmr_read(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  timer_data_type res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  res = platform_timer_op(id, PLATFORM_TIMER_OP_READ, 0);
  set_timer_data(r, res);
}

// PicoC: timervalue = tmr_start(id);
static void tmr_start(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  timer_data_type res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  res = platform_timer_op(id, PLATFORM_TIMER_OP_START, 0);
  set_timer_data(r, res);
}

// PicoC: time_us = tmr_gettimediff(id, start, end); 
static void tmr_gettimediff(pstate *p, val *r, val **param, int n)
{
  timer_data_type start, end, res;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  get_timer_data(&start, param, 1);
  get_timer_data(&end, param, 2);
  res = platform_timer_get_diff_us(id, start, end);
  set_timer_data(r, res);
}

// PicoC: time_us = tmr_getdiffnow(id, start);
static void tmr_getdiffnow(pstate *p, val *r, val **param, int n)
{
  timer_data_type start, res;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  get_timer_data(&start, param, 1);
  res = platform_timer_get_diff_crt(id, start);
  set_timer_data(r, res);
}

// PicoC: res = tmr_getmindelay(id);
static void tmr_getmindelay(pstate *p, val *r, val **param, int n)
{
  timer_data_type res;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_MIN_DELAY, 0);
  set_timer_data(r, res);		 
}

// PicoC: res = tmr_getmaxdelay(id);
static void tmr_getmaxdelay(pstate *p, val *r, val **param, int n)
{
  timer_data_type res;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_MAX_DELAY, 0);
  set_timer_data(r, res);
}

// PicoC: realclock = tmr_setclock(id, clock); 
static void tmr_setclock(pstate *p, val *r, val **param, int n)
{
  u32 clock;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  clock = param[1]->Val->UnsignedLongInteger;
  clock = platform_timer_op(id, PLATFORM_TIMER_OP_SET_CLOCK, clock);
  r->Val->UnsignedLongInteger = clock;
}

// PicoC: clock = tmr_getclock(id); 
static void tmr_getclock(pstate *p, val *r, val **param, int n)
{
  u32 res;
  unsigned id;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_TIMER(id);
  res = platform_timer_op(id, PLATFORM_TIMER_OP_GET_CLOCK, 0);
  r->Val->UnsignedLongInteger = res;
}

#ifdef HAS_TMR_MATCH_INT_PICOC
// TODO: For now, PicoC on Alcor6L doesn't support
// interrupts.
// static void tmr_set_match_int(pstate *p, val *r, val **param, int n)
// {}
#endif // HAS_TMR_MATCH_INT_PICOC

#if VTMR_NUM_TIMERS > 0
// Look for all VIRTx timer identifiers
static void tmr_decode(pstate *p, val *r, val **param, int n)
{
  const char *key = param[0]->Val->Identifier;
  char* pend;
  long res;

  if (strlen(key) > MAX_VTIMER_NAME_LEN || strlen(key) < MIN_VTIMER_NAME_LEN) {
    r->Val->LongInteger = 0;
    return;
  }
  if (strncmp(key, "VIRT", 4)) {
    r->Val->LongInteger = 0;
    return;
  }
  res = strtol(key + 4, &pend, 10);
  if (*pend != '\0') {
    r->Val->LongInteger = 0;
    return;
  }
  if (res >= VTMR_NUM_TIMERS) {
    r->Val->LongInteger = 0;
    return;
  }
  r->Val->LongInteger = (res + VTMR_FIRST_ID);
}
#endif // #if VTMR_NUM_TIMERS > 0 

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE tmr_variables[] = {
  {STRKEY("tmr_SYS_TIMER"), INT(sysid)},
#ifdef HAS_TMR_MATCH_INT_PICOC
  {STRKEY("tmr_INT_ONESHOT"), INT(oneshot)},
  {STRKEY("tmr_INT_CYCLIC"), INT(cyclic)},
#endif //#ifdef HAS_TMR_MATCH_INT
  {NILKEY, NILVAL}
};
#endif

// List of all library functions and their prototypes
const PICOC_REG_TYPE tmr_library[] = {
  {FUNC(tmr_delay), PROTO("void tmr_delay(unsigned int, unsigned long);")},
  {FUNC(tmr_read), PROTO("unsigned long tmr_read(unsigned int);")},
  {FUNC(tmr_start), PROTO("unsigned long tmr_start(unsigned int);")},
  {FUNC(tmr_gettimediff), PROTO("unsigned long tmr_gettimediff(unsigned int, unsigned long, unsigned long);")},
  {FUNC(tmr_getdiffnow), PROTO("unsigned long tmr_getdiffnow(unsigned int, unsigned long);")},
  {FUNC(tmr_getmindelay), PROTO("unsigned long tmr_getmindelay(unsigned int);")},
  {FUNC(tmr_getmaxdelay), PROTO("unsigned long tmr_getmaxdelay(unsigned int);")},
  {FUNC(tmr_setclock), PROTO("unsigned long tmr_setclock(unsigned int, unsigned long);")},
  {FUNC(tmr_getclock), PROTO("unsigned long tmr_getclock(unsigned int);")},
#if VTMR_NUM_TIMERS > 0
  {FUNC(tmr_decode), PROTO("unsigned long tmr_decode(char *);")},
#endif
  {NILFUNC, NILPROTO}
};

// Init library. 
extern void tmr_library_init(void)
{
  REGISTER("tmr.h", &tmr_lib_setup_func,
	   &tmr_library[0]);
}

#else

// ****************************************************************************
// Timer module for Lua.

// Helper function for the read/start functions
static int tmrh_timer_op( lua_State* L, int op )
{
  unsigned id;
  timer_data_type res;
    
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  res = platform_timer_op( id, op, 0 );
  lua_pushnumber( L, ( lua_Number )res );
  return 1;
}

// Lua: delay( id, period )
static int tmr_delay( lua_State* L )
{
  timer_data_type period;
  unsigned id;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  period = ( timer_data_type )luaL_checknumber( L, 2 );
  platform_timer_delay( id, period );
  return 0;
}

// Lua: timervalue = read( [id] )
static int tmr_read( lua_State* L )
{
  return tmrh_timer_op( L, PLATFORM_TIMER_OP_READ );
}

// Lua: timervalue = start( [id] )
static int tmr_start( lua_State* L )
{
  return tmrh_timer_op( L, PLATFORM_TIMER_OP_START );
}

// Lua: time_us = gettimediff( id, start, end )
static int tmr_gettimediff( lua_State* L )
{
  timer_data_type start, end, res;
  unsigned id;
    
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID ); 
  MOD_CHECK_TIMER( id );
  start = ( timer_data_type )luaL_checknumber( L, 2 );
  end = ( timer_data_type )luaL_checknumber( L, 3 );  
  res = platform_timer_get_diff_us( id, start, end );
  lua_pushnumber( L, ( lua_Number )res );
  return 1;    
}

// Lua: time_us = getdiffnow( id, start )
static int tmr_getdiffnow( lua_State *L )
{
  timer_data_type start, res;
  unsigned id;

  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  start = ( timer_data_type )luaL_checknumber( L, 2 );
  res = platform_timer_get_diff_crt( id, start );
  lua_pushnumber( L, ( lua_Number )res );
  return 1;
}

// Lua: res = getmindelay( [id] )
static int tmr_getmindelay( lua_State* L )
{
  timer_data_type res;
  unsigned id;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  res = platform_timer_op( id, PLATFORM_TIMER_OP_GET_MIN_DELAY, 0 );
  lua_pushnumber( L, ( lua_Number )res );
  return 1;
}

// Lua: res = getmaxdelay( [id] )
static int tmr_getmaxdelay( lua_State* L )
{
  timer_data_type res;
  unsigned id;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  res = platform_timer_op( id, PLATFORM_TIMER_OP_GET_MAX_DELAY, 0 );
  lua_pushnumber( L, ( lua_Number )res );
  return 1;
}

// Lua: realclock = setclock( id, clock )
static int tmr_setclock( lua_State* L )
{
  u32 clock;
  unsigned id;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  clock = ( u32 )luaL_checkinteger( L, 2 );
  clock = platform_timer_op( id, PLATFORM_TIMER_OP_SET_CLOCK, clock );
  lua_pushinteger( L, clock );
  return 1;
}

// Lua: clock = getclock( [id] )
static int tmr_getclock( lua_State* L )
{
  u32 res;
  unsigned id;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  res = platform_timer_op( id, PLATFORM_TIMER_OP_GET_CLOCK, 0 );
  lua_pushinteger( L, res );
  return 1;
}

#ifdef HAS_TMR_MATCH_INT_LUA
// Lua: set_match_int( id, timeout, type )
static int tmr_set_match_int( lua_State *L )
{
  unsigned id;
  u32 res;
  
  id = ( unsigned )luaL_optinteger( L, 1, PLATFORM_TIMER_SYS_ID );
  MOD_CHECK_TIMER( id );
  res = platform_timer_set_match_int( id, ( timer_data_type )luaL_checknumber( L, 2 ), ( int )luaL_checkinteger( L, 3 ) );
  if( res == PLATFORM_TIMER_INT_TOO_SHORT )
    return luaL_error( L, "timer interval too small" );
  else if( res == PLATFORM_TIMER_INT_TOO_LONG )
    return luaL_error( L, "timer interval too long" );
  else if( res == PLATFORM_TIMER_INT_INVALID_ID )
    return luaL_error( L, "match interrupt cannot be set on this timer" );
  return 0;
}
#endif // #ifdef HAS_TMR_MATCH_INT_LUA

#if VTMR_NUM_TIMERS > 0
// __index metafunction for TMR
// Look for all VIRTx timer identifiers
static int tmr_mt_index( lua_State* L )
{
  const char *key = luaL_checkstring( L ,2 );
  char* pend;
  long res;
  
  if( strlen( key ) > MAX_VTIMER_NAME_LEN || strlen( key ) < MIN_VTIMER_NAME_LEN )
    return 0;
  if( strncmp( key, "VIRT", 4 ) )
    return 0;  
  res = strtol( key + 4, &pend, 10 );
  if( *pend != '\0' )
    return 0;
  if( res >= VTMR_NUM_TIMERS )
    return 0;
  lua_pushinteger( L, VTMR_FIRST_ID + res );
  return 1;
}
#endif // #if VTMR_NUM_TIMERS > 0

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE tmr_map[] = 
{
  { LSTRKEY( "delay" ), LFUNCVAL( tmr_delay ) },
  { LSTRKEY( "read" ), LFUNCVAL( tmr_read ) },
  { LSTRKEY( "start" ), LFUNCVAL( tmr_start ) },
  { LSTRKEY( "gettimediff" ), LFUNCVAL( tmr_gettimediff ) },
  { LSTRKEY( "getdiffnow" ), LFUNCVAL( tmr_getdiffnow ) },
  { LSTRKEY( "getmindelay" ), LFUNCVAL( tmr_getmindelay ) },
  { LSTRKEY( "getmaxdelay" ), LFUNCVAL( tmr_getmaxdelay ) },
  { LSTRKEY( "setclock" ), LFUNCVAL( tmr_setclock ) },
  { LSTRKEY( "getclock" ), LFUNCVAL( tmr_getclock ) },
#ifdef HAS_TMR_MATCH_INT_LUA
  { LSTRKEY( "set_match_int" ), LFUNCVAL( tmr_set_match_int ) },
#endif  
#if LUA_OPTIMIZE_MEMORY > 0 && VTMR_NUM_TIMERS > 0
  { LSTRKEY( "__metatable" ), LROVAL( tmr_map ) },
#endif
#if VTMR_NUM_TIMERS > 0  
  { LSTRKEY( "__index" ), LFUNCVAL( tmr_mt_index ) },
#endif  
#if LUA_OPTIMIZE_MEMORY > 0
  { LSTRKEY( "SYS_TIMER" ), LNUMVAL( PLATFORM_TIMER_SYS_ID ) },
#endif
#if LUA_OPTIMIZE_MEMORY > 0 && defined( BUILD_LUA_INT_HANDLERS )
  { LSTRKEY( "INT_ONESHOT" ), LNUMVAL( PLATFORM_TIMER_INT_ONESHOT ) },
  { LSTRKEY( "INT_CYCLIC" ), LNUMVAL( PLATFORM_TIMER_INT_CYCLIC ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_tmr( lua_State *L )
{
#if LUA_OPTIMIZE_MEMORY > 0
  return 0;
#else // #if LUA_OPTIMIZE_MEMORY > 0
  luaL_register( L, AUXLIB_TMR, tmr_map );
#if VTMR_NUM_TIMERS > 0
  // Set this table as its own metatable
  lua_pushvalue( L, -1 );
  lua_setmetatable( L, -2 );  
#endif // #if VTMR_NUM_TIMERS > 0
  MOD_REG_NUMBER( L, "SYS_TIMER", PLATFORM_TIMER_SYS_ID );
#ifdef HAS_TMR_MATCH_INT_LUA
  MOD_REG_NUMBER( L, "INT_ONESHOT", PLATFORM_TIMER_INT_ONESHOT );
  MOD_REG_NUMBER( L, "INT_CYCLIC", PLATFORM_TIMER_INT_CYCLIC );
#endif //#ifdef HAS_TMR_MATCH_INT_LUA
  return 1;
#endif // #if LUA_OPTIMIZE_MEMORY > 0
}

#endif // #ifdef ALCOR_LANG_PICOLISP
