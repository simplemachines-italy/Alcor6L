// eLua Module for LM3S RIT OLED Display Support
// disp is a platform-dependent (LM3S) module, that binds to Lua the basic API
// from Luminary Micro
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
# include "lrotable.h"
#endif

#include "platform.h"
#include "platform_conf.h"
#include "disp.h"

#ifdef ALCOR_LANG_PICOC

// ****************************************************************************
// LM3S OLED display module for PicoC.

// PicoC: lm3s_disp_init(frequency);
static void disp_init(pstate *p, val *r, val **param, int n)
{
  unsigned long freq;
  
  freq = param[0]->Val->UnsignedLongInteger;
  lm3s_disp_init(freq);
}

// PicoC: lm3s_disp_enable(frequency);
static void disp_enable(pstate *p, val *r, val **param, int n)
{
  unsigned long freq;
 
  freq = param[0]->Val->UnsignedLongInteger;
  lm3s_disp_enable(freq);
}

// PicoC: lm3s_disp_disable(void);
static void disp_disable(pstate *p, val *r, val **param, int n)
{
  lm3s_disp_disable();
}

// PicoC: lm3s_disp_on(void);
static void disp_on(pstate *p, val *r, val **param, int n)
{
  lm3s_disp_displayOn();    
}

// PicoC: lm3s_disp_off(void);
static void disp_off(pstate *p, val *r, val **param, int n)
{
  lm3s_disp_displayOff();
}
   
// PicoC: lm3s_disp_clear(void);
static void disp_clear(pstate *p, val *r, val **param, int n)
{
  lm3s_disp_clear();
}
   
// PicoC: lm3s_disp_print(str, x, y, level);
static void disp_print(pstate *p, val *r, val **param, int n)
{
  const char *str;
  unsigned long x, y; 
  unsigned char level;
 
  str = param[0]->Val->Identifier;
  x = param[1]->Val->UnsignedLongInteger;
  y = param[2]->Val->UnsignedLongInteger;
  level = param[3]->Val->Character;

  lm3s_disp_stringDraw(str, x, y, level);
}

// PicoC: lm3s_disp_draw(img, x, y, width, height);
static void disp_image_draw(pstate *p, val *r, val **param, int n)
{
  const char *img;
  unsigned long x, y, width, height;

  img = param[0]->Val->Identifier;
  x = param[1]->Val->UnsignedLongInteger;
  y = param[2]->Val->UnsignedLongInteger;
  width = param[3]->Val->UnsignedLongInteger;
  height = param[4]->Val->UnsignedLongInteger;
  lm3s_disp_imageDraw((const unsigned char *)img, x, y, width, height);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

const PICOC_REG_TYPE lm3s_disp_library[] = {
  {FUNC(disp_init), PROTO("void lm3s_disp_init(unsigned long);")},
  {FUNC(disp_enable), PROTO("void lm3s_disp_enable(unsigned long);")},
  {FUNC(disp_disable), PROTO("void lm3s_disp_disable(void);")},
  {FUNC(disp_on), PROTO("void lm3s_disp_on(void);")},
  {FUNC(disp_off), PROTO("void lm3s_disp_off(void);")},
  {FUNC(disp_clear), PROTO("void lm3s_disp_clear(void);")},
  {FUNC(disp_print), PROTO("void lm3s_disp_print(char *, unsigned long,"
			   "unsigned long, char);")},
  {FUNC(disp_image_draw), PROTO("void lm3s_disp_draw(char *, unsigned long,"
				"unsigned long, unsigned long, char);")},
  {NILFUNC, NILPROTO}
};

/* init library */
extern void lm3s_disp_library_init(void)
{
  REGISTER("disp.h", NULL, &lm3s_disp_library[0]);
}

#else

// ****************************************************************************
// LM3S OLED display module for Lua.

//Lua: init(frequency)
static int disp_init(lua_State *L) {
  unsigned long freq;
  
  freq = luaL_checkinteger(L, 1);
  lm3s_disp_init(freq);
  return 0;
   
}

//Lua: enable(frequency)
static int disp_enable(lua_State *L) {
  unsigned long freq;
  
  freq = luaL_checkinteger(L, 1);
  lm3s_disp_enable(freq);
  return 0;
}

//Lua: disable()   
static int disp_disable(lua_State *L) {    
  lm3s_disp_disable();
  return 0; 
}
   
//Lua: on()
static int disp_on(lua_State *L) {
  lm3s_disp_displayOn();    
  return 0; 
}

//Lua: off()
static int disp_off(lua_State *L) {    
  lm3s_disp_displayOff();
  return 0; 
}
   
//Lua: clear()
static int disp_clear(lua_State *L) {    
  lm3s_disp_clear();
  return 0; 
}
   
//Lua: strDraw(str, x, y, lvl)
static int disp_stringDraw(lua_State *L) {
  const char *str;
  unsigned long x; 
  unsigned long y;
  unsigned char lvl;
  
  str   = luaL_checkstring(L, 1);         
  x     = luaL_checkinteger(L, 2);
  y     = luaL_checkinteger(L, 3);
  lvl   = (unsigned char) luaL_checkinteger(L, 4);    
  lm3s_disp_stringDraw(str, x, y, lvl);
  return 0; 
}
   
// Lua: imageDraw(img, x, y, width, height)
static int disp_imageDraw(lua_State *L) {    
  const char *img;
  unsigned long x;
  unsigned long y;
  unsigned long width;
  unsigned long height;
  
  img    = luaL_checkstring(L, 1);
  x      = luaL_checkinteger(L, 2);
  y      = luaL_checkinteger(L, 3);
  width  = luaL_checkinteger(L, 4);
  height = luaL_checkinteger(L, 5);
  lm3s_disp_imageDraw(( const unsigned char* )img, x, y, width, height);
  return 0; 
}   



#define MIN_OPT_LEVEL 2
#include "lrodefs.h"  

// Module function map
const LUA_REG_TYPE disp_map[] =
{ 
  { LSTRKEY( "init" ),  LFUNCVAL( disp_init ) },
  { LSTRKEY( "enable" ),  LFUNCVAL( disp_enable ) },
  { LSTRKEY( "disable" ), LFUNCVAL( disp_disable ) },
  { LSTRKEY( "on" ), LFUNCVAL( disp_on ) },    
  { LSTRKEY( "off" ), LFUNCVAL( disp_off ) },
  { LSTRKEY( "clear" ), LFUNCVAL( disp_clear ) },
  { LSTRKEY( "print" ), LFUNCVAL( disp_stringDraw ) },
  { LSTRKEY( "draw" ), LFUNCVAL( disp_imageDraw ) },  
  { LNILKEY, LNILVAL }
};

#endif
