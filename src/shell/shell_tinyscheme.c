// Shell: 'tinyscheme' implementation

#ifdef ALCOR_LANG_TINYSCHEME

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

// EOF is different in UART mode and TCP/IP mode
#ifdef BUILD_CON_GENERIC
  #define SHELL_EOF_STRING        "CTRL+Z"
#else
  #define SHELL_EOF_STRING        "CTRL+D"
#endif

const char shell_help_tinyscheme[] = "[<file1> <file2> ...]\n"
  "  followed by\n"
  "  -1 <file> [<arg1> <arg2> ...]\n"
  "  -c <Scheme commands> [<arg1> <arg2> ...]\n"
  "  assuming that the executable is named tinyscheme.\n"
  "  Use - as filename for stdin.\n";
const char shell_help_summary_tinyscheme[] = "start a TinyScheme session";

void shell_tinyscheme( int argc, char **argv )
{
  printf( "Press " SHELL_EOF_STRING " to exit TinyScheme\n" );
  tinyscheme_main( argc, argv );
  clearerr( stdin );
}

#endif // #ifdef ALCOR_LANG_TINYSCHEME
