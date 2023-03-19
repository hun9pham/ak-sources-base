
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  _____ ___   _____ 
// |_   _|__ \ / ____|
//   | |    ) | |     
//   | |   / /| |     
//  _| |_ / /_| |____ 
// |_____|____|\_____|
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "sys_ctl.h"
#include "io_cfg.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Portable function marcros
 *  Note: Adapt the init for your microcontroller
 *----------------------------------------------------------------------------*/
#define I2cDelayUs(n)   delayMicroseconds(n)
#define I2cIOInit()     OledI2cPinInit()        

#define SDA_Low()       setOledSda(0)
#define SDA_Open()      setOledSda(1)
#define SDA_Read        IO_OLED_SDA_READ

#define SCL_Low()       setOledScl(0)
#define SCL_Open()      setOledScl(1)
#define SCL_Read        IO_OLED_SCL_READ

/* Typedef -------------------------------------------------------------------*/
typedef enum {
    ACK  = 0,
    NACK = 1,
} I2cAck_t;

typedef enum {
    NO_ERROR       = 0x00, /* No error */
    ACK_ERROR      = 0x01, /* No acknowledgment error */
    CHECKSUM_ERROR = 0x02, /* Checksum mismatch error */
    TIMEOUT_ERROR  = 0x04, /* Timeout error */
    PARM_ERROR     = 0x80, /* Parameter out of range error */
} I2cError_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void SoftI2c_Init(void);
extern void SoftI2c_StartCondition(void);
extern void SoftI2c_StopCondition(void);
extern I2cError_t SoftI2c_WriteByte(uint8_t txByte);
extern I2cError_t SoftI2c_WriteMulti(uint8_t *txByte, uint8_t nbrByte);
extern I2cError_t SoftI2c_ReadByte(uint8_t *rxByte, I2cAck_t ack, uint8_t timeout);
extern I2cError_t SoftI2c_GeneralCallReset(void);


#ifdef __cplusplus
}
#endif

#endif /* __SOFT_I2C_H */
