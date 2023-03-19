//=============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//=============================================================================
// Project   :  SHT3x Sample Code (V1.1)
// File      :  i2c_hal.c (V1.1)
// Author    :  RFU
// Date      :  6-Mai-2015
// Controller:  STM32F100RB
// IDE       :  ?Vision V5.12.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer
//=============================================================================

//-- Includes -----------------------------------------------------------------
#include "i2c_hal.h"

//-- User add includes --------------------------------------------------------
#include "stm32xx_inc.h"

//-- Defines ------------------------------------------------------------------
/* -- adapt the defines for your uC -- */
#define SCL_Pin                     ( GPIO_Pin_2 )
#define SDA_Pin                     ( GPIO_Pin_3 )
#define I2C_Port                    ( GPIOA )

// SDA_Pin ----------------------------
#define SDA_Low()                   GPIO_ResetBits(I2C_Port, SDA_Pin)
#define SDA_Open()                  GPIO_SetBits(I2C_Port, SDA_Pin)
#define SDA_Read                    (I2C_Port->IDR  & SDA_Pin)

/* -- adapt the defines for your uC -- */
// SCL_Pin ----------------------------
#define SCL_Low()                   GPIO_ResetBits(I2C_Port, SDA_Pin)
#define SCL_Open()                  GPIO_SetBits(I2C_Port, SDA_Pin)
#define SCL_Read                    (I2C_Port->IDR  & SCL_Pin)

//-- Static function prototypes -----------------------------------------------
static etError I2c_WaitWhileClockStreching(uint8_t timeout);

//-----------------------------------------------------------------------------
/* -- adapt the init for your uC -- */
void I2c_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SCL_Pin | SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(I2C_Port, &GPIO_InitStructure);

    //-- I2C-bus idle mode SDA_Pin / SCL_Pin released 
    SDA_Open();
    SCL_Open();
}

//-----------------------------------------------------------------------------
void I2c_StartCondition(void) {
    SDA_Open();
    DELAY_Us(1);

    SCL_Open();
    DELAY_Us(1);

    SDA_Low();
    DELAY_Us(10);

    SCL_Low();
    DELAY_Us(10);
}

//-----------------------------------------------------------------------------
void I2c_StopCondition(void) {
    SCL_Low();
    DELAY_Us(1);

    SDA_Low();
    DELAY_Us(1);

    SCL_Open();
    DELAY_Us(10);

    SDA_Open();
    DELAY_Us(10);
}

//-----------------------------------------------------------------------------
etError I2c_WriteByte(uint8_t txByte) {
    etError error = NO_ERROR;
    uint8_t mask;
    for(mask = 0x80; mask > 0; mask >>= 1) {
        if((mask & txByte) == 0) {
            SDA_Low();
        }
        else {
            SDA_Open();
        }

        DELAY_Us(1);
        SCL_Open(); 
        DELAY_Us(5);
        SCL_Low();
        DELAY_Us(1);
    }

    SDA_Open();
    SCL_Open();
    DELAY_Us(1);

    //-- Check ACK from I2C slave
    if(SDA_Read) {
        error = ACK_ERROR;
    }
    SCL_Low();

    DELAY_Us(20);
    return error;
}

//-----------------------------------------------------------------------------
etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout) {
    etError error = NO_ERROR;
    uint8_t mask;
    *rxByte = 0x00;

    SDA_Open();
    for(mask = 0x80; mask > 0; mask >>= 1) {
        SCL_Open();
        DELAY_Us(1);
        error = I2c_WaitWhileClockStreching(timeout);
        DELAY_Us(3);

        if(SDA_Read) {
            *rxByte |= mask;
        }
        SCL_Low();
        DELAY_Us(1);
    }
    if(ack == ACK) {
        SDA_Low();
    }
    else {
        SDA_Open();
    }
    DELAY_Us(1);
    SCL_Open();
    DELAY_Us(5);
    SCL_Low();
    SDA_Open();
    DELAY_Us(20);

    return error;
}

//-----------------------------------------------------------------------------
etError I2c_GeneralCallReset(void) {
    etError error;

    I2c_StartCondition();
    error = I2c_WriteByte(0x00);

    if(error == NO_ERROR) {
        error = I2c_WriteByte(0x06);
    }

    return error;
}

//-----------------------------------------------------------------------------
static etError I2c_WaitWhileClockStreching(uint8_t timeout) {
    etError error = NO_ERROR;

    while(SCL_Read == 0) {
        if(timeout-- == 0) {
            return TIMEOUT_ERROR;
        }

        DELAY_Us(1000);
    }

    return error;
}
