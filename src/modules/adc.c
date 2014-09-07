// Module for interfacing with ADC
// Modified to include support for Alcor6L.

// Language specific includes.
//
#if defined ALCOR_LANG_TINYSCHEME
# include "scheme.h"
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

#include "platform.h"
#include "common.h"
#include "platform_conf.h"
#include "elua_adc.h"

#ifdef BUILD_ADC

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// ADC (Analog to digital converter) module for picoLisp.

// (adc-maxval 'num) -> num
any plisp_adc_maxval(any ex) {
  unsigned id;
  u32 res;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, adc, id);

  res = platform_adc_get_maxval(id);
  return box(res);
}

// (adc-setclock 'num 'num 'num) -> num
any plisp_adc_setclock(any ex) {
  s32 sfreq; // signed version for negative checking.
  u32 freq;
  unsigned id, timer_id = 0;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get adc id.
  MOD_CHECK_ID(ex, adc, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  sfreq = unBox(y); // get frequency.
  if (sfreq < 0)
    err(ex, y, "frequency must be 0 or positive");

  freq = (u32) sfreq;
  if (freq > 0) {
    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    timer_id = unBox(y); // get timer id.
    MOD_CHECK_ID(ex, timer, timer_id);
    MOD_CHECK_RES_ID(ex, adc, id, timer, timer_id);
  }

  platform_adc_set_timer(id, timer_id);
  freq = platform_adc_set_clock(id, freq);
  return box(freq);
}

// (adc-isdone) -> T | Nil
any plisp_adc_isdone(any ex) {
  unsigned id;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, adc, id);

  return platform_adc_is_done(id) == 0 ?
    T : Nil;
}

// (adc-setblocking 'num 'num) -> Nil
any plisp_adc_setblocking(any ex) {
  unsigned id, mode;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, adc, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  mode = unBox(y); // get mode.

  platform_adc_set_blocking(id, mode);
  return Nil;
}

// (adc-setsmoothing 'num 'num) -> num
any plisp_adc_setsmoothing(any ex) {
  unsigned id, length, res;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, adc, id);

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  length = unBox(y); // get length.
  if (!(length & (length - 1))) {
    res = platform_adc_set_smoothing(id, length);
    if (res == PLATFORM_ERR)
      err(ex, NULL, "Buffer allocation failed.");
    else
      return box(res);
  } else {
    err(ex, y, "Length must be power of 2");
  }
}

// (adc-sample 'num 'num) -> Nil
any plisp_adc_sample(any ex) {
  unsigned id, count = 0, nchans = 1;
  int res, i;
  any x, y, s;

  // get count value, the second parameter
  // in the picoLisp function call.
  s = cdr(ex), y = EVAL(car(s)); s = cdr(s);
  NeedNum(ex, y = EVAL(car(s)));
  count = unBox(y);

  // validate count.
  if ((count == 0) || count & (count - 1))
    err(ex, y, "count must be power of 2 and > 0");

  // get first parameter in the function
  // call.
  x = cdr(ex), y = EVAL(car(x));
  // If first parameter is a table,
  // extract channel list.
  if (isCell(y)) {
    nchans = length(y);
    for (i = 0; i < nchans; i++) {
      NeedNum(y, car(y));
      id = unBox(car(y));
      MOD_CHECK_ID(ex, adc, id);
      res = adc_setup_channel(id, intlog2(count));
      if (res != PLATFORM_OK)
	err(ex, y, "sampling setup failed");
    }
    // initiate sampling.
    platform_adc_start_sequence();
  } else if (isNum(y)) {
    NeedNum(ex, y);
    id = unBox(y);
    MOD_CHECK_ID(ex, adc, id);
    res = adc_setup_channel(id, intlog2(count));
    if (res != PLATFORM_OK)
      err(ex, y, "sampling setup failed");
    platform_adc_start_sequence();
  } else {
    err(ex, y, "invalid channel selection");
  }
  return Nil;
}

