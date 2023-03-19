//=============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//=============================================================================
// Project   :  SHT3x Sample Code (V1.1)
// File      :  i2c_hal.h (V1.1)
// Author    :  RFU
// Date      :  6-Mai-2015
// Controller:  STM32F100RB
// IDE       :  �Vision V5.12.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer 
//=============================================================================

#ifndef __I2C_HAL_H
#define __I2C_HAL_H

//-- Includes -----------------------------------------------------------------
#include <stdint.h>
#include "sys_ctl.h"

//-- Macro function------------------------------------------------------------
#define DELAY_Us(n)             delayMicros(n)

//-- Enumerations -------------------------------------------------------------

// I2C acknowledge
typedef enum {
    ACK  = 0,
    NACK = 1,
} etI2cAck;

//-- Error codes -------------------------------------------------------------
typedef enum {
    NO_ERROR       = 0x00, /* no error */
    ACK_ERROR      = 0x01, /* no acknowledgment error */
    CHECKSUM_ERROR = 0x02, /* checksum mismatch error */
    TIMEOUT_ERROR  = 0x04, /* timeout error */
    PARM_ERROR     = 0x80, /* parameter out of range error */
} etError;

//=============================================================================
void I2c_Init(void);
//=============================================================================
// Initializes the ports for I2C interface.
//-----------------------------------------------------------------------------

//=============================================================================
void I2c_StartCondition(void);
//=============================================================================
// Writes a start condition on I2C-Bus.
//-----------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//       _____
// SDA:       |_____
//       _______
// SCL:         |___

//=============================================================================
void I2c_StopCondition(void);
//=============================================================================
// Writes a stop condition on I2C-Bus.
//-----------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//              _____
// SDA:   _____|
//            _______
// SCL:   ___|

//=============================================================================
etError I2c_WriteByte(uint8_t txByte);
//=============================================================================
// Writes a byte to I2C-Bus and checks acknowledge.
//-----------------------------------------------------------------------------
// input:  txByte       transmit byte
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error
//
// remark: Timing (delay) may have to be changed for different microcontroller.

//=============================================================================
etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout);

etError I2c_GeneralCallReset(void);

#endif /* __I2C_HAL_H */
