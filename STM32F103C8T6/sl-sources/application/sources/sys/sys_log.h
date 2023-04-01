#ifndef __SYS_LOG_H
#define __SYS_LOG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "xprintf.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define SYS_PRINT_EN
#define SYS_LOG_EN              (1)

#define SYS_PRINT			    xprintf

#if (SYS_LOG_EN == 1)
#define SYS_LOG(tag, fmt, ...)  SYS_PRINT(KCYN "[" tag "] " KYEL fmt  KNRM "\r\n", ##__VA_ARGS__)
#else
#define SYS_LOG(tag, fmt, ...)
#endif

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __SYS_LOG_H */
