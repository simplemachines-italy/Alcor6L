
/* 23jan11abu
 * (c) Software Lab. Alexander Burger
 * Modified for Alcor6L <simplemachines-italy>
 * July, 2013.
 */

#include "pico.h"
#include "platform_conf.h"

typedef struct symInit {fun code; char *name;} symInit;

static symInit Symbols[] = {
#if defined PICOLISP_PLATFORM_LIBS_ROM
#  undef _ROM
#  define _ROM(module)\
   PICOLISP_MOD_##module
   PICOLISP_PLATFORM_LIBS_ROM
#if defined PICOLISP_TARGET_SPECIFIC_LIBS
   PICOLISP_TARGET_SPECIFIC_LIBS
#endif
#endif
   {doAbs, "abs"},
   {doAdd, "+"},
   {doAll, "all"},
   {doAnd, "and"},
   {doAny, "any"},
   {doAppend, "append"},
   {doApply, "apply"},
   {doArg, "arg"},
   {doArgs, "args"},
   {doArgv, "argv"},
   {doArrow, "->"},
   {doAs, "as"},
   {doAsoq, "asoq"},
   {doAssoc, "assoc"},
   {doAt, "at"},
   {doAtom, "atom"},
   {doBind, "bind"},
   {doBitAnd, "&"},
   {doBitOr, "|"},
   {doBitQ, "bit?"},
   {doBitXor, "x|"},
   {doBool, "bool"},
   {doBox, "box"},
   {doBoxQ, "box?"},
   {doBreak, "!"},
   {doBy, "by"},
   {doBye, "bye"},
   {doCaaar, "caaar"},
   {doCaadr, "caadr"},
   {doCaar, "caar"},
   {doCadar, "cadar"},
   {doCadddr, "cadddr"},
   {doCaddr, "caddr"},
   {doCadr, "cadr"},
   {doCar, "car"},
   {doCase, "case"},
   {doCatch, "catch"},
   {doCdaar, "cdaar"},
   {doCdadr, "cdadr"},
   {doCdar, "cdar"},
   {doCddar, "cddar"},
   {doCddddr, "cddddr"},
   {doCdddr, "cdddr"},
   {doCddr, "cddr"},
   {doCdr, "cdr"},
   {doChar, "char"},
   {doChain, "chain"},
   {doChop, "chop"},
   {doCirc, "circ"},
   {doCircQ, "circ?"},
   {doClip, "clip"},
   {doCmd, "cmd"},
   {doCnt, "cnt"},
   {doCol, ":"},
   {doCon, "con"},
   {doConc, "conc"},
   {doCond, "cond"},
   {doCons, "cons"},
   {doCopy, "copy"},
   {doCut, "cut"},
   {doDate, "date"},
   {doDe, "de"},
   {doDec, "dec"},
   {doDef, "def"},
   {doDefault, "default"},
   {doDel, "del"},
   {doDelete, "delete"},
   {doDelq, "delq"},
   {doDiff, "diff"},
   {doDiv, "/"},
   {doDm, "dm"},
   {doDo, "do"},
   {doE, "e"},
   {doEnv, "env"},
   {doEof, "eof"},
   {doEol, "eol"},
   {doEq, "=="},
   {doEq0, "=0"},
   {doEqT, "=T"},
   {doEqual, "="},
   {doEval, "eval"},
   {doExtra, "extra"},
   {doExtract, "extract"},
   {doFifo, "fifo"},
   {doFill, "fill"},
   {doFilter, "filter"},
   {doFin, "fin"},
   {doFinally, "finally"},
   {doFind, "find"},
   {doFish, "fish"},
   {doFlgQ, "flg?"},
   {doFlip, "flip"},
   {doFlush, "flush"},
   {doFold, "fold"},
   {doFor, "for"},
   {doFormat, "format"},
   {doFrom, "from"},
   {doFull, "full"},
   {doFunQ, "fun?"},
   {doGc, "gc"},
   {doGe, ">="},
   {doGe0, "ge0"},
   {doGet, "get"},
   {doGetl, "getl"},
   {doGlue, "glue"},
   {doGt, ">"},
   {doGt0, "gt0"},
   {doHead, "head"},
   {doHeap, "heap"},
   {doHide, "===="},
   {doIdx, "idx"},
   {doIf, "if"},
   {doIf2, "if2"},
   {doIfn, "ifn"},
   {doIn, "in"},
   {doInc, "inc"},
   {doIndex, "index"},
   {doIntern, "intern"},
   {doIsa, "isa"},
   {doJob, "job"},
   {doLast, "last"},
   {doLe, "<="},
   {doLe0, "le0"},
   {doLength, "length"},
   {doLet, "let"},
   {doLetQ, "let?"},
   {doLine, "line"},
   {doLink, "link"},
   {doList, "list"},
   {doLit, "lit"},
   {doLstQ, "lst?"},
   {doLoad, "load"},
   {doLoop, "loop"},
   {doLowQ, "low?"},
   {doLowc, "lowc"},
   {doLt, "<"},
   {doLt0, "lt0"},
   {doLup, "lup"},
   {doMade, "made"},
   {doMake, "make"},
   {doMap, "map"},
   {doMapc, "mapc"},
   {doMapcan, "mapcan"},
   {doMapcar, "mapcar"},
   {doMapcon, "mapcon"},
   {doMaplist, "maplist"},
   {doMaps, "maps"},
   {doMatch, "match"},
   {doMax, "max"},
   {doMaxi, "maxi"},
   {doMember, "member"},
   {doMemq, "memq"},
   {doMeta, "meta"},
   {doMethod, "method"},
   {doMin, "min"},
   {doMini, "mini"},
   {doMix, "mix"},
   {doMmeq, "mmeq"},
   {doMul, "*"},
   {doMulDiv, "*/"},
   {doName, "name"},
   {doNand, "nand"},
   {doNEq, "n=="},
   {doNEq0, "n0"},
   {doNEqT, "nT"},
   {doNEqual, "<>"},
   {doNeed, "need"},
   {doNew, "new"},
   {doNext, "next"},
   {doNil, "nil"},
   {doNond, "nond"},
   {doNor, "nor"},
   {doNot, "not"},
   {doNth, "nth"},
   {doNumQ, "num?"},
   {doOff, "off"},
   {doOffset, "offset"},
   {doOn, "on"},
   {doOne, "one"},
   {doOnOff, "onOff"},
   {doOpt, "opt"},
   {doOr, "or"},
   {doOut, "out"},
   {doPack, "pack"},
   {doPair, "pair"},
   {doPass, "pass"},
   {doPath, "path"},
   {doPatQ, "pat?"},
   {doPeek, "peek"},
   {doPick, "pick"},
   {doPop, "pop"},
   {doPreQ, "pre?"},
   {doPrin, "prin"},
   {doPrinl, "prinl"},
   {doPrint, "print"},
   {doPrintln, "println"},
   {doPrintsp, "printsp"},
   {doPrior, "prior"},
   {doProg, "prog"},
   {doProg1, "prog1"},
   {doProg2, "prog2"},
   {doProp, "prop"},
   {doPropCol, "::"},
   {doProve, "prove"},
   {doPush, "push"},
   {doPush1, "push1"},
   {doPut, "put"},
   {doPutl, "putl"},
   {doQueue, "queue"},
   {doQuit, "quit"},
   {doRand, "rand"},
   {doRank, "rank"},
   {doRead, "read"},
   {doRem, "%"},
   {doReplace, "replace"},
   {doRest, "rest"},
   {doReverse, "reverse"},
   {doRot, "rot"},
   {doRun, "run"},
   {doSave, "save"},
   {doSect, "sect"},
   {doSeed, "seed"},
   {doSeek, "seek"},
   {doSemicol, ";"},
   {doSend, "send"},
   {doSet, "set"},
   {doSetCol, "=:"},
   {doSetq, "setq"},
   {doShift, ">>"},
   {doSize, "size"},
   {doSkip, "skip"},
   {doSort, "sort"},
   {doSpace, "space"},
   {doSplit, "split"},
   {doSpQ, "sp?"},
   {doSqrt, "sqrt"},
   {doState, "state"},
   {doStem, "stem"},
   {doStr, "str"},
   {doStrip, "strip"},
   {doStrQ, "str?"},
   {doSub, "-"},
   {doSum, "sum"},
   {doSuper, "super"},
   {doSym, "sym"},
   {doSymQ, "sym?"},
   {doT, "t"},
   {doTail, "tail"},
   {doText, "text"},
   {doThrow, "throw"},
   {doTill, "till"},
   {doTrace, "$"},
   {doTrim, "trim"},
   {doTry, "try"},
   {doType, "type"},
   {doUnify, "unify"},
   {doUnless, "unless"},
   {doUntil, "until"},
   {doUp, "up"},
   {doUppQ, "upp?"},
   {doUppc, "uppc"},
   {doUse, "use"},
   {doVal, "val"},
   {doWhen, "when"},
   {doWhile, "while"},
   {doWith, "with"},
   {doXchg, "xchg"},
   {doXor, "xor"},
   {doYoke, "yoke"},
   {doZap, "zap"},
   {doZero, "zero"},
};

