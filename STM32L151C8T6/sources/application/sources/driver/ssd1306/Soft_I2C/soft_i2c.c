#include "soft_i2c.h"


static I2cError_t SoftI2c_WaitWhileClockStreching(uint8_t timeout);

void SoftI2c_Init(void) {
    I2cIOInit();
    SDA_Open();
    SCL_Open();
}

void SoftI2c_StartCondition(void) {
    SDA_Open();
    I2cDelayUs(1);

    SCL_Open();
    I2cDelayUs(1);

    SDA_Low();
    I2cDelayUs(10);

    SCL_Low();
    I2cDelayUs(10);
}

void SoftI2c_StopCondition(void) {
    SCL_Low();
    I2cDelayUs(1);

    SDA_Low();
    I2cDelayUs(1);

    SCL_Open();
    I2cDelayUs(10);

    SDA_Open();
    I2cDelayUs(10);
}

I2cError_t SoftI2c_WriteByte(uint8_t txByte) {
    I2cError_t error = NO_ERROR;
    uint8_t mask;
    for(mask = 0x80; mask > 0; mask >>= 1) {
        if((mask & txByte) == 0) {
            SDA_Low();
        }
        else {
            SDA_Open();
        }

        I2cDelayUs(1);
        SCL_Open(); 
        I2cDelayUs(5);
        SCL_Low();
        I2cDelayUs(1);
    }

    SDA_Open();
    SCL_Open();
    I2cDelayUs(1);

    SCL_Low();

    I2cDelayUs(20);
    return error;
}

I2cError_t SoftI2c_WriteMulti(uint8_t *txByte, uint8_t nbrByte) {
    I2cError_t error = NO_ERROR;
    uint8_t count = 0;

    for (; count < nbrByte; ++count) {
        error = SoftI2c_WriteByte(txByte[count]);
    }

    return error;
}

I2cError_t SoftI2c_ReadByte(uint8_t *rxByte, I2cAck_t ack, uint8_t timeout) {
    I2cError_t error = NO_ERROR;
    uint8_t mask;
    *rxByte = 0x00;

    SDA_Open();
    for(mask = 0x80; mask > 0; mask >>= 1) {
        SCL_Open();
        I2cDelayUs(1);
        error = SoftI2c_WaitWhileClockStreching(timeout);
        I2cDelayUs(3);

        if(SDA_Read) {
            *rxByte |= mask;
        }
        SCL_Low();
        I2cDelayUs(1);
    }
    if(ack == ACK) {
        SDA_Low();
    }
    else {
        SDA_Open();
    }
    I2cDelayUs(1);
    SCL_Open();
    I2cDelayUs(5);
    SCL_Low();
    SDA_Open();
    I2cDelayUs(20);

    return error;
}

I2cError_t SoftI2c_GeneralCallReset(void) {
    I2cError_t error;

    SoftI2c_StartCondition();
    error = SoftI2c_WriteByte(0x00);

    if(error == NO_ERROR) {
        error = SoftI2c_WriteByte(0x06);
    }

    return error;
}

static I2cError_t SoftI2c_WaitWhileClockStreching(uint8_t timeout) {
    I2cError_t error = NO_ERROR;

    while(SCL_Read == 0) {
        if(timeout-- == 0) {
            return TIMEOUT_ERROR;
        }

        I2cDelayUs(1000);
    }

    return error;
}
