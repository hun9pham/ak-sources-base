#ifndef __LCD_ST7920_H
#define __LCD_ST7920_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "io_cfg.h"
#include "sys_ctl.h"


#define LcdEnHigh()            setLcdEn(1)
#define LcdEnLow()             setLcdEn(0)
#define LcdCsHigh()            setLcdCs(1)
#define LcdCsLow()             setLcdCs(0)
#define LcdRstHigh()           setLcdRst(1)
#define LcdRstLow()            setLcdRst(0)
#define LcdBlackLightHigh()    setLcdBl(1)
#define LcdBlackLightLow()     setLcdBl(0)

#define LcdSendData(d)         sendLcdData(d)

#define LcdDelayUs(t)          delayMicrosecond(t)
#define LcdDelayMs(t)          delayMillisecond(t)


#define HI_BYTE(w)  (((w) >> 8) & 0xFF)   // Extract high-order byte from unsigned word
#define LO_BYTE(w)  ((w) & 0xFF)          // Extract low-order byte from unsigned word

#define LESSER_OF(arg1,arg2)  ((arg1)<=(arg2)?(arg1):(arg2))
#define ARRAY_SIZE(a)  (sizeof(a)/sizeof(a[0]))
#define MIN(x,y)       ((x > y)? y: x)

//  LCD controller low-level functions defined in this driver module,
//  accessible by higher-level modules...
//
extern void    LCD_Init(void);
extern void    LCD_ClearGDRAM();
extern void    LCD_WriteBlock(uint16_t *scnBuf, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
extern void    LCD_Test();

#ifdef __cplusplus
}
#endif

#endif  // LCD_ST7920_DRV_H
