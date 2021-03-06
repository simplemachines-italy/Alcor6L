// eLua module for Mizar32 LCD character display
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
# include "rotable.h"
#endif

#if defined ALCOR_LANG_LUA
# include "lua.h"
# include "lualib.h"
# include "lauxlib.h"
# include "lrotable.h"
#endif

// Generic includes.
#include "platform.h"
#include "platform_conf.h"
#include "lcd.h"       
#include "i2c.h"

// The LCD firmware only runs at up to 50kHz on the I2C bus, so
// we bracket all I2C packets to the LCD module with two functions
// to be able to save, change and restore the I2C clock rate to what it was
// before.

// Declarations to save/restore the I2C clock rate
extern u32 i2c_delay;
static u32 old_i2c_delay;

static void lcd_start()
{
  old_i2c_delay = i2c_delay;
  i2c_delay = REQ_CPU_FREQ / LCD_BUS_FREQ / 2;
}

static void lcd_stop()
{
  i2c_delay = old_i2c_delay;
}

// Low-level functions to send LCD commands or data.
// The command and data packets differ only in the slave address used,
// so we coalesce them into a single function, generating smaller code.
//
// All three are designed to be used in a tail call:
//   return send_generic( data, len );

// Send a command or data packet.
// "address" is LCD_CMD for LCD commands, LCD_DATA for LCD data.
static int send_generic( u8 address, const u8 *data, int len )
{
  lcd_start();
  i2c_send( address, data, len, true );
  lcd_stop();
  return 0;
}

// Send an I2C read-data command and return the answer.
// "address" is LCD_GETPOS to read the cursor position,
//              LCD_BUTTONS for to read the buttons.
// The answer is always a single byte.
// Returns the number of bytes read (== 1) or 0 f the slave did not
// acknowledge its address.
static int recv_generic( u8 address )
{
  u8 retval;

  lcd_start();
  if( i2c_recv( address, &retval, 1, true ) < 0 ) {
    // The address was not acknowledged, so no slave is present.
    // There is no way to signal this to the Lua layer, so return a
    // harmless value (meaning no buttons pressed or cursor at (1,1)).
    retval = 0;
  }
  lcd_stop();

  return retval;
}

// Send a command byte
static int send_command( const u8 command )
{
  return send_generic( LCD_CMD, &command, 1 );
}

// Send data bytes
// This is used for printing data and for programming the user-defining chars
static int send_data( const u8 *data, int len )
{
  return send_generic( LCD_DATA, data, len );
}

// Return the current value of the address counter.
static u8 recv_address_counter()
{
  return recv_generic( LCD_GETPOS );
}

// Return the current state of the buttons, a bit mask in the bottom 5 bits
// of a byte.
static u8 recv_buttons()
{
  return recv_generic( LCD_BUTTONS );
}

// Turning the display on can only be achieved by simultaneously specifying the
// cursor type, so we have to remember what type of cursor they last set.
// Similarly, if they have turned the display off then set the cursor, this
// shouldn't turn the display on.

// Power-on setting is no cursor
#define DEFAULT_CURSOR_TYPE   LCD_CMD_CURSOR_NONE

static u8 cursor_type = DEFAULT_CURSOR_TYPE;
static u8 display_is_off = 0;     // Have they called display("off")?

// Helper function to set a cursor type if the display is on,
// or to remember which cursor they asked for, to be able to set it
// when they turn the display on.
static int set_cursor( u8 command_byte )
{
  cursor_type = command_byte;

  // Setting cursor type always turns the display on
  if ( display_is_off )
    return 0;
  else
    return send_command( cursor_type );
}

#if defined ALCOR_LANG_PICOLISP

// ****************************************************************************
// LCD display module for picoLisp.

// (mizar32-lcd-reset) -> Nil
any plisp_lcd_reset(any ex) {
  cursor_type = DEFAULT_CURSOR_TYPE;
  display_is_off = 0;
  
  send_command(LCD_CMD_RESET);
  return Nil;
}

