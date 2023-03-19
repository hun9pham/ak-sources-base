#ifndef __APP_DBG_H
#define __APP_DBG_H

#include "xprintf.h"

#define APP_PRINT_EN
#define APP_DBG_EN
#define APP_DBG_SIG_EN
#define LOG_ERROR_EN
// #define DBG_LINK_PRINT_EN

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#if defined(APP_DBG_EN)
#define APP_DBG(tag, fmt, ...)       xprintf(KRED "[" tag "] " KGRN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define APP_DBG(tag, fmt, ...)
#endif

#if defined(APP_PRINT_EN)
#define APP_PRINT(fmt, ...)     xprintf(fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#if defined(DBG_LINK_PRINT_EN)
#define DBG_LINK_PRINT(tag, fmt, ...)   xprintf(KCYN "[LINK] " KRED tag " -> " KYEL fmt KNRM, ##__VA_ARGS__)
#else
#define DBG_LINK_PRINT(tag, fmt, ...)
#endif

#if defined(APP_DBG_SIG_EN)
#define APP_DBG_SIG(tag, fmt, ...)  xprintf(KMAG "[SIG] " KGRN tag " -> " KYEL fmt KNRM, ##__VA_ARGS__)
#else
#define APP_DBG_SIG(tag, fmt, ...)
#endif

#if defined(LOG_ERROR_EN)
#define LOG_ERR(tag, fmt, ...)  xprintf(KRED "[ERR]" KYEL " " tag " : " KGRN "%d" KYEL "\t" KNRM fmt, __LINE__, ##__VA_ARGS__)
#else
#define LOG_ERR(tag, fmt, ...)
#endif


#endif /* __APP_DBG_H */
