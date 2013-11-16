
/* 07jun12abu
 * (c) Software Lab. Alexander Burger
 * Modified for Alcor6L, <simplemachines-italy>, 2013.
 */

#ifndef __MPLISP_H__
#define __MPLISP_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <stdint.h>

#define PICOLISP_WORD ((int)sizeof(long))
#define BITS (8*PICOLISP_WORD)

#if defined ALCOR_BOARD_EKLM3S8962
# define PC_MUL 32
#elif defined ALCOR_SIM_LINUX
# define PC_MUL 64
#elif defined ALCOR_BOARD_MIZAR32
# define PC_MUL 1024
#elif defined ALCOR_BOARD_HEXAGON
# define PC_MUL 64
#else
# error "Unknown board/CPU. PC_MUL not defined."
#endif

#define CELLS (PC_MUL*1024/sizeof(cell))

typedef unsigned long word;
typedef unsigned char byte;
typedef unsigned char *ptr;

#undef bool
typedef enum {NO,YES} bool;

typedef struct cell {            // PicoLisp primary data type
   struct cell *car;
   struct cell *cdr;
} cell, *any;

typedef any (*fun)(any);

typedef struct heap {
   cell cells[CELLS];
   struct heap *next;
} heap;

typedef struct bindFrame {
   struct bindFrame *link;
   int i, cnt;
   struct {any sym; any val;} bnd[1];
} bindFrame;

typedef struct inFrame {
   struct inFrame *link;
   void (*get)(void);
   FILE *fp;
   int next;
} inFrame;

typedef struct outFrame {
   struct outFrame *link;
   void (*put)(int);
   FILE *fp;
} outFrame;

typedef struct parseFrame {
   int i;
   word w;
   any sym, nm;
} parseFrame;

typedef struct stkEnv {
   cell *stack, *arg;
   bindFrame *bind;
   int next;
   any key, cls, *make, *yoke;
   inFrame *inFrames;
   outFrame *outFrames;
   parseFrame *parser;
   void (*get)(void);
   void (*put)(int);
   bool brk;
} stkEnv;

typedef struct catchFrame {
   struct catchFrame *link;
   any tag, fin;
   stkEnv env;
   jmp_buf rst;
} catchFrame;

/*** Macros ***/
#define Free(p)         ((p)->car=Avail, Avail=(p))

/* Number access */
#define num(x)          ((long)(x))
#define txt(n)          ((any)(num(n)<<1|1))
#define box(n)          ((any)(num(n)<<2|2))
#define unBox(n)        (num(n)>>2)
#define Zero            ((any)2)
#define One             ((any)6)

/* Symbol access */
#define symPtr(x)       ((any)&(x)->cdr)
#define val(x)          ((x)->car)
#define tail(x)         (((x)-1)->cdr)

/* Cell access */
#define car(x)          ((x)->car)
#define cdr(x)          ((x)->cdr)
#define caar(x)         (car(car(x)))
#define cadr(x)         (car(cdr(x)))
#define cdar(x)         (cdr(car(x)))
#define cddr(x)         (cdr(cdr(x)))
#define caaar(x)        (car(car(car(x))))
#define caadr(x)        (car(car(cdr(x))))
#define cadar(x)        (car(cdr(car(x))))
#define caddr(x)        (car(cdr(cdr(x))))
#define cdaar(x)        (cdr(car(car(x))))
#define cdadr(x)        (cdr(car(cdr(x))))
#define cddar(x)        (cdr(cdr(car(x))))
#define cdddr(x)        (cdr(cdr(cdr(x))))
#define cadddr(x)       (car(cdr(cdr(cdr(x)))))
#define cddddr(x)       (cdr(cdr(cdr(cdr(x)))))

#define data(c)         ((c).car)
#define Save(c)         ((c).cdr=Env.stack, Env.stack=&(c))
#define drop(c)         (Env.stack=(c).cdr)
#define Push(c,x)       (data(c)=(x), Save(c))
#define Pop(c)          (drop(c), data(c))

#define Bind(s,f)       ((f).i=0, (f).cnt=1, (f).bnd[0].sym=(s), (f).bnd[0].val=val(s), (f).link=Env.bind, Env.bind=&(f))
#define Unbind(f)       (val((f).bnd[0].sym)=(f).bnd[0].val, Env.bind=(f).link)

/* Predicates */
#define isNil(x)        ((x)==Nil)
#define isTxt(x)        (num(x)&1)
#define isNum(x)        (num(x)&2)
#define isSym(x)        (num(x)&PICOLISP_WORD)
#define isSymb(x)       ((num(x)&(PICOLISP_WORD+2))==PICOLISP_WORD)
#define isCell(x)       (!(num(x)&(2*PICOLISP_WORD-1)))

/* Evaluation */
#define EVAL(x)         (isNum(x)? x : isSym(x)? val(x) : evList(x))

#ifndef ALCOR_SIM_LINUX
# define evSubr(f,x)     (*(fun)(num(f) & ~2))(x)
#else
# define evSubr(f,x)     (*(fun)(num(f) >> 2))(x)
#endif

/* Error checking */
#define NeedNum(ex,x)   if (!isNum(x)) numError(ex,x)
#define NeedSym(ex,x)   if (!isSym(x)) symError(ex,x)
#define NeedSymb(ex,x)  if (!isSymb(x)) symError(ex,x)
#define NeedPair(ex,x)  if (!isCell(x)) pairError(ex,x)
#define NeedAtom(ex,x)  if (isCell(x)) atomError(ex,x)
#define NeedLst(ex,x)   if (!isCell(x) && !isNil(x)) lstError(ex,x)
#define NeedVar(ex,x)   if (isNum(x)) varError(ex,x)
#define CheckVar(ex,x)  if ((x)>=Nil && (x)<=T) protError(ex,x)

/* Globals */
extern int Chr, Trace;
extern char **AV, *AV0, *Home;
extern heap *Heaps;
extern cell *Avail;
extern stkEnv Env;
extern catchFrame *CatchPtr;
extern FILE *InFile, *OutFile;
extern any TheKey, TheCls, Thrown;
extern any Intern[2], Transient[2], Reloc;
extern any ApplyArgs, ApplyBody;
extern any Nil, Meth, Quote, T, At, At2, At3, This;
extern any Dbg, Scl, Class, Up, Err, Msg, Bye;

// globals for picoLisp platform modules.

// system timer.
any sys_timer;

// i2c symbols.
any plisp_i2c_fast;
any plisp_i2c_slow;
any plisp_i2c_trans;
any plisp_i2c_recv;

// spi symbols.
any plisp_spi_master;
any plisp_spi_slave;

// pio symbols.
any plisp_pio_input;
any plisp_pio_output;
any plisp_pio_pullup;
any plisp_pio_pulldown;
any plisp_pio_nopull;

// uart symbols.
any plisp_uart_par_even;
any plisp_uart_par_odd;
any plisp_uart_par_none;
any plisp_uart_stop_1;
any plisp_uart_stop_1_5;
any plisp_uart_stop_2;
any plisp_uart_no_timeout;
any plisp_uart_inf_timeout;
any plisp_uart_flow_none;
any plisp_uart_flow_rts;
any plisp_uart_flow_cts;

// term modules.
any plisp_term_wait;
any plisp_term_nowait;
  
// Alcor6L platform modules.
any pd_platform(any x);
any pd_cpu(any x);
any pd_board(any x);

// Terminal module.
any plisp_term_clrscr(any x);
any plisp_term_clreol(any x);
any plisp_term_moveto(any x);
any plisp_term_moveup(any x);
any plisp_term_movedown(any x);
any plisp_term_moveleft(any x);
any plisp_term_moveright(any x);
any plisp_term_getlines(any x);
any plisp_term_getcols(any x);
any plisp_term_prinl(any x);
any plisp_term_getcx(any x);
any plisp_term_getcy(any x);
any plisp_term_getchar(any x);
any plisp_term_decode(any x);

// eLua module.
any elua_version(any x);
any elua_save_history(any x);
any elua_shell(any x);

// cpu module.
any cpu_w32(any x);
any cpu_r32(any x);
any cpu_w16(any x);
any cpu_r16(any x);
any cpu_w8(any x);
any cpu_r8(any x);
any cpu_clock(any x);

// can module.
any can_setup(any x);
any can_send(any x);
any can_recv(any x);

// timer module.
any tmr_delay(any ex);
any tmr_read(any ex);
any tmr_start(any ex);
any tmr_gettimediff(any ex);
any tmr_getdiffnow(any ex);
any tmr_getmindelay(any ex);
any tmr_getmaxdelay(any ex);
any tmr_setclock(any ex);
any tmr_getclock(any ex);

