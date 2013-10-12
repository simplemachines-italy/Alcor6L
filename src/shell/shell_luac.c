// Shell: 'luac' implementation

#ifdef ALCOR_LANG_LUA

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

#if LUA_COMPILER

const char shell_help_luac[] = "usage: luac [options] [filenames].\n"
  " Available options are:\n"
  "  -           process stdin\n"
  "  -l          list\n"
  "  -o name     output to file 'name' (default is \"/mmc/luac)\")\n"
  "  -p          parse only\n"
  "  -s          strip debug information\n"
  "  -v          show version information\n"
  "  -cci bits       cross-compile with given integer size\n"
  "  -ccn type bits  cross-compile with given lua_Number type and size\n"
  "  -cce endian     cross-compile with given endianness ('big or 'little')\n"
  "  --          stop handling options\n"
  "Without arguments, it prints the above message.\n";
const char shell_help_summary_luac[] = "compile scripts with the Lua compiler";

void shell_luac( int argc, char **argv )
{
  luac_main( argc, argv );
  clearerr( stdin );
}

#else // #if LUA_COMPILER

const char shell_help_luac[] = "";
const char shell_help_summary_luac[] = "";

void shell_luac( int argc, char **argv )
{
  shellh_not_implemented_handler( argc, argv );
}

#endif // #if LUA_COMPILER

#endif // #ifdef ALCOR_LANG_LUA