// (adc-getsample 'num) -> num
any plisp_adc_getsample(any ex) {
  unsigned id;
  any x, y;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id.
  MOD_CHECK_ID(ex, adc, id);

  // If we have at least one sample, return it.
  if (adc_wait_samples(id, 1) >= 1)
    return box(adc_get_processed_sample(id));

  return Nil;
}

// (adc-getsamples 'num ['num]) -> lst
any plisp_adc_getsamples(any ex) {
#ifdef BUF_ENABLE_ADC
  unsigned id, i;
  u16 bcnt, count = 0;
  any x, y;
  cell c1;

  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id
  MOD_CHECK_ID(ex, adc, id);

  if (plen(ex) >= 2) {
    x = cdr(x);
    NeedNum(ex, y = EVAL(car(x)));
    count = (u16)unBox(y); // get count
  }

  bcnt = adc_wait_samples(id, count);

  // If count is zero, grab all samples
  if (count == 0)
    count = bcnt;

  // Don't pull more samples than are available
  if (count > bcnt)
    count = bcnt;

  // Make the list of adc samples
  Push(c1, y = cons(box(adc_get_processed_sample(id)), Nil));
  for (i = 1; i < count - 1; i++)
    Push(c1, y = cons(box(adc_get_processed_sample(id)), y));

  return Pop(c1);
#else
  err(NULL, NULL, "BUF_ENABLE_ADC not defined");
#endif
}

// Helper function:
//
// Insert one element (value) in a list (list) at a
// given position (pos). The function will return the
// new list with the inserted value.

static any ins_element(any list, int pos, int value) {
  any temp, tab;
  int i;

  tab = temp = list;
  temp = cons(box(value), nCdr(pos - 1, temp));
  for (i = pos - 1; i > 0; i--) {
    temp = cons(car(nth(i, tab)), temp);
  }
  return temp;
}

// (adc-insertsamples 'num 'lst 'num 'num) -> lst
any plisp_adc_insertsamples(any ex) {
#ifdef BUF_ENABLE_ADC
  unsigned id, i, startidx;
  u16 bcnt, count;
  any x, y, tab;
  
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  id = unBox(y); // get id
  MOD_CHECK_ID(ex, adc, id);

  // get the list of samples
  x = cdr(x);
  NeedLst(ex, y = EVAL(car(x)));
  tab = y;

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  startidx = unBox(y); // get startidx
  if (startidx <= 0)
    err(ex, y, "idx must be > 0");

  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  count = unBox(y); // get count
  if (count == 0)
    err(ex, y, "count must be > 0");

  bcnt = adc_wait_samples(id, count);

  for (i = startidx; i < (count + startidx); i++)
    tab = ins_element(tab, i, adc_get_processed_sample(id));

  return tab;
#else
  err(NULL, NULL, "BUF_ENABLE_ADC not defined");
#endif
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// ADC (Analog to digital converter) module for PicoC.

// PicoC: data = adc_maxval(id);
static void adc_maxval(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  u32 res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  res = platform_adc_get_maxval(id);
  
  r->Val->UnsignedLongInteger = res;
}

// PicoC: realclock = adc_setclock(id, freq, timer_id);
static void adc_setclock(pstate *p, val *r, val **param, int n)
{
  s32 sfreq; // signed version for negative checking
  u32 freq;
  unsigned id, timer_id = 0;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  sfreq = param[1]->Val->LongInteger;
  if (sfreq < 0)
    return pmod_error("frequency must be 0 or positive");
  freq = (u32) sfreq;
  if (freq > 0) {
    timer_id = param[2]->Val->UnsignedInteger;
    MOD_CHECK_ID(timer, timer_id);
    MOD_CHECK_RES_ID(adc, id, timer, timer_id);
  }
  
  platform_adc_set_timer(id, timer_id);
  freq = platform_adc_set_clock(id, freq);
  r->Val->UnsignedLongInteger = freq;
}

// PicoC: data = adc_isdone(id);
static void adc_isdone(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  r->Val->UnsignedInteger = platform_adc_is_done(id);
}

// PicoC: adc_setblocking(id, mode);
static void adc_setblocking(pstate *p, val *r, val **param, int n)
{
  unsigned id, mode;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  mode = param[1]->Val->UnsignedInteger;
  platform_adc_set_blocking(id, mode);
}

// PicoC: adc_setsmoothing(id, length);
static void adc_setsmoothing(pstate *p, val *r, val **param, int n)
{
  unsigned id, length, res;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);

  length = param[1]->Val->UnsignedInteger;
  if (!(length & (length - 1))) {
    res = platform_adc_set_smoothing(id, length);
    if (res == PLATFORM_ERR) {
      return pmod_error("Buffer allocation failed.");
    } else {
      r->Val->UnsignedInteger = res;
      return;
    }      
  } else {
    return pmod_error("Length must be power of 2");
  }
}

