#ifndef __APP_DBG_H
#define __APP_DBG_H

#include "xprintf.h"

#define APP_PRINT_EN        (1)
#define APP_DBG_EN          (0)
#define APP_DBG_SIG_EN      (1)
#define APP_LOG_EN          (1)
#define DBG_LINK_PRINT_EN   (0)

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#if (APP_DBG_EN == 1)
#define APP_DBG(tag, fmt, ...)  xprintf(KRED "[" tag "] " KGRN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define APP_DBG(tag, fmt, ...)
#endif

#if (APP_PRINT_EN == 1)
#define APP_PRINT(fmt, ...) xprintf(fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#if (DBG_LINK_PRINT_EN == 1)
#define DBG_LINK_PRINT(tag, fmt, ...)   xprintf(KCYN "[LINK] " KRED tag " -> " KYEL fmt KNRM, ##__VA_ARGS__)
#else
#define DBG_LINK_PRINT(tag, fmt, ...)
#endif

#if (APP_DBG_SIG_EN == 1)
#define APP_DBG_SIG(tag, fmt, ...)  xprintf(KMAG "[SIG] " KGRN tag " -> " KYEL fmt KNRM, ##__VA_ARGS__)
#else
#define APP_DBG_SIG(tag, fmt, ...)
#endif

#if (APP_LOG_EN == 1)
#define APP_LOG(assertion, fmt, ...)                                \
    do {                                                            \
        if (!(assertion))                                           \
        {                                                           \
            xprintf(KRED "[NG] " fmt KNRM "\r\n", ##__VA_ARGS__);   \
        }                                                           \
        else                                                        \
        {                                                           \
            xprintf(KGRN "[OK] " fmt KNRM "\r\n", ##__VA_ARGS__);   \
        }                                                           \
    } while (0)
                                                          
#else
#define APP_LOG(assert, fmt, ...)
#endif



#endif /* __APP_DBG_H */
