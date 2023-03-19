#ifndef __SYS_DBG_H
#define __SYS_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#define SYS_PRINT_EN

#if defined(SYS_PRINT_EN)
#define SYS_PRINT(fmt, ...)    xprintf((const char*)fmt, ##__VA_ARGS__)
#else
#define SYS_PRINT(fmt, ...)
#endif

#define FATAL(s, c)            fatalBoot(s, c)

extern void fatalBoot(const char* s, uint8_t c);

#ifdef __cplusplus
}
#endif

#endif  /* __SYS_DBG_H */