// (mizar32-lcd-setup shift-disp r-to-l) -> Nil
any plisp_lcd_setup(any ex) {
  any x, y;
  unsigned shift_disp, r_to_l;

  x = cdr(ex);
  NeedSym(ex, y = EVAL(car(x)));
  shift_disp = (y == T) ? 1 : 0;

  x = cdr(x);
  NeedSym(ex, y = EVAL(car(x)));
  r_to_l = (y == T) ? 1 : 0;

  send_command(LCD_CMD_ENTRYMODE +
	       shift_disp +
	       (!r_to_l) * 2);
  return Nil;
}

// (mizar32-lcd-clear) -> Nil
// Clear the display, reset its shiftedness and put the cursor at 1,1
any plisp_lcd_clear(any ex) {
  send_command(LCD_CMD_CLEAR);
  return Nil;
}

// (mizar32-lcd-home) -> Nil
// Reset the display's shiftedness and put the cursor at 1,1
any plisp_lcd_home(any ex) {
  send_command(LCD_CMD_HOME);
  return Nil;
}

// (mizar32-lcd-goto row col) -> Nil
// Move the cursor to the specified row (1 or 2) and
// column (1-40) in the character memory.
any plisp_lcd_goto(any ex) {
  any x, y;
  unsigned row, col, address;
  
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  row = (unsigned)unBox(y);
  
  x = cdr(x);
  NeedNum(ex, y = EVAL(car(x)));
  col = (unsigned)unBox(y);
  
  if (row < 1 || row > 2 || col < 1 || col > 40)
    err(NULL, ex, "row/column must be 1-2 and 1-40");
  
  address = (row - 1) * 0x40 + (col - 1);
  send_command((u8) (LCD_CMD_DDADDR + address));
    
  return Nil;
}

/***
static void outString_lcd(char *s) {
  while (*s)
    send_data((const u8 *)s++, 1);
}

static void outNum_lcd(long n) {
  char buf[BITS/2];

  bufNum(buf, n);
  outString_lcd(buf);
} ***/

// Helpers for picoLisp LCD print function.
static void plisp_lcdh_prin(any x) {
  if (!isNil(x)) {
    if (isNum(x)) {
      u8 byte = (u8)unBox(x);
      send_data(&byte, 1);
      // outNum_lcd(unBox(x));
    }
    else if (isSym(x)) {
      int i, c;
      word w;
      u8 byte;

      for (x = name(x), c = getByte1(&i, &w, &x); c; c = getByte(&i, &w, &x)) {
	if (c != '^') {
	  byte = c;
	  send_data(&byte, 1);
	}
	else if (!(c = getByte(&i, &w, &x))) {
	  byte = '^';
	  send_data(&byte, 1);
	} 
	else if (c == '?') {
	  byte = 127;
	  send_data(&byte, 1);
	}
	else {
	  c &= 0x1F;
	  byte = (u8)c;
	  send_data(&byte, 1);
	}
      }
    }
    else {
      while (plisp_lcdh_prin(car(x)), !isNil(x = cdr(x))) {
	if (!isCell(x)) {
	  plisp_lcdh_prin(x);
	  break;
	}
      }
    }
  }
}

// (mizar32-lcd-prinl 'any ..) -> any
any plisp_lcd_prinl(any x) {
  any y = Nil;

  while (isCell(x = cdr(x)))
    plisp_lcdh_prin(y = EVAL(car(x)));
  return y;
}

// (mizar32-lcd-getpos) -> lst
any plisp_lcd_getpos(any x) {
  u8 addr = recv_address_counter();
  any y;
  cell c1;

  Push(c1, y = cons(box(((addr & 0x40) ? 2 : 1)), Nil));
  Push(c1, y = cons(box(((addr & 0x3F) + 1)), y));

  return Pop(c1);
}

