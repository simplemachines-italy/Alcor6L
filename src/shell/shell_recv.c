// Shell: 'recv' implementation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "shell.h"
#include "common.h"
#include "type.h"
#include "platform_conf.h"
#include "xmodem.h"

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
#endif

#if defined ALCOR_LANG_LUA
# include "lua.h"
# include "lualib.h"
# include "lauxlib.h"
#endif

#ifdef BUILD_XMODEM

const char shell_help_recv[] = "[<path>]\n"
  "  [<path>] - the data received via XMODEM will be saved to this file.\n"
#ifdef ALCOR_LANG_PICOC
  "Without arguments, it shows this help message.\n";
#else
  "Without arguments it runs Lua to execute the data it receives.\n";
#endif

const char shell_help_summary_recv[] = "receive files via XMODEM";

extern char *shell_prog;

void shell_recv( int argc, char **argv )
{
  char *p;
  long actsize;
#ifdef ALCOR_LANG_LUA
  lua_State* L;
#endif

#ifdef ALCOR_LANG_PICOC
  if( ( argc > 2 ) || ( argc == 0 ) )
#else
  if( argc > 2 )
#endif
  {
    SHELL_SHOW_HELP( recv );
    return;
  }

  if( ( shell_prog = malloc( XMODEM_INITIAL_BUFFER_SIZE ) ) == NULL )
  {
    printf( "Unable to allocate memory\n" );
    return;
  }
  printf( "Waiting for file ... " );
  if( ( actsize = xmodem_receive( &shell_prog ) ) < 0 )
  {
    if( actsize == XMODEM_ERROR_OUTOFMEM )
      printf( "file too big\n" );
    else
      printf( "XMODEM error\n" );
    goto exit;
  }
  // Eliminate the XMODEM padding bytes
  p = shell_prog + actsize - 1;
  while( *p == '\x1A' )
    p --;
  p ++;
  printf( "done, got %u bytes\n", ( unsigned )( p - shell_prog ) );          
  
  // we've received an argument, try saving it to a file
  if( argc == 2 )
  {
    FILE *foutput = fopen( argv[ 1 ], "w" );
    size_t file_sz = p - shell_prog;
    if( foutput == NULL )
    {
      printf( "unable to open file %s\n", argv[ 1 ] );
      goto exit;
    }
    if( fwrite( shell_prog, sizeof( char ), file_sz, foutput ) == file_sz )
      printf( "received and saved as %s\n", argv[ 1 ] );
    else
      printf( "unable to save file %s (no space left on target?)\n", argv[ 1 ] );
    fclose( foutput );
  }
#ifdef ALCOR_LANG_LUA
  else // no arg, running the file with lua.
  {
    if( ( L = lua_open() ) == NULL )
    {
      printf( "Unable to create Lua state\n" );
      goto exit;
    }
    luaL_openlibs( L );
    if( luaL_loadbuffer( L, shell_prog, p - shell_prog, "xmodem" ) != 0 )
      printf( "Error: %s\n", lua_tostring( L, -1 ) );
    else
      if( lua_pcall( L, 0, LUA_MULTRET, 0 ) != 0 )
        printf( "Error: %s\n", lua_tostring( L, -1 ) );
    lua_close( L );
  }
#endif
exit:
  free( shell_prog );
  shell_prog = NULL;
}

#else // #ifdef BUILD_XMODEM

const char shell_help_recv[] = "";
const char shell_help_summary_recv[] = "";

void shell_recv( int argc, char **argv )
{
  shellh_not_implemented_handler( argc, argv );
}

#endif // #ifdef BUILD_XMODEM

