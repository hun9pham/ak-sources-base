//=============================================================================
//
//=============================================================================
// Project   :  74HC595.h
// Author    :  HungPNQ
// Date      :  2022-07-22
// Brief     :  Shift register output
//=============================================================================

#ifndef	__HC595_H
#define	__HC595_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define HC595_TOTAL_OUTPUT_GATE     ( 8 )

#define HC595_OK                    ( 1 )
#define HC595_NG                    ( 0 )

#define LSB_FIRST                   ( 0 )   /* Byte_In[0:7] = Byte_Out[0 : 7] */
#define MSB_FIRST                   ( 1 )   /* Byte_In[0:7] = Byte_Out[7 : 0] */


/* Typedef -------------------------------------------------------------------*/
typedef void (*pf_IO_Init)(void);
typedef void (*pf_Ctl_Clk)(uint8_t);
typedef void (*pf_Ctl_Dat)(uint8_t);
typedef void (*pf_Ctl_Lat)(uint8_t);

typedef struct {
    uint8_t Shift_Byte_Data;
    pf_IO_Init Init;
    pf_Ctl_Clk Clk;
    pf_Ctl_Dat Data;
    pf_Ctl_Lat Latch;
} HC595_Ctl_t;

/* Extern variables ----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
extern void     HC595_IO_Setup(HC595_Ctl_t *Obj, pf_IO_Init Init,	pf_Ctl_Clk Clk, pf_Ctl_Dat Data, pf_Ctl_Lat Latch);
extern int8_t   HC595_IO_Shift_Byte(HC595_Ctl_t *Obj, uint8_t byte, uint8_t Opt);
extern void     HC595_IO_Set_Bit(HC595_Ctl_t *Obj, uint8_t bitiDx, uint8_t valBit);
extern uint8_t  HC595_IO_Read_Byte(HC595_Ctl_t *Obj);
extern uint8_t  HC595_IO_Read_Bit(HC595_Ctl_t *Obj, uint8_t bitiDx);

#ifdef __cplusplus
}
#endif

#endif /* __HC595_H */