// (mizar32-lcd-buttons) -> sym
any plisp_lcd_buttons(any x) {
  u8 code;                // bit code for buttons held
  char string[6];         // Up to 5 buttons and a \0
  char *stringp = string; // Where to write the next character;
  
  code = recv_buttons();
  if (code & LCD_BUTTON_SELECT) *stringp++ = 'S';
  if (code & LCD_BUTTON_LEFT) *stringp++ = 'L';
  if (code & LCD_BUTTON_RIGHT) *stringp++ = 'R';
  if (code & LCD_BUTTON_UP) *stringp++ = 'U';
  if (code & LCD_BUTTON_DOWN) *stringp++ = 'D';
  *stringp = '\0';

  return mkStr(string);
}

// Perform cursor operations selected by a
// string (a transient symbol in picoLisp)
// parameter.
//
// (mizar32-lcd-cursor 'sym) -> Nil
any plisp_lcd_cursor(any x) {
  static const char const *args[] = {
    "none",
    "block",
    "line",
    "left",
    "right",
    NULL
  };
  
  any y;
  x = cdr(x), y = EVAL(car(x));
  
  if (equal(mkStr(args[0]), y))
    set_cursor(LCD_CMD_CURSOR_NONE);
  else if (equal(mkStr(args[1]), y))
    set_cursor(LCD_CMD_CURSOR_BLOCK);
  else if (equal(mkStr(args[2]), y))
    set_cursor(LCD_CMD_CURSOR_LINE);
  else if (equal(mkStr(args[3]), y))
    set_cursor(LCD_CMD_SHIFT_CURSOR_LEFT);
  else if (equal(mkStr(args[4]), y))
    set_cursor(LCD_CMD_SHIFT_CURSOR_RIGHT);
  else
    err(NULL, y, "invalid cursor argument");

  return Nil;
}

// Perform display operations,
// selected by a string parameter.
//
// (mizar32-lcd-display 'sym) -> Nil
any plisp_lcd_display(any x) {
  static const char const *args[] = {
    "off",
    "on",
    "left",
    "right",
    NULL
  };

  any y;
  x = cdr(x), y = EVAL(car(x));

  if (equal(mkStr(args[0]), y)) {
    display_is_off = 1;
    send_command(LCD_CMD_DISPLAY_OFF);
  }
  else if (equal(mkStr(args[1]), y)) {
    display_is_off = 0;
    send_command(cursor_type);
  }
  else if (equal(mkStr(args[2]), y))
    send_command(LCD_CMD_SHIFT_DISPLAY_LEFT);
  else if (equal(mkStr(args[3]), y))
    send_command(LCD_CMD_SHIFT_DISPLAY_RIGHT);
  else
    err(NULL, x, "invalid display argument");

  return Nil;
}

// (mizar32-lcd-definechar 'num 'lst) -> Nil
// code: 0-7
// glyph: a list of up to 8 numbers with values 0-31.
//        If less than 8 are supplied, the bottom rows are blanked.
//        If more than 8 are supplied, the extra are ignored.
// The current cursor position in the character display RAM is preserved.
any plisp_lcd_definechar(any ex) {
  int code;        // The character code we are defining, 0-7
  size_t datalen;  // The number of elements in the glyph table
  size_t line = 0; // Which line of the char are we defining?
  u8 data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int old_address; // The coded value for the current cursor position
  any x, y;

  // First parameter: glyph code to define.
  x = cdr(ex);
  NeedNum(ex, y = EVAL(car(x)));
  code = unBox(y);
  if (code < 0 || code > 7)
    err(ex, y, "user-defined characters have codes 0-7");

  // Second parameter: list of integer values to define the glyph
  x = cdr(x);
  NeedLst(ex, y = EVAL(car(x)));
  datalen = length(y);

  // Check all parameters before starting the I2C command.
  if (datalen >= 8) datalen = 8; // Ignore extra parameters
  for (line = 0; line < datalen; line++) {
    NeedNum(y, car(y));
    data[line] = unBox(car(y));
    y = EVAL(cdr(y));
  }

  old_address = recv_address_counter();
  send_command(LCD_CMD_CGADDR + code * 8);
  send_data(data, sizeof(data));

  // Move back to where we were
  send_command(LCD_CMD_DDADDR + old_address);
  return Nil;
}

#endif // ALCOR_LANG_PICOLISP

#if defined ALCOR_LANG_PICOC

