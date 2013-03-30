
/* read only tables for picoc */

#include <string.h>
#include "rotable.h"
#include "interpreter.h"

/* Local defines */
#define PICOCR_FINDFUNCTION     0
#define PICOCR_FINDVALUE        1

/* Externally defined read-only table array */
extern const picoc_rt picoc_rotable[];

/* Find a global "read only table" in the constant picoc_rotable array */
void* picoc_findglobal(const char *name, unsigned len) {
  unsigned i;    
  
  if (strlen(name) > PICOC_MAX_ROTABLE_NAME)
    return NULL;
  for (i=0; picoc_rotable[i].name; i ++)
    if (*picoc_rotable[i].name != '\0' && strlen(picoc_rotable[i].name) == len && !strncmp(picoc_rotable[i].name, name, len)) {
      return (void*)(picoc_rotable[i].pentries);
    }
  return NULL;
}

/* Find an entry in a rotable and return it */
const struct Value* picoc_auxfind(const picoc_roentry *pentry, const char *strkey, picoc_numkey numkey, unsigned *ppos) {
  const struct Value *res = NULL;
  unsigned i = 0;
  
  if (pentry == NULL)
    return NULL;  
  while(pentry->k.type != PICOC_RO_KEY_NIL) {
    if ((strkey && (pentry->k.type == PICOC_RO_KEY_STR) && (!strcmp(pentry->k.id.strkey, strkey))) || 
        (!strkey && (pentry->k.type == PICOC_RO_KEY_NUM) && ((picoc_numkey)pentry->k.id.numkey == numkey))) {
      res = &pentry->v;
      break;
    }
    i ++; pentry ++;
  }
  if (res && ppos)
    *ppos = i;   
  return res;
}

