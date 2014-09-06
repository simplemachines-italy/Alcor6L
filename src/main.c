
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "type.h"
#include "devman.h"
#include "platform.h"
#include "romfs.h"
#include "xmodem.h"
#include "shell.h"

// Language specific includes.
#if defined ALCOR_LANG_TINYSCHEME
# include "scheme.h"
#endif

#if defined ALCOR_LANG_PICOLISP
# include "pico.h"
#endif

#if defined ALCOR_LANG_PICOC
# include "picoc.h"
#endif

#if defined ALCOR_LANG_LUA
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
#endif

// Generic includes.
#include "term.h"
#include "platform_conf.h"
#include "elua_rfs.h"

#ifdef ELUA_SIMULATOR
#include "hostif.h"
#endif

// Validate configuratin options
#include "validate.h"

#include "mmcfs.h"
#include "romfs.h"
#include "semifs.h"

// Define here your autorun/boot files,
// in the order you want eLua to search for them
char *boot_order[] = {
#if defined BUILD_MMCFS
#if defined ALCOR_LANG_TINYSCHEME
  "/mmc/autorun.scm",
  "/mmc/autorun.tscm",
#endif

#if defined ALCOR_LANG_PICOLISP
  "/mmc/autorun.l",
  "/mmc/autorun.lc",
#endif

#if defined ALCOR_LANG_PICOC
  "/mmc/autorun.c",
  "/mmc/autorun.pc",
#endif

#if defined ALCOR_LANG_LUA
  "/mmc/autorun.lua",
  "/mmc/autorun.lc",
#endif
#endif
#if defined BUILD_ROMFS
#if defined ALCOR_LANG_TINYSCHEME
  "/rom/autorun.scm",
  "/rom/autorun.tscm",
#endif

#if defined ALCOR_LANG_PICOLISP
  "/rom/autorun.l",
  "/rom/autorun.lc",
#endif

#if defined ALCOR_LANG_PICOC
  "/rom/autorun.c",
  "/rom/autorun.pc",
#endif

#if defined ALCOR_LANG_LUA
  "/rom/autorun.lua",
  "/rom/autorun.lc",
#endif
#endif
};

extern char etext[];


#if defined (ELUA_BOOT_RPC) && defined (ALCOR_LANG_LUA)

#ifndef RPC_UART_ID
  #define RPC_UART_ID     CON_UART_ID
#endif

#ifndef RPC_TIMER_ID
  #define RPC_TIMER_ID    PLATFORM_TIMER_SYS_ID
#endif

#ifndef RPC_UART_SPEED
  #define RPC_UART_SPEED  CON_UART_SPEED
#endif

void boot_rpc( void )
{
  lua_State *L = lua_open();
  luaL_openlibs(L);  /* open libraries */
  
  // Set up UART for 8N1 w/ adjustable baud rate
  platform_uart_setup( RPC_UART_ID, RPC_UART_SPEED, 8, PLATFORM_UART_PARITY_NONE, PLATFORM_UART_STOPBITS_1 );
  
  // Start RPC Server
  lua_getglobal( L, "rpc" );
  lua_getfield( L, -1, "server" );
  lua_pushnumber( L, RPC_UART_ID );
  lua_pushnumber( L, RPC_TIMER_ID );
  lua_pcall( L, 2, 0, 0 );
}
#endif

// ****************************************************************************
//  Program entry point

int main( void )
{
  int i;
  FILE* fp;

  // Initialize platform first
  if( platform_init() != PLATFORM_OK )
  {
    // This should never happen
    while( 1 );
  }

  // Initialize device manager
  dm_init();

  // Register the ROM filesystem
  romfs_init();

  // Register the MMC filesystem
  mmcfs_init();

  // Register the Semihosting filesystem
  semifs_init();

  // Register the remote filesystem
  remotefs_init();

  // Search for autorun files in the defined order and execute the 1st if found
  for( i = 0; i < sizeof( boot_order ) / sizeof( *boot_order ); i++ )
  {
    if( ( fp = fopen( boot_order[ i ], "r" ) ) != NULL )
    {
      fclose( fp );
// The entry point for languages.
//
#if defined ALCOR_LANG_TINYSCHEME
      char* tinyscheme_argv[] = { "tinyscheme", boot_order[i], NULL };
      tinyscheme_main( 2, tinyscheme_argv );
#endif

#if defined ALCOR_LANG_PICOLISP
      char* picolisp_argv[] = { "picolisp", boot_order[i], NULL };
      picolisp_main( 2, picolisp_argv );
#endif

#if defined ALCOR_LANG_PICOC
      char* picoc_argv[] = { "picoc", boot_order[i], NULL };
      picoc_main( 2, picoc_argv );
#endif

#if defined ALCOR_LANG_LUA
      char* lua_argv[] = { "lua", boot_order[i], NULL };
      lua_main( 2, lua_argv );
#endif
      break; // autoruns only the first found
    }
  }

#if defined (ELUA_BOOT_RPC) && defined (ALCOR_LANG_LUA)
  boot_rpc();
#else
  
  // Run the shell
  if( shell_init() == 0 )
  {
#if defined ALCOR_LANG_TINYSCHEME
    char* tinyscheme_argv[] = { "tinyscheme", NULL };
    tinyscheme_main( 1, tinyscheme_argv );
#endif

#if defined ALCOR_LANG_PICOLISP
    char* picolisp_argv[] = { "picolisp", NULL };
    picolisp_main( 1, picolisp_argv );
#endif

#if defined ALCOR_LANG_PICOC
    char* picoc_argv[] = { "picoc", NULL };
    picoc_main( 1, picoc_argv );
#endif

#if defined ALCOR_LANG_LUA
    // Start Lua directly
    char* lua_argv[] = { "lua", NULL };
    lua_main( 1, lua_argv );
#endif
  }
  else
    shell_start();
#endif // #if defined (ELUA_BOOT_RPC) && defined (ALCOR_LANG_LUA)

#ifdef ELUA_SIMULATOR
  hostif_exit(0);
  return 0;
#else
  while( 1 );
#endif
}
