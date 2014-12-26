/* 05oct14abu
 * (c) Software Lab. Alexander Burger
 *
 * 15dec2014
 * (c) SimpleMachines. Raman Gopalan
 */

#include "pico.h"

#if (PICOLISP_OPTIMIZE_MEMORY == 2)
static void mark(any);
#endif

/* Mark data */
#if (PICOLISP_OPTIMIZE_MEMORY == 2)
static void markTail(any x) {
#else
static void mark(any x) {
#endif
#if (PICOLISP_OPTIMIZE_MEMORY == 0)
   while (isCell(x)) {
      if (!(num(cdr(x)) & 1))
         return;
      *(long*)&cdr(x) &= ~1;
      mark(car(x)),  x = cdr(x);
   }
   if (!isNum(x)  &&  num(val(x)) & 1) {
      *(long*)&val(x) &= ~1;
      mark(val(x)),  x = tail(x);
#endif
      while (isCell(x)) {
         if (!(num(cdr(x)) & 1))
            return;
         *(long*)&cdr(x) &= ~1;
         mark(cdr(x)),  x = car(x);
      }
      if (!isTxt(x))
         do {
            if (!(num(val(x)) & 1))
               return;
            *(long*)&val(x) &= ~1;
         } while (!isNum(x = val(x)));
#if (PICOLISP_OPTIMIZE_MEMORY == 0)
   }
#endif
}   

#if (PICOLISP_OPTIMIZE_MEMORY == 2)
static void mark(any x) {
   while (isCell(x)) {
      if (!(num(cdr(x)) & 1))
         return;
      *(long*)&cdr(x) &= ~1;
      mark(car(x)),  x = cdr(x);
   }
   if (!isNum(x)  &&  num(val(x)) & 1) {
      *(long*)&val(x) &= ~1;
      mark(val(x));
      markTail(tail(x));
   }
}
#endif

/* Garbage collector */
static void gc(long c) {
   any p;
   heap *h;
   int i;

   h = Heaps;
   do {
      p = h->cells + CELLS-1;
      do
         *(long*)&cdr(p) |= 1;
      while (--p >= h->cells);
   } while (h = h->next);
   /* Mark */
#if (PICOLISP_OPTIMIZE_MEMORY == 2)
   for (i = 0;  i < RAMS;  i += 2) {
      markTail(Ram[i]);
      mark(Ram[i+1]);
   }
#else
   mark(Nil+1);
#endif
   mark(Intern[0]),  mark(Intern[1]);
   mark(Transient[0]), mark(Transient[1]);
   mark(ApplyArgs),  mark(ApplyBody);
#if (PICOLISP_OPTIMIZE_MEMORY == 0)
   mark(Reloc);
#endif
   for (p = Env.stack; p; p = cdr(p))
      mark(car(p));
   for (p = (any)Env.bind;  p;  p = (any)((bindFrame*)p)->link)
      for (i = ((bindFrame*)p)->cnt;  --i >= 0;) {
         mark(((bindFrame*)p)->bnd[i].sym);
         mark(((bindFrame*)p)->bnd[i].val);
      }
   for (p = (any)CatchPtr; p; p = (any)((catchFrame*)p)->link) {
      if (((catchFrame*)p)->tag)
         mark(((catchFrame*)p)->tag);
      mark(((catchFrame*)p)->fin);
   }
   /* Sweep */
   Avail = NULL;
   h = Heaps;
   if (c) {
      do {
         p = h->cells + CELLS-1;
         do
            if (num(p->cdr) & 1)
               Free(p),  --c;
         while (--p >= h->cells);
      } while (h = h->next);
      while (c >= 0)
         heapAlloc(),  c -= CELLS;
   }
   else {
      heap **hp = &Heaps;
      cell *av;

      do {
         c = CELLS;
         av = Avail;
         p = h->cells + CELLS-1;
         do
            if (num(p->cdr) & 1)
               Free(p),  --c;
         while (--p >= h->cells);
         if (c)
            hp = &h->next,  h = h->next;
         else
            Avail = av,  h = h->next,  free(*hp),  *hp = h;
      } while (h);
   }
}

// (gc ['num]) -> num | NIL
any doGc(any x) {
   x = cdr(x),  x = EVAL(car(x));
   val(At) = val(At2) = Nil;
   gc(isNum(x)? CELLS*unBox(x) : CELLS);
   return x;
}

/* Construct a cell */
any cons(any x, any y) {
   cell *p;

   if (!(p = Avail)) {
      cell c1, c2;

      Push(c1,x);
      Push(c2,y);
      gc(CELLS);
      drop(c1);
      p = Avail;
   }
   Avail = p->car;
   p->car = x;
   p->cdr = y;
   return p;
}

/* Construct a symbol */
any consSym(any val, word w) {
   cell *p;

   if (!(p = Avail)) {
      cell c1;

      if (!val)
         gc(CELLS);
      else {
         Push(c1,val);
         gc(CELLS);
         drop(c1);
      }
      p = Avail;
   }
   Avail = p->car;
   p = symPtr(p);
   val(p) = val ?: p;
   tail(p) = txt(w);
   return p;
}

/* Construct a name cell */
any consName(word w, any n) {
   cell *p;

   if (!(p = Avail)) {
      gc(CELLS);
      p = Avail;
   }
   Avail = p->car;
   p = symPtr(p);
   val(p) = n;
   tail(p) = (any)w;
   return p;
}
