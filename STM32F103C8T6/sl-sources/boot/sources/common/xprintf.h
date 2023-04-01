#ifndef __XPRINTF_H
#define __XPRINTF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define CR_CRLF								(1)

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\e[1;34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern void (*xfunc_output)(int);

/* Function prototypes -------------------------------------------------------*/
extern void xprintf(const char* fmt, ...);


#ifdef __cplusplus
}
#endif

#endif /* __XPRINTF_H */