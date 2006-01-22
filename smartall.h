/*

        Definitions for the smart memory allocator

*/

#ifndef SMARTALL_H
#define SMARTALL_H

#ifdef SMARTALLOC

extern void *sm_malloc(char *fname, int lineno, size_t nbytes);
extern void *sm_calloc(char *fname, int lineno, size_t nelem, size_t elsize);
extern void *sm_realloc(char *fname, int lineno, void *ptr, size_t size);
extern void *sm_strdup(char *fname, int lineno, const char *s);
extern void *actuallymalloc(size_t size);
extern void *actuallycalloc(size_t nelem, size_t elsize);
extern void *actuallyrealloc(void *ptr, size_t size);
extern char *actuallystrdup(const char *s);

extern void sm_free(void *fp);
extern void actuallyfree(void *cp);
extern void sm_dump(int bufdump);
extern void sm_static(int mode);

/* Redefine standard memory allocator calls to use our routines
   instead. */

#define free           sm_free
#define cfree          sm_free
#define malloc(x)      sm_malloc(__FILE__, __LINE__, (x))
#define calloc(n,e)    sm_calloc(__FILE__, __LINE__, (n), (e))
#define realloc(p,x)   sm_realloc(__FILE__, __LINE__, (p), (x))
#define strdup(s)      sm_strdup(__FILE__, __LINE__, (s))

#else

/* If SMARTALLOC is disabled, define its special calls to default to
   the standard routines.  */

#define actuallyfree(x)      free(x)
#define actuallymalloc(x)    malloc(x)
#define actuallycalloc(x,y)  calloc(x,y)
#define actuallyrealloc(x,y) realloc(x,y)
#define actuallystrdup(s)    strdup(s)
#define sm_dump(x)
#define sm_static(x)
#endif

#endif /* SMARTALL_H not defined */
