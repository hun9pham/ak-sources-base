#ifndef __APP_DBG_H
#define __APP_DBG_H

#include "xprintf.h"

#define APP_DBG_EN
#define APP_PRINT_EN

#if defined(APP_DBG_EN)
#define APP_DBG(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#if defined(APP_PRINT_EN)
#define APP_PRINT(fmt, ...) xprintf(KBLU fmt KNRM "\r", ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif


#endif /* __APP_DBG_H */