// ****************************************************************************
// LCD display module for PicoC.

// picoc: mizar32_lcd_reset();
static void lcd_reset(pstate *p, val *r, val **param, int n)
{
  cursor_type = DEFAULT_CURSOR_TYPE;
  display_is_off = 0;

  send_command(LCD_CMD_RESET);
}

// PicoC: mizar32_lcd_setup(shift_display, right-to-left);
// Set right-to-left mode,
static void lcd_setup(pstate *p, val *r, val **param, int n)
{
  unsigned shift_display = param[0]->Val->UnsignedInteger;
  unsigned right_to_left = param[0]->Val->UnsignedInteger;

  send_command(LCD_CMD_ENTRYMODE + shift_display +
	       (! right_to_left) * 2);
}

// PicoC: mizar32_lcd_clear();
// Clear the display, reset its shiftedness and put the cursor at 1,1
static void lcd_clear(pstate *p, val *r, val **param, int n)
{
  send_command(LCD_CMD_CLEAR);
}

// PicoC: mizar32_lcd_home();
// Reset the display's shiftedness and put the cursor at 1,1
static void lcd_home(pstate *p, val *r, val **param, int n)
{
  send_command(LCD_CMD_HOME);
}

// PicoC: mizar32_lcd_goto(row, col);
// Move the cursor to the specified row (1 or 2) and
// column (1-40) in the character memory.
static void lcd_goto(pstate *p, val *r, val **param, int n)
{
  int row = param[0]->Val->UnsignedInteger;
  int col = param[1]->Val->UnsignedInteger;
  unsigned address;

  if (row < 1 || row > 2 || col < 1 || col > 40)
    return pmod_error("row/column must be 1-2 and 1-40");

  address = (row - 1) * 0x40 + (col - 1) ;
  send_command((u8) (LCD_CMD_DDADDR + address));
}

// We split lcd_print function (for Lua) into two
// seperate function for PicoC - one for printing
// strings and the other for printing integers.


// PicoC: mizar32_lcd_print_str(str);
// TODO: Convert this into a variadic
// function.
static void lcd_print_string(pstate *p, val *r, val **param, int n)
{
  const char *s = param[0]->Val->Identifier;
  size_t len = strlen(s);

  send_data((u8 *) s, len);
  r->Val->Integer = len;
}

// PicoC: mizar32_lcd_print_int(i);
// TODO: Convert this into a variadic
// function.
static void lcd_print_integer(pstate *p, val *r, val **param, int n)
{
  u8 byte = param[0]->Val->Character;

  send_data(&byte, 1);
  r->Val->Integer = 1;
}

// For some info, check the Lua version of the
// same function.
// PicoC: mizar32_lcd_getpos(&row, &col);
static void lcd_getpos(pstate *p, val *r, val **param, int n)
{
  u8 addr = recv_address_counter();

  r->Val->Integer = 0;
  *((int *)param[0]->Val->UnsignedInteger) = ((addr & 0x40) ? 2 : 1);
  *((int *)param[1]->Val->UnsignedInteger) = ((addr & 0x3F) + 1);
}

// Return the current state of the pressed buttons as a
// string containing a selection of the letters S, L, R,
// U, D or an empty string if none are currently held
// down.

// PicoC: mizar32_lcd_buttons();
static void lcd_buttons(pstate *p, val *r, val **param , int n)
{
  u8 code;                // bit code for buttons held
  char string[6];         // Up to 5 buttons and a \0
  char *stringp = string; // Where to write the next character;

  code = recv_buttons();
  if(code & LCD_BUTTON_SELECT) *stringp++ = 'S';
  if(code & LCD_BUTTON_LEFT) *stringp++ = 'L';
  if(code & LCD_BUTTON_RIGHT) *stringp++ = 'R';
  if(code & LCD_BUTTON_UP) *stringp++ = 'U';
  if(code & LCD_BUTTON_DOWN) *stringp++ = 'D';
  *stringp = '\0';

  r->Val->Identifier = string;
}