// i2c module.
any plisp_i2c_setup(any ex);
any plisp_i2c_start(any ex);
any plisp_i2c_stop(any ex);
any plisp_i2c_address(any ex);
any plisp_i2c_write(any ex);
any plisp_i2c_read(any ex);

// pwm module.
any plisp_pwm_setup(any ex);
any plisp_pwm_start(any ex);
any plisp_pwm_stop(any ex);
any plisp_pwm_setclock(any ex);
any plisp_pwm_getclock(any ex);

// spi module.
any plisp_spi_sson(any ex);
any plisp_spi_ssoff(any ex);
any plisp_spi_setup(any ex);
any plisp_spi_write(any ex);

// gpio module.
any plisp_pio_pin_setdir(any ex);
any plisp_pio_pin_setpull(any ex);
any plisp_pio_pin_setval(any ex);
any plisp_pio_pin_sethigh(any ex);
any plisp_pio_pin_setlow(any ex);
any plisp_pio_pin_getval(any ex);
any plisp_pio_port_setdir(any ex);
any plisp_pio_port_setpull(any ex);
any plisp_pio_port_setval(any ex);
any plisp_pio_port_sethigh(any ex);
any plisp_pio_port_setlow(any ex);
any plisp_pio_port_getval(any ex);
any plisp_pio_decode(any ex);

// uart module.
any plisp_uart_setup(any ex);
any plisp_uart_write(any ex);
any plisp_uart_set_buffer(any ex);
any plisp_uart_set_flow_control(any ex);
any plisp_uart_getchar(any ex);
any plisp_uart_vuart_tmr_ident(any ex);
any plisp_uart_read(any ex);

#include "platform_picolisp.h"

// Include prototypes for target/
// platform specific modules.
#ifdef PICOLISP_TARGET_SPECIFIC_PROTOS
  PICOLISP_TARGET_SPECIFIC_PROTOS
#endif

/* Prototypes */
int picolisp_main(int argc, char *argv[]);
void *alloc(void*,size_t);
any apply(any,any,bool,int,cell*);
void argError(any,any) __attribute__ ((noreturn));
void atomError(any,any) __attribute__ ((noreturn));
void begString(void);
any boxSubr(fun);
void brkLoad(any);
int bufNum(char[BITS/2],long);
int bufSize(any);
void bufString(any,char*);
void bye(int) __attribute__ ((noreturn));
void pairError(any,any) __attribute__ ((noreturn));
any circ(any);
int compare(any,any);
any cons(any,any);
any consName(word,any);
any consSym(any,word);
void newline(void);
any endString(void);
bool equal(any,any);
void err(any,any,char*,...) __attribute__ ((noreturn));
any evExpr(any,any);
any evList(any);
long evNum(any,any);
any evSym(any);
void execError(char*) __attribute__ ((noreturn));
int firstByte(any);
any get(any,any);
int getByte(int*,word*,any*);
int getByte1(int*,word*,any*);
void getStdin(void);
void giveup(char*) __attribute__ ((noreturn));
void heapAlloc(void);
void initSymbols(void);
any intern(any,any[2]);
bool isBlank(any);
any isIntern(any,any[2]);
void lstError(any,any) __attribute__ ((noreturn));
any load(any,int,any);
any loadAll(any);
any method(any);
any mkChar(int);
any mkChar2(int,int);
any mkSym(byte*);
any mkStr(const char*);
any mkTxt(int);
any name(any);
int numBytes(any);
void numError(any,any) __attribute__ ((noreturn));
any numToSym(any,int,int,int);
void outName(any);
void outNum(long);
void outString(char*);
void pack(any,int*,word*,any*,cell*);
int pathSize(any);
void pathString(any,char*);
void popInFiles(void);
void popOutFiles(void);
any popSym(int,word,any,cell*);
void prin(any);
void print(any);
void protError(any,any) __attribute__ ((noreturn));
void pushInFiles(inFrame*);
void pushOutFiles(outFrame*);
void put(any,any,any);
void putByte(int,int*,word*,any*,cell*);
void putByte0(int*,word*,any*);
void putByte1(int,int*,word*,any*);
void putStdout(int);
void rdOpen(any,any,inFrame*);
any read1(int);
int secondByte(any);
void space(void);
int symBytes(any);
void symError(any,any) __attribute__ ((noreturn));
any symToNum(any,int,int,int);
void undefined(any,any);
void unintern(any,any[2]);
void unwind (catchFrame*);
void varError(any,any) __attribute__ ((noreturn));
void wrOpen(any,any,outFrame*);
long xNum(any,any);
any xSym(any);

