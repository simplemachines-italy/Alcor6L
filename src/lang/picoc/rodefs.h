/* Read-only tables helper */

#undef PICOC_REG_TYPE
#undef PICOC_RO_TYPE
#undef FUNC
#undef PROTO
#undef NIL_FUNC
#undef NIL_PROTO
#undef REGISTER

#define PICOC_REG_TYPE              struct LibraryFunction

#if (MIN_OPT_LEVEL > 0) && (PICOC_OPTIMIZE_MEMORY >= MIN_OPT_LEVEL)
/* for ro-functions */
#define PICOC_RO_TYPE		    picoc_roentry
#define FUNC                        RO_FUNC
#define PROTO                       RO_PROTO
#define NILFUNC                     RO_NILFUNC
#define NILPROTO                    RO_NILPROTO
/* for ro-variables */
#define STRKEY                      RO_STRKEY
#define NUMKEY                      RO_NUMKEY
#define NILKEY                      RO_NILKEY
#define INT			    RO_INT
#define CHAR			    RO_CHAR
#define FLOAT			    RO_FLOAT
#define STR			    RO_STR
#define NILVAL			    RO_NILVAL
#define REGISTER(header, setup_func, table)
#else
#define FUNC(x)                     x
#define PROTO(x)                    x
#define NILFUNC                     NULL
#define NILPROTO                    NULL
#define REGISTER(header, setup_func, table)\
	IncludeRegister(header, setup_func, table, NULL);
#endif