// cursor and command specific values.
const int lcd_cur_none = LCD_CMD_CURSOR_NONE;
const int lcd_cur_block = LCD_CMD_CURSOR_BLOCK;
const int lcd_cur_line = LCD_CMD_CURSOR_LINE;
const int lcd_cur_left = LCD_CMD_SHIFT_CURSOR_LEFT;
const int lcd_cur_right = LCD_CMD_SHIFT_CURSOR_RIGHT;
const int lcd_cmd_off = LCD_CMD_DISPLAY_OFF;
const int lcd_cmd_on = 1;
const int lcd_cmd_lshift = LCD_CMD_SHIFT_DISPLAY_LEFT;
const int lcd_cmd_rshift = LCD_CMD_SHIFT_DISPLAY_RIGHT;

// "Display on/off control" functions
static void lcd_cursor(pstate *p, val *r, val **param, int n)
{
  switch (param[0]->Val->Integer) {
  case LCD_CMD_CURSOR_NONE:
    set_cursor(LCD_CMD_CURSOR_NONE);
    break;
  case LCD_CMD_CURSOR_BLOCK:
    set_cursor(LCD_CMD_CURSOR_BLOCK);
    break;
  case LCD_CMD_CURSOR_LINE:
    set_cursor(LCD_CMD_CURSOR_LINE);
    break;
  case LCD_CMD_SHIFT_CURSOR_LEFT:
    send_command(LCD_CMD_SHIFT_CURSOR_LEFT);
    break;
  case LCD_CMD_SHIFT_CURSOR_RIGHT:
    send_command(LCD_CMD_SHIFT_CURSOR_RIGHT);
    break;
  default:
    return pmod_error("invslid LCD cursor command.");
  }
}

// Perform display operations, selected by an integer parameter.
static void lcd_display(pstate *p, val *r, val **param, int n)
{
  switch (param[0]->Val->Integer) {
  case LCD_CMD_DISPLAY_OFF:
    display_is_off = 1;
    send_command(LCD_CMD_DISPLAY_OFF);
    break;
  case 1:
    display_is_off = 0;
    send_command(cursor_type);
    break;
  case LCD_CMD_SHIFT_DISPLAY_LEFT:
    send_command(LCD_CMD_SHIFT_DISPLAY_LEFT);
    break;
  case LCD_CMD_SHIFT_DISPLAY_RIGHT:
    send_command(LCD_CMD_SHIFT_DISPLAY_RIGHT);
    break;
  default:
    return pmod_error("invalid LCD display command.");
  }
}

// PicoC: mizar32_lcd_definechar(code, arr, len_arr);
static void lcd_definechar(pstate *p, val *r, val **param, int n)
{
  int code;        // The character code we are defining, 0-7
  size_t datalen;  // The number of elements in the glyph table
  size_t line;     // Which line of the char are we defining?
  int old_address; // The coded value for the current cursor position
  u8 data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int *ptr;

  // First parameter: glyph code to define
  code = param[0]->Val->Integer;
  if (code < 0 || code > 7)
    return pmod_error("user-defined characters have codes 0-7");

  // Second parameter: table of integer values to define the glyph
  ptr = param[1]->Val->Pointer;
  datalen = param[2]->Val->Integer;

  // Check all parameters before starting the I2C command.
  if (datalen >= 8) datalen = 8;            // Ignore extra parameters 
  for (line = 0; line < datalen; line++)
    data[line] = ptr[line];

  old_address = recv_address_counter();
  send_command(LCD_CMD_CGADDR + code * 8);
  send_data(data, sizeof(data));

  // Move back to where we were
  send_command(LCD_CMD_DDADDR + old_address);
}

#define MIN_OPT_LEVEL 2
#include "rodefs.h"

