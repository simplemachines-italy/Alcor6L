// Shell: 'picoc' implementation

#ifdef ALCOR_LANG_PICOC

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

const char shell_help_picoc[] = "[-e <stat>] [-l <name>] [-i] [-v] [<file>]\n"
  "  [<script>]: execute the given file and runs 'main'.\n"
  "  [-s <file>]: Script mode (runs program without calling main).\n"
  "  [-i]: enter interactive mode.\n"
  "  [-v]: show version information.\n"
  "Without arguments it executes the interactive PicoC interpreter.\n";
const char shell_help_summary_picoc[] = "start a PicoC session";

void shell_picoc( int argc, char **argv )
{
  printf( "Press " SHELL_EOF_STRING " to exit PicoC\n" );
  picoc_main( argc, argv );
  clearerr( stdin );
}

#endif // #ifdef ALCOR_LANG_PICOC
