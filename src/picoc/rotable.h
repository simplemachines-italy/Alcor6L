
/* read only tables for picoc */

#ifndef ROTABLE_H
#define ROTABLE_H

#include "interpreter.h"

/* key types for rotables */
#define PICOC_RO_KEY_STR  0
#define PICOC_RO_KEY_NUM  1
#define PICOC_RO_KEY_NIL  2

/* Macros one can use to define rotable entries */
#define RO_FUNC(v)   .Func = v
#define RO_PROTO(v)  .Prototype = v
#define RO_NILFUNC   .Func = NULL
#define RO_NILPROTO  .Prototype = NULL

/* Macros one can use to define rotable entries */
#define RO_INT(v)\
	{.Typ = &IntType, .Val = (union AnyValue *)&v, .LValueFrom = NULL, .ValOnHeap = FALSE, .ValOnStack = TRUE, .IsLValue = FALSE}

#define RO_CHAR(v)\
	{.Typ = &CharType, .Val = (union AnyValue *)&v, .LValueFrom = NULL, .ValOnHeap = FALSE, .ValOnStack = TRUE, .IsLValue = FALSE}

/**
 * To use FP, declate your platform variables
 * as double. Ex: const double PI = 3.14
 */
#define RO_FLOAT(v)\
	{.Typ = &FPType, .Val = (union AnyValue *)&v, .LValueFrom = NULL, .ValOnHeap = FALSE, .ValOnStack = TRUE, .IsLValue = FALSE}

#define RO_STR(v)\
	{.Typ = &CharPtrType, .Val = (union AnyValue *)&v, .LValueFrom = NULL, .ValOnHeap = FALSE, .ValOnStack = TRUE, .IsLValue = FALSE}

#define RO_NILVAL\
	{.Typ = NULL, .Val = NULL, .LValueFrom = NULL, .ValOnHeap = FALSE, .ValOnStack = FALSE, .IsLValue = FALSE}

#define RO_STRKEY(k)   {PICOC_RO_KEY_STR, {.strkey = k}}
#define RO_NUMKEY(k)   {PICOC_RO_KEY_NUM, {.numkey = k}}
#define RO_NILKEY      {PICOC_RO_KEY_NIL, {.strkey = NULL}}

/* Maximum length of a rotable name and of a string key */
#define PICOC_MAX_ROTABLE_NAME      32

/* Type of a numeric key in a rotable */
typedef int picoc_numkey;

/* The next structure defines the type of a key */
typedef struct {
	int type;
  	union {
    		const char* strkey;
    		picoc_numkey numkey;
  	} id;
} picoc_key;

/* An entry in the read only table */
typedef struct {
	const picoc_key k;
  	const struct Value v;
} picoc_roentry;

/* A rotable */
typedef struct {
	const char *name;
  	const picoc_roentry *pentries;
} picoc_rt;

void* picoc_findglobal(const char *key, unsigned len);
const struct Value* picoc_auxfind(const picoc_roentry *pentry, const char *strkey, picoc_numkey numkey, unsigned *ppos);

#endif /* ROTABLE_H */