#if PICOC_TINYRAM_ON
const PICOC_RO_TYPE lcd_variables[] = {
  {STRKEY("lcd_CUR_NONE"), INT(lcd_cur_none)},
  {STRKEY("lcd_CUR_BLOCK"), INT(lcd_cur_block)},
  {STRKEY("lcd_CUR_LINE"), INT(lcd_cur_line)},
  {STRKEY("lcd_CUR_LEFT"), INT(lcd_cur_left)},
  {STRKEY("lcd_CUR_RIGHT"), INT(lcd_cur_right)},
  {STRKEY("lcd_CMD_OFF"), INT(lcd_cmd_off)},
  {STRKEY("lcd_CMD_ON"), INT(lcd_cmd_on)},
  {STRKEY("lcd_CMD_LSHIFT"), INT(lcd_cmd_lshift)},
  {STRKEY("lcd_CMD_RSHIFT"), INT(lcd_cmd_rshift)},
  {NILKEY, NILVAL}
};
#endif

const PICOC_REG_TYPE lcd_disp_library[] = {
  {FUNC(lcd_reset), PROTO("void mizar32_lcd_reset(void);")},
  {FUNC(lcd_setup), PROTO("void mizar32_lcd_setup(unsigned int, unsigned int);")},
  {FUNC(lcd_clear), PROTO("void mizar32_lcd_clear(void);")},
  {FUNC(lcd_home), PROTO("void mizar32_lcd_home(void);")},
  {FUNC(lcd_goto), PROTO("void mizar32_lcd_goto(unsigned int, unsigned int);")},
  {FUNC(lcd_print_string), PROTO("int mizar32_lcd_print_str(char *);")},
  {FUNC(lcd_print_integer), PROTO("int mizar32_lcd_print_char(int);")},
  {FUNC(lcd_getpos), PROTO("int mizar32_lcd_getpos(int *, int *);")},
  {FUNC(lcd_buttons), PROTO("char* mizar32_lcd_buttons(void);")},
  {FUNC(lcd_cursor), PROTO("void mizar32_lcd_cursor(int);")},
  {FUNC(lcd_display), PROTO("void mizar32_lcd_display(int);")},
  {FUNC(lcd_definechar), PROTO("void mizar32_lcd_definechar(int, int *, int);")},
  {NILFUNC, NILPROTO}
};

// Library setup function.
extern void lcd_lib_setup_func(void)
{
#if PICOC_TINYRAM_OFF
  picoc_def_integer("lcd_CUR_NONE", lcd_cur_none);
  picoc_def_integer("lcd_CUR_BLOCK", lcd_cur_block);
  picoc_def_integer("lcd_CUR_LINE", lcd_cur_line);
  picoc_def_integer("lcd_CUR_LEFT", lcd_cur_left);
  picoc_def_integer("lcd_CUR_RIGHT", lcd_cur_right);
  picoc_def_integer("lcd_CMD_OFF", lcd_cmd_off);
  picoc_def_integer("lcd_CMD_ON", lcd_cmd_on);
  picoc_def_integer("lcd_CMD_LSHIFT", lcd_cmd_lshift);
  picoc_def_integer("lcd_CMD_RSHIFT", lcd_cmd_rshift);
#endif
}

// Init library.
extern void lcd_disp_library_init(void)
{
  REGISTER("lcd.h", lcd_lib_setup_func,
	   &lcd_disp_library[0]);
}

#endif

#if defined ALCOR_LANG_LUA

// ****************************************************************************
// LCD display module for Lua.

// Lua: mizar32.disp.reset()
// Ensure the display is in a known initial state
static int lcd_reset( lua_State *L )
{
  // Set the static variables
  cursor_type = DEFAULT_CURSOR_TYPE;
  display_is_off = 0;

  send_command( LCD_CMD_RESET );
  return 0;
}

// "Entry mode" function.

// Lua: mizar32.disp.setup( shift_display, right-to-left )
// Set right-to-left mode,
static int lcd_setup( lua_State *L )
{
  // lua_toboolean returns 0 or 1, and returns 0 if the parameter is absent
  unsigned shift_display = lua_toboolean( L, 1 );  // Default: move cursor
  unsigned right_to_left = lua_toboolean( L, 2 );  // Default: print left-to-right

  send_command( LCD_CMD_ENTRYMODE + shift_display +
                ( ! right_to_left ) * 2 );
  return 0;
}