static any initSym(any v, char *s) {
   any x;

   val(x = intern(mkSym((byte*)s), Intern)) = v;
   return x;
}

#include "common.h"

void initSymbols(void) {
   int i;

   Nil = symPtr(Avail),  Avail = Avail->car->car;  // Allocate 2 cells for NIL
   tail(Nil) = txt(83 | 73<<7 | 79<<14);
   val(Nil) = tail(Nil+1) = val(Nil+1) = Nil;
   Intern[0] = Intern[1] = Transient[0] = Transient[1] = Nil;
   intern(Nil, Intern);
   Meth  = initSym(boxSubr(doMeth), "meth");
   Quote = initSym(boxSubr(doQuote), "quote");

   // system timer symbols.
#ifdef PICOLISP_MOD_TIMER
   sys_timer = initSym(box(PLATFORM_TIMER_SYS_ID), "*tmr-sys-timer*");
#endif
   
   // i2c symbols.
#ifdef PICOLISP_MOD_I2C
   plisp_i2c_fast = initSym(box(PLATFORM_I2C_SPEED_FAST), "*i2c-fast*");
   plisp_i2c_slow = initSym(box(PLATFORM_I2C_SPEED_SLOW), "*i2c-slow*");
   plisp_i2c_trans = initSym(box(PLATFORM_I2C_DIRECTION_TRANSMITTER), "*i2c-transmitter*");
   plisp_i2c_recv = initSym(box(PLATFORM_I2C_DIRECTION_RECEIVER), "*i2c-receiver*");
#endif

   // can symbols.
#ifdef PICOLISP_MOD_CAN
   can_id_ext = initSym(box(ELUA_CAN_ID_EXT), "*can-id-ext*");
   can_id_std = initSym(box(ELUA_CAN_ID_STD), "*can-id-std*");
#endif

   // spi symbols.
#ifdef PICOLISP_MOD_SPI
   plisp_spi_master = initSym(box(PLATFORM_SPI_MASTER), "*spi-master*");
   plisp_spi_slave = initSym(box(PLATFORM_SPI_SLAVE), "*spi-slave*");
#endif

   // pio symbols.
#ifdef PICOLISP_MOD_PIO
   // these two macros are defined locally in modules/pio.c
   // since they can't be accessed from here, we're defining
   // them here again.
   #define PIO_DIR_INPUT  1
   #define PIO_DIR_OUTPUT 0

   plisp_pio_input = initSym(box(PIO_DIR_INPUT), "*pio-input*");
   plisp_pio_output = initSym(box(PIO_DIR_OUTPUT), "*pio-output*");
   plisp_pio_pullup = initSym(box(PLATFORM_IO_PIN_PULLUP), "*pio-pullup*");
   plisp_pio_pulldown = initSym(box(PLATFORM_IO_PIN_PULLDOWN), "*pio-pulldown*");
   plisp_pio_nopull = initSym(box(PLATFORM_IO_PIN_NOPULL), "*pio-nopull*");
#endif

   T     = initSym(Nil, "T"),  val(T) = T;  // Last protected symbol

   At    = initSym(Nil, "@");
   At2   = initSym(Nil, "@@");
   At3   = initSym(Nil, "@@@");
   This  = initSym(Nil, "This");
   Dbg   = initSym(Nil, "*Dbg");
   Scl   = initSym(Zero, "*Scl");
   Class = initSym(Nil, "*Class");
   Up    = initSym(Nil, "^");
   Err   = initSym(Nil, "*Err");
   Msg   = initSym(Nil, "*Msg");
   Bye   = initSym(Nil, "*Bye");  // Last unremovable symbol

   for (i = 0; i < (int)(sizeof(Symbols)/sizeof(symInit)); ++i)
      initSym(boxSubr(Symbols[i].code), Symbols[i].name);
}