// PicoC: adc_sample(id, count);
// In PicoC, you could write an array and
// use an iteration to init and setup a
// list of channels.
static void adc_sample(pstate *p, val *r, val **param, int n)
{
  unsigned id, count;
  int res;

  count = param[1]->Val->UnsignedInteger;
  if ((count == 0) || count & (count - 1))
    return pmod_error("count must be power of 2 and > 0");

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
      
  res = adc_setup_channel(id, intlog2(count));
  if (res != PLATFORM_OK)
    return pmod_error("sampling setup failed");
      
  platform_adc_start_sequence();
  r->Val->Integer = res;
}

// PicoC: val = adc_getsample(id);
static void adc_getsample(pstate *p, val *r, val **param, int n)
{
  unsigned id;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  
  // If we have at least one sample, return it
  if (adc_wait_samples(id, 1) >= 1)
    r->Val->Integer = adc_get_processed_sample(id);
}

#if defined (BUF_ENABLE_ADC)
// PicoC: adc_getsamples(id, count, arr);
// In PicoC, setup an array of integers of
// size 'count'.
static void adc_getsamples(pstate *p, val *r, val **param, int n)
{
  int id, i, *arr;
  u16 bcnt, count = 0;
  
  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);
  
  count = (u16)param[1]->Val->UnsignedInteger;
  arr = (int *)param[2]->Val->Pointer;

  bcnt = adc_wait_samples(id, count);
  
  /* If count is zero, grab all samples */
  if (count == 0)
    count = bcnt;
  
  /* Don't pull more samples than are available */
  if (count > bcnt)
    count = bcnt;
  
  for (i = 0; i < count; i++)
    arr[i] = adc_get_processed_sample(id);
}

// PicoC: adc_insertsamples(id, arr, idx, count);
// Function parameters:
// 1. id - ADC channel ID.
// 2. arr - array to write samples to. Values at arr[idx]
//    to arr[idx + count -1] will be overwritten with samples
//    (or 0 if not enough samples are available).
// 3. idx - first index to use in the array for writing
//    samples.
// 4. count - number of samples to return. If not enough
//    samples are available (after blocking, if enabled)
//    remaining values will be 0;
static void adc_insertsamples(pstate *p, val *r, val **param, int n)
{
  unsigned id, i, startidx;
  int *arr;
  u16 bcnt, count;

  id = param[0]->Val->UnsignedInteger;
  MOD_CHECK_ID(adc, id);

  arr = param[1]->Val->Pointer;

  startidx = param[2]->Val->UnsignedInteger;
  if (startidx < 0)
     return pmod_error("idx must be >= 0");

  count = param[3]->Val->UnsignedInteger;
  if (count == 0)
    return pmod_error("count must be > 0");
  
  bcnt = adc_wait_samples(id, count);

  for (i = startidx; i < (count + startidx); i++) {
    if (i < bcnt + startidx)
      arr[i] = adc_get_processed_sample(id);
    else
      // zero-out values where we don't have enough samples
      arr[i] = 0;
  }
}

#endif // #if defined (BUF_ENABLE_ADC)

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