// Lua: mizar32.disp.clear()
// Clear the display, reset its shiftedness and put the cursor at 1,1
static int lcd_clear( lua_State *L )
{
  send_command( LCD_CMD_CLEAR );
  return 0;
}

// Lua: mizar32.disp.home()
// Reset the display's shiftedness and put the cursor at 1,1
static int lcd_home( lua_State *L )
{
  send_command( LCD_CMD_HOME );
  return 0;
}

// Lua: mizar32.disp.goto( row, col )
// Move the cursor to the specified row (1 or 2) and column (1-40)
// in the character memory.
static int lcd_goto( lua_State *L )
{
  int row = luaL_checkinteger( L, 1 );
  int col = luaL_checkinteger( L, 2 );
  unsigned address;

  if ( row < 1 || row > 2 || col < 1 || col > 40 )
    return luaL_error( L, "row/column must be 1-2 and 1-40" );

  address = ( row - 1 ) * 0x40 + ( col - 1 ) ;
  send_command( (u8) (LCD_CMD_DDADDR + address) );
  return 0;
}

// Lua: mizar32.disp.print( string )
// Send data bytes to the LCD module.
// Usually this will be a string of text or a list of character codes.
// If they pass us integer values <0 or >255, we just use the bottom 8 bits.

static int lcd_print( lua_State *L )
{
  unsigned argc = lua_gettop( L );  // Number of parameters supplied
  int argn;
  
  for ( argn = 1; argn <= argc; argn ++ )
  {
    switch ( lua_type( L, argn ) )
    {
      case LUA_TNUMBER:
      {
        u8 byte = luaL_checkint( L, argn );
        send_data( &byte, 1 );
      }
      break;
 
      case LUA_TSTRING:
      {
        size_t len;  // Number of chars in string
        const char *str = luaL_checklstring( L, argn, &len );
        send_data( (u8 *) str, len );
      }
      break;

      default:
        return luaL_typerror( L, argn, "integer or string" );
    }
  }
  return 0;
}

// Return the cursor position as row and column in the ranges 1-2 and 1-40
// The bottom 7-bits of addr are the contents of the address counter.
// The Ampire datasheet says:
//   0x00-0x0F for the first line of DDRAM (presumably 0..39 really),
//   0x40-0x4F for the second line of DDRAM (presumably 64..(64+39) really)
// The top bit (128) is the "Busy Flag", which should always be 0.

static int lcd_getpos( lua_State *L )
{
   u8 addr = recv_address_counter();
   lua_pushinteger( L, (lua_Integer) ( (addr & 0x40) ? 2 : 1 ) );  // row
   lua_pushinteger( L, (lua_Integer) ( (addr & 0x3F) + 1 ) );      // column
   return 2;
}

// Return the current state of the pressed buttons as a string containing
// a selection of the letters S, L, R, U, D or an empty string if none are
// currently held down.

static int lcd_buttons( lua_State *L )
{
   u8 code;           // bit code for buttons held
   char string[6];         // Up to 5 buttons and a \0
   char *stringp = string; // Where to write the next character;

   code = recv_buttons();
   if( code & LCD_BUTTON_SELECT ) *stringp++ = 'S';
   if( code & LCD_BUTTON_LEFT   ) *stringp++ = 'L';
   if( code & LCD_BUTTON_RIGHT  ) *stringp++ = 'R';
   if( code & LCD_BUTTON_UP     ) *stringp++ = 'U';
   if( code & LCD_BUTTON_DOWN   ) *stringp++ = 'D';
   *stringp = '\0';

   lua_pushstring( L, string );

   return 1;
}

// "Display on/off control" functions

