#ifndef __SYS_LOG_H
#define __SYS_LOG_H

#define SYS_PRINT_EN
#define SYS_LOG_EN

#define SYS_PRINT			    xprintf

#if defined(SYS_LOG_EN)
#define SYS_LOG(tag, fmt, ...)  SYS_PRINT(KCYN "[" tag "] " KYEL fmt  KNRM "\r\n", ##__VA_ARGS__)
#else
#define SYS_LOG(tag, fmt, ...)
#endif

#endif /* __SYS_LOG_H */
