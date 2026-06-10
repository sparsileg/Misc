/*   sccsid = "@(#)bsa_Main.h	1.6         27 Dec 1993;
     This file contains the general include information for the TAMPS
     strip chart generator.

     Written by: Jerel McDonald            Date: 13 July, 1993
     Modified by:                          Date:
*/

#ifndef _bsa_Main_h
#define _bsa_Main_h

/*
**************************
 Compile time directives
 Included here instead of
 in make files to allow
 easy integration into TRW
 build environment
**************************
*/

#ifdef SOLARIS
#define bsa_USE_SIGSET   /* use sigset() instead of signal() */
#endif

#ifdef SUNOS
#define bsa_USE_ON_EXIT       /* use on_exit() instead of atexit() */
#endif

#ifdef SGI                            /* Silicon Graphics machine */
#define bsa_NO_XT_SET_LANGUAGE_PROC   /* don't use XtSetLanguageProc() */
#endif

/* Define boolean type */

typedef unsigned char tbsa_Logical;

/* Define standard symbols */

#define bsa_TRUE   (tbsa_Logical) 1     /* TAMPS value for true */
#define bsa_FALSE  (tbsa_Logical) 0     /* TAMPS value for false */

/*  Define PUBLIC, PROTECTED, and PRIVATE function types */

#ifndef PUBLIC
#define PUBLIC
#endif

#ifndef PRIVATE
#define PRIVATE static
#endif

#ifndef PROTECTED
#define PROTECTED
#endif

/*  On some flavors of Unix, MIN and MAX are incorrect.
    We will define TAMPS versions of these macros which are correct. */

#define bsa_MIN(A,B)  ((A) < (B) ? (A) : (B))
#define bsa_MAX(A,B)  ((A) > (B) ? (A) : (B))

/*  Define a square macro */

#define bsa_SQR(X) ((X)*(X))

/*  Define a free macro for Tamps */

#define bsa_FREE(Pointer) \
   if ((Pointer) != NULL) \
   { \
     free(Pointer); \
     Pointer = NULL; \
   }



#endif  /* no code after this point */
