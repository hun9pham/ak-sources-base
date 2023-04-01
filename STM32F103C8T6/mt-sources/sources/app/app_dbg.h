#ifndef __APP_DBG_H
#define __APP_DBG_H

#include <stdio.h>
#include "sys_dbg.h"

#define APP_PRINT_EN		1
#define APP_DBG_EN			1
#define APP_DBG_SIG_EN		1

#if (APP_PRINT_EN == 1)
#define APP_PRINT(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#if (APP_DBG_EN == 1)
#define APP_DBG(tag, fmt, ...)  printf(KRED "[" tag "] " KCYN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define APP_DBG(tag, fmt, ...)
#endif

#if (APP_DBG_SIG_EN == 1)
#define APP_DBG_SIG(fmt, ...)   __LOG__(fmt, "SIG -> ", ##__VA_ARGS__)
#else
#define APP_DBG_SIG(fmt, ...)
#endif


#endif /* __APP_DBG_H */