any doAbs(any);
any doAdd(any);
any doAll(any);
any doAnd(any);
any doAny(any);
any doAppend(any);
any doApply(any);
any doArg(any);
any doArgs(any);
any doArgv(any);
any doArrow(any);
any doAsoq(any);
any doAs(any);
any doAssoc(any);
any doAt(any);
any doAtom(any);
any doBind(any);
any doBitAnd(any);
any doBitOr(any);
any doBitQ(any);
any doBitXor(any);
any doBool(any);
any doBox(any);
any doBoxQ(any);
any doBreak(any);
any doBy(any);
any doBye(any) __attribute__ ((noreturn));
any doCaaar(any);
any doCaadr(any);
any doCaar(any);
any doCadar(any);
any doCadddr(any);
any doCaddr(any);
any doCadr(any);
any doCar(any);
any doCase(any);
any doCatch(any);
any doCdaar(any);
any doCdadr(any);
any doCdar(any);
any doCddar(any);
any doCddddr(any);
any doCdddr(any);
any doCddr(any);
any doCdr(any);
any doChain(any);
any doChar(any);
any doChop(any);
any doCirc(any);
any doCircQ(any);
any doClip(any);
any doCmd(any);
any doCnt(any);
any doCol(any);
any doCon(any);
any doConc(any);
any doCond(any);
any doCons(any);
any doCopy(any);
any doCut(any);
any doDate(any);
any doDe(any);
any doDec(any);
any doDef(any);
any doDefault(any);
any doDel(any);
any doDelete(any);
any doDelq(any);
any doDiff(any);
any doDiv(any);
any doDm(any);
any doDo(any);
any doE(any);
any doEnv(any);
any doEof(any);
any doEol(any);
any doEq(any);
any doEq0(any);
any doEqT(any);
any doEqual(any);
any doEval(any);
any doExtra(any);
any doExtract(any);
any doFifo(any);
any doFill(any);
any doFilter(any);
any doFin(any);
any doFinally(any);
any doFind(any);
any doFish(any);
any doFlgQ(any);
any doFlip(any);
any doFlush(any);
any doFold(any);
any doFor(any);
any doFormat(any);
any doFrom(any);
any doFull(any);
any doFunQ(any);
any doGc(any);
any doGe(any);
any doGe0(any);
any doGet(any);
any doGetl(any);
any doGlue(any);
any doGt(any);
any doGt0(any);
any doHead(any);
any doHeap(any);
any doHide(any);
any doIdx(any);
any doIf(any);
any doIf2(any);
any doIfn(any);
any doIn(any);
any doInc(any);
any doIndex(any);
any doIntern(any);
any doIsa(any);
any doJob(any);
any doLast(any);
any doLe(any);
any doLe0(any);
any doLength(any);
any doLet(any);
any doLetQ(any);
any doLine(any);
any doLink(any);
any doList(any);
any doLit(any);
any doLstQ(any);
any doLoad(any);
any doLoop(any);
any doLowQ(any);
any doLowc(any);
any doLt(any);
any doLt0(any);
any doLup(any);
any doMade(any);
any doMake(any);
any doMap(any);
any doMapc(any);
any doMapcan(any);
any doMapcar(any);
any doMapcon(any);
any doMaplist(any);
any doMaps(any);
any doMatch(any);
any doMax(any);
any doMaxi(any);
any doMember(any);
any doMemq(any);
any doMeta(any);
any doMeth(any);
any doMethod(any);
any doMin(any);
any doMini(any);
any doMix(any);
any doMmeq(any);
any doMul(any);
any doMulDiv(any);
any doName(any);
any doNand(any);
any doNEq(any);
any doNEq0(any);
any doNEqT(any);
any doNEqual(any);
any doNeed(any);
any doNew(any);
any doNext(any);
any doNil(any);
any doNond(any);
any doNor(any);
any doNot(any);
any doNth(any);
any doNumQ(any);
any doOff(any);
any doOffset(any);
any doOn(any);
any doOne(any);
any doOnOff(any);
any doOpt(any);
any doOr(any);
any doOut(any);
any doPack(any);
any doPair(any);
any doPass(any);
any doPath(any);
any doPatQ(any);
any doPeek(any);
any doPick(any);
any doPop(any);
any doPreQ(any);
any doPrin(any);
any doPrinl(any);
any doPrint(any);
any doPrintln(any);
any doPrintsp(any);
any doPrior(any);
any doProg(any);
any doProg1(any);
any doProg2(any);
any doProp(any);
any doPropCol(any);
any doProve(any);
any doPush(any);
any doPush1(any);
any doPut(any);
any doPutl(any);
any doQueue(any);
any doQuit(any);
any doQuote(any);
any doRand(any);
any doRank(any);
any doRead(any);
any doRem(any);
any doReplace(any);
any doRest(any);
any doReverse(any);
any doRot(any);
any doRun(any);
any doSave(any);
any doSect(any);
any doSeed(any);
any doSeek(any);
any doSemicol(any);
any doSend(any);
any doSet(any);
any doSetCol(any);
any doSetq(any);
any doShift(any);
any doSize(any);
any doSkip(any);
any doSort(any);
any doSpace(any);
any doSplit(any);
any doSpQ(any);
any doSqrt(any);
any doState(any);
any doStem(any);
any doStr(any);
any doStrip(any);
any doStrQ(any);
any doSub(any);
any doSum(any);
any doSuper(any);
any doSym(any);
any doSymQ(any);
any doT(any);
any doTail(any);
any doText(any);
any doThrow(any);
any doTill(any);
any doTrace(any);
any doTrim(any);
any doTry(any);
any doType(any);
any doUnify(any);
any doUnless(any);
any doUntil(any);
any doUp(any);
any doUppQ(any);
any doUppc(any);
any doUse(any);
any doVal(any);
any doWhen(any);
any doWhile(any);
any doWith(any);
any doXchg(any);
any doXor(any);
any doYoke(any);
any doZap(any);
any doZero(any);

