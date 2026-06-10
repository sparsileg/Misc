/*  sccsid = "@(#)bsa_Error.h	1.9       22 Dec 1993";
    This file contains the include information for bsa_Error.c.
    This includes a typedef for the error return code type as well as
    all error codes.

    Written By:  Jerel McDonald               Date:  July 14, 1993
    Modified By:                              Date:
*/



#ifndef _bsa_Error_h
#define _bsa_Error_h

#include "bsa_Main.h"
#include "util/uet_structs.h"

typedef long tbsa_Error;                 /* error indicator type     */
typedef long tbsa_ErrorLevel;            /* error severity indicator */

#ifndef _bsa_Error_c
extern tbsa_Error BSA_kErrorNo;   /* the global errno variable              */
                                  /* this variable is set when errors occur */
                                  /* in functions that do not return an     */
                                  /* error status.                          */
#endif /* bsa_Error_c */

/*
***************************
Error codes
***************************
*/

#define bsa_OK (tbsa_Error) 0            /* value to indicate success        */
#define bsa_BAD_FILE (tbsa_Error) 1      /* indicates illegal file           */
#define bsa_BAD_VALUE (tbsa_Error) 2     /* value out of allowable range     */
#define bsa_BAD_WIDGET (tbsa_Error) 3    /* NULL widget                      */
#define bsa_BAD_WIDGET_TYPE (tbsa_Error) 4 /* widget is of the wrong class   */
#define bsa_ERROR (tbsa_Error) 5         /* value to indicate general error  */
#define bsa_MALLOC_ERROR (tbsa_Error) 6  /* value to indicate a malloc error */
#define bsa_NO_DEFINITION (tbsa_Error) 7 /* Definition was not found.        */
#define bsa_NULL_POINTER (tbsa_Error) 8  /* indicates a null pointer         */
#define bsa_OPEN_ERROR (tbsa_Error) 9    /* error in open() system call      */
#define bsa_READ_ERROR (tbsa_Error) 10   /* error in read() system call      */
#define bsa_SYS_ERROR (tbsa_Error) 11    /* system call failed               */
#define bsa_TMS_ERROR (tbsa_Error) 12    /* TMS function failed.             */
#define bsa_WRITE_ERROR (tbsa_Error) 13  /* Error writing to a file.         */
#define bsa_DOUBLE_BACK (tbsa_Error) 14  /* line doubles back on itself      */

/*
*************************
Error severity codes
These are mapped to the error codes defined in util/uet_structs.h by
bsa_ShowError().
*************************
*/

#define bsa_INFORMATION (tbsa_ErrorLevel)1
#define bsa_WARNING (tbsa_ErrorLevel)2
#define bsa_SEVERE (tbsa_ErrorLevel)3
#define bsa_FATAL (tbsa_ErrorLevel)4


#define bsa_MAX_ERROR_LENGTH (long) 500  /* maximum length of error messages */

#endif /* bsa_Error_h */