// List of all library functions and their prototypes
const PICOC_REG_TYPE adc_library[] = {
  {FUNC(adc_maxval), PROTO("unsigned long adc_maxval(unsigned int);")},
  {FUNC(adc_setclock), PROTO("unsigned long adc_setclock(unsigned int, long, unsigned int);")},
  {FUNC(adc_isdone), PROTO("unsigned int adc_isdone(unsigned int);")},
  {FUNC(adc_setblocking), PROTO("void adc_setblocking(unsigned int, unsigned int);")},
  {FUNC(adc_setsmoothing), PROTO("unsigned int adc_setsmoothing(unsigned int, unsigned int);")},
  {FUNC(adc_sample), PROTO("int adc_sample(unsigned int, unsigned int);")},
  {FUNC(adc_getsample), PROTO("int adc_getsample(unsigned int);")},
#if defined (BUF_ENABLE_ADC)
  {FUNC(adc_getsamples), PROTO("void adc_getsamples(int, unsigned int, int *);")},
  {FUNC(adc_insertsamples), PROTO("void adc_insertsamples(unsigned int,\
                                   int *, unsigned int, unsigned int);")},
#endif
  {NILFUNC, NILPROTO}
};

// Init library.
extern void adc_library_init(void)
{
  REGISTER("adc.h", NULL, &adc_library[0]);
}

#endif // ALCOR_LANG_PICOC

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// ADC (Analog to digital converter) module for Lua.

// Lua: data = maxval( id )
static int adc_maxval( lua_State* L )
{
  unsigned id;
  u32 res;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  res = platform_adc_get_maxval( id );
  lua_pushinteger( L, res );
  return 1;
}

// Lua: realclock = setclock( id, freq, [timer_id] )
static int adc_setclock( lua_State* L )
{
  s32 sfreq; // signed version for negative checking
  u32 freq;
  unsigned id, timer_id = 0;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  sfreq = luaL_checkinteger( L, 2 );
  if ( sfreq < 0 )
    return luaL_error( L, "frequency must be 0 or positive" );
  freq = ( u32 ) sfreq;
  if ( freq > 0 )
  {
    timer_id = luaL_checkinteger( L, 3 );
    MOD_CHECK_ID( timer, timer_id );
    MOD_CHECK_RES_ID( adc, id, timer, timer_id );
  }

  platform_adc_set_timer( id, timer_id );
  freq = platform_adc_set_clock( id, freq );
  lua_pushinteger( L, freq );
  return 1;
}

// Lua: data = isdone( id )
static int adc_isdone( lua_State* L )
{
  unsigned id;
    
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  lua_pushinteger( L, platform_adc_is_done( id ) );
  return 1;
}

// Lua: setblocking( id, mode )
static int adc_setblocking( lua_State* L )
{
  unsigned id, mode;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  mode = luaL_checkinteger( L, 2 );
  platform_adc_set_blocking( id, mode );
  return 0;
}

// Lua: setsmoothing( id, length )
static int adc_setsmoothing( lua_State* L )
{
  unsigned id, length, res;

  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  
  length = luaL_checkinteger( L, 2 );
  if ( !( length & ( length - 1 ) ) )
  {
    res = platform_adc_set_smoothing( id, length );
    if ( res == PLATFORM_ERR )
      return luaL_error( L, "Buffer allocation failed." );
    else
      return 0;
  }
  else
    return luaL_error( L, "length must be power of 2" );
}

// Lua: sample( id, count )
static int adc_sample( lua_State* L )
{
  unsigned id, count, nchans = 1;
  int res, i;  
  
  count = luaL_checkinteger( L, 2 );
  if  ( ( count == 0 ) || count & ( count - 1 ) )
    return luaL_error( L, "count must be power of 2 and > 0" );
  
  // If first parameter is a table, extract channel list
  if ( lua_istable( L, 1 ) == 1 )
  {
    nchans = lua_objlen(L, 1);
    
    // Get/check list of channels and setup
    for( i = 0; i < nchans; i++ )
    {
      lua_rawgeti( L, 1, i+1 );
      id = luaL_checkinteger( L, -1 );
      MOD_CHECK_ID( adc, id );
      
      res = adc_setup_channel( id, intlog2( count ) );
      if ( res != PLATFORM_OK )
        return luaL_error( L, "sampling setup failed" );
    }
    // Initiate sampling
    platform_adc_start_sequence();
  }
  else if ( lua_isnumber( L, 1 ) == 1 )
  {
    id = luaL_checkinteger( L, 1 );
    MOD_CHECK_ID( adc, id );
    
    res = adc_setup_channel( id, intlog2( count ) );
    if ( res != PLATFORM_OK )
      return luaL_error( L, "sampling setup failed" );
    
    platform_adc_start_sequence();
  }
  else
  {
    return luaL_error( L, "invalid channel selection" );
  }
  return 0;
}


