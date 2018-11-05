#include <SDL.h>

#ifndef DEBUG_UAE4ALL_H
#define DEBUG_UAE4ALL_H

#ifdef DEBUG_UAE4ALL

#if defined(__PSP2__) || defined(__SWITCH__)
#include <psp2shell.h>
#define puts psp2shell_print
#define printf psp2shell_print

#define dbg psp2shell_print
#define dbgf psp2shell_print
#else

#ifdef DEBUG_UAE4ALL_FFLUSH
#define UAE4ALL_FFLUSH fflush(stdout);
#else
#define UAE4ALL_FFLUSH
#endif

extern int DEBUG_AHORA;

#define dbg(TEXTO) \
	if (DEBUG_AHORA) \
	{ \
		puts(TEXTO); \
		UAE4ALL_FFLUSH \
	}


#define dbgf(FORMATO, RESTO...) \
	if (DEBUG_AHORA) \
	{ \
		printf (FORMATO , ## RESTO); \
		UAE4ALL_FFLUSH \
	}


static __inline__ void dbgsum(char *str, void *buff, unsigned len)
{
	if (DEBUG_AHORA)
	{
		unsigned char *p=(unsigned char *)buff;
		unsigned i,ret=0;
		for(i=0;i<len;i++,p++)
			ret+=(*p)*i;
		printf("%s : 0x%X\n",str,ret);
#ifdef DEBUG_UAE4ALL_FFLUSH
		fflush(stdout);
#endif
	}
}

#endif // __PSP2__
#else

#define dbg(TEXTO)
#define dbgf(FORMATO, RESTO...)
#define debsum(STR)

#endif

#define uae4all_prof_start(A)
#define uae4all_prof_end(A)

#endif