// Perform cursor operations, selected by a string parameter,
// as recommended in the Lua Reference Manual, p.58: "luaL_checkoption()"
static int lcd_cursor( lua_State *L )
{
  static const char const *args[] =
    { "none", "block", "line", "left", "right", NULL };

  switch ( luaL_checkoption( L, 1, NULL, args ) )
  {
  case 0:
    set_cursor( LCD_CMD_CURSOR_NONE );
    break;
  case 1:
    set_cursor( LCD_CMD_CURSOR_BLOCK );
    break;
  case 2:
    set_cursor( LCD_CMD_CURSOR_LINE );
    break;

  case 3: 
    send_command( LCD_CMD_SHIFT_CURSOR_LEFT );
    break;
  case 4:
    send_command( LCD_CMD_SHIFT_CURSOR_RIGHT );
    break;

  default:
    return luaL_argerror( L, 1, NULL );
  }
  return 0;
}

// Perform display operations, selected by a string parameter.
static int lcd_display( lua_State *L )
{
  static const char const *args[] =
    { "off", "on", "left", "right", NULL };

  switch ( luaL_checkoption( L, 1, NULL, args ) )
  {
  case 0: display_is_off = 1;
	  send_command( LCD_CMD_DISPLAY_OFF );
          break;
  case 1: display_is_off = 0;
          send_command( cursor_type );	// Turns display on
          break;
  case 2: send_command( LCD_CMD_SHIFT_DISPLAY_LEFT );
          break;
  case 3: send_command( LCD_CMD_SHIFT_DISPLAY_RIGHT );
          break;
  default: return luaL_argerror( L, 1, NULL );
  }
  return 0;
}

// Lua: mizar32.disp.definechar( code, glyph )
// code: 0-7
// glyph: a table of up to 8 numbers with values 0-31.
//        If less than 8 are supplied, the bottom rows are blanked.
//        If more than 8 are supplied, the extra are ignored.
// The current cursor position in the character display RAM is preserved.

static int lcd_definechar( lua_State *L ) {
  int code;        // The character code we are defining, 0-7
  size_t datalen;  // The number of elements in the glyph table
  size_t line;     // Which line of the char are we defining?
  u8 data[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  int old_address; // The coded value for the current cursor position

  // First parameter: glyph code to define
  code = luaL_checkint( L, 1 );
  if( code < 0 || code > 7 )
    return luaL_error( L, "user-defined characters have codes 0-7" );

  // Second parameter: table of integer values to define the glyph
  luaL_checktype( L, 2, LUA_TTABLE );
  datalen = lua_objlen( L, 2 );
  // Check all parameters before starting the I2C command.
  if( datalen >= 8 ) datalen = 8;            // Ignore extra parameters
  for( line = 0; line < datalen; line ++ )
  {
    int value;
    lua_rawgeti( L, 2, line + 1 );
    value = luaL_checkint( L, -1 );
    lua_pop( L, 1 );
    data[line] = value;
  }

  old_address = recv_address_counter();

  send_command( LCD_CMD_CGADDR + code * 8 );
  send_data( data, sizeof( data ) );

  // Move back to where we were
  send_command( LCD_CMD_DDADDR + old_address );

  return 0;
}

#define MIN_OPT_LEVEL 2
#include "lrodefs.h"

// mizar32.disp.*() module function map
const LUA_REG_TYPE lcd_map[] =
{
  { LSTRKEY( "reset" ),      LFUNCVAL( lcd_reset ) },
  { LSTRKEY( "setup" ),      LFUNCVAL( lcd_setup ) },
  { LSTRKEY( "clear" ),      LFUNCVAL( lcd_clear ) },
  { LSTRKEY( "home" ),       LFUNCVAL( lcd_home ) },
  { LSTRKEY( "goto" ),       LFUNCVAL( lcd_goto ) },
  { LSTRKEY( "print" ),      LFUNCVAL( lcd_print ) },
  { LSTRKEY( "definechar" ), LFUNCVAL( lcd_definechar ) },
  { LSTRKEY( "cursor" ),     LFUNCVAL( lcd_cursor ) },
  { LSTRKEY( "display" ),    LFUNCVAL( lcd_display ) },
  { LSTRKEY( "getpos" ),     LFUNCVAL( lcd_getpos ) } ,
  { LSTRKEY( "buttons" ),    LFUNCVAL( lcd_buttons ) } ,
  { LNILKEY, LNILVAL }
};

#endif // #ifdef ALCOR_LANG_LUA