// Lua: val = getsample( id )
static int adc_getsample( lua_State* L )
{
  unsigned id;

  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  
  // If we have at least one sample, return it
  if ( adc_wait_samples( id, 1 ) >= 1 )
  {
    lua_pushinteger( L, adc_get_processed_sample( id ) );
    return 1;
  }
  return 0;
}

#if defined( BUF_ENABLE_ADC )
// Lua: table_of_vals = getsamples( id, [count] )
static int adc_getsamples( lua_State* L )
{
  unsigned id, i;
  u16 bcnt, count = 0;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );

  if ( lua_isnumber(L, 2) == 1 )
    count = ( u16 )lua_tointeger(L, 2);
  
  bcnt = adc_wait_samples( id, count );
  
  // If count is zero, grab all samples
  if ( count == 0 )
    count = bcnt;
  
  // Don't pull more samples than are available
  if ( count > bcnt )
    count = bcnt;
  
  lua_createtable( L, count, 0 );
  for( i = 1; i <= count; i ++ )
  {
    lua_pushinteger( L, adc_get_processed_sample( id ) );
    lua_rawseti( L, -2, i );
  }
  return 1;
}


// Lua: insertsamples(id, table, idx, count)
static int adc_insertsamples( lua_State* L )
{
  unsigned id, i, startidx;
  u16 bcnt, count;
  
  id = luaL_checkinteger( L, 1 );
  MOD_CHECK_ID( adc, id );
  
  luaL_checktype(L, 2, LUA_TTABLE);
  
  startidx = luaL_checkinteger( L, 3 );
  if  ( startidx <= 0 )
    return luaL_error( L, "idx must be > 0" );

  count = luaL_checkinteger(L, 4 );
  if  ( count == 0 )
    return luaL_error( L, "count must be > 0" );
  
  bcnt = adc_wait_samples( id, count );
  
  for( i = startidx; i < ( count + startidx ); i ++ )
  {
    if ( i < bcnt + startidx )
      lua_pushinteger( L, adc_get_processed_sample( id ) );
    else
      lua_pushnil( L ); // nil-out values where we don't have enough samples

    lua_rawseti( L, 2, i );
  }
  
  return 0;
}
#endif

// Module function map
#define MIN_OPT_LEVEL 2
#include "lrodefs.h"
const LUA_REG_TYPE adc_map[] = 
{
  { LSTRKEY( "sample" ), LFUNCVAL( adc_sample ) },
  { LSTRKEY( "maxval" ), LFUNCVAL( adc_maxval ) },
  { LSTRKEY( "setclock" ), LFUNCVAL( adc_setclock ) },
  { LSTRKEY( "isdone" ), LFUNCVAL( adc_isdone ) },
  { LSTRKEY( "setblocking" ), LFUNCVAL( adc_setblocking ) },
  { LSTRKEY( "setsmoothing" ), LFUNCVAL( adc_setsmoothing ) },
  { LSTRKEY( "getsample" ), LFUNCVAL( adc_getsample ) },
#if defined( BUF_ENABLE_ADC )
  { LSTRKEY( "getsamples" ), LFUNCVAL( adc_getsamples ) },
  { LSTRKEY( "insertsamples" ), LFUNCVAL( adc_insertsamples ) },
#endif
  { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_adc( lua_State *L )
{
  LREGISTER( L, AUXLIB_ADC, adc_map );
}

#endif

#endif // #ifdef ALCOR_LANG_LUA