/* List element access */
static inline any nCdr(int n, any x) {
   while (--n >= 0)
      x = cdr(x);
   return x;
}

static inline any nth(int n, any x) {
   if (--n < 0)
      return Nil;
   return nCdr(n,x);
}

static inline any getn(any x, any y) {
   if (isNum(x)) {
      long n = unBox(x);

      if (n < 0) {
         while (++n)
            y = cdr(y);
         return cdr(y);
      }
      if (n == 0)
         return Nil;
      while (--n)
         y = cdr(y);
      return car(y);
   }
   do
      if (isCell(car(y)) && x == caar(y))
         return cdar(y);
   while (isCell(y = cdr(y)));
   return Nil;
}

/* List length calculation */
static inline int length(any x) {
   int n;

   for (n = 0; isCell(x); x = cdr(x))
      ++n;
   return n;
}

/* list length minus 1 is the number
   of parameters to a function */
#define plen(foo) (length(foo) - 1)

/* Membership */
static inline any member(any x, any y) {
   any z = y;

   while (isCell(y)) {
      if (equal(x, car(y)))
         return y;
      if (z == (y = cdr(y)))
         return NULL;
   }
   return isNil(y) || !equal(x,y)? NULL : y;
}

static inline any memq(any x, any y) {
   any z = y;

   while (isCell(y)) {
      if (x == car(y))
         return y;
      if (z == (y = cdr(y)))
         return NULL;
   }
   return isNil(y) || x != y? NULL : y;
}

static inline int indx(any x, any y) {
   int n = 1;
   any z = y;

   while (isCell(y)) {
      if (equal(x, car(y)))
         return n;
      ++n;
      if (z == (y = cdr(y)))
         return 0;
   }
   return 0;
}

/* List interpreter */
static inline any prog(any x) {
   any y;

   do
      y = EVAL(car(x));
   while (isCell(x = cdr(x)));
   return y;
}

static inline any run(any x) {
   any y;
   cell at;

   Push(at,val(At));
   do
      y = EVAL(car(x));
   while (isCell(x = cdr(x)));
   val(At) = Pop(at);
   return y;
}

#endif // #define __MPLISP_H__