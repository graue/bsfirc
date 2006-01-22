/*

        Definitions for error-checking memory allocator

*/

#ifndef ALLOC_H
#define ALLOC_H

#ifdef SMARTALLOC

extern void *sm_xmalloc(size_t size);
extern void *sm_xrealloc(void *ptr, size_t size);
extern char *sm_xstrdup(const char *s);
#define xmalloc(x)      sm_xmalloc( __FILE__, __LINE__, (x))
#define xrealloc(p,x)   sm_xrealloc(__FILE__, __LINE__, (p), (x))
#define xstrdup(s)      sm_xstrdup( __FILE__, __LINE__, (s))

#else

extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern char *xstrdup(const char *s);

#endif

#endif /* ALLOC_H not defined */
