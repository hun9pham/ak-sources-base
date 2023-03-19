#include "SoftSPI.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define Clk_Set(me, Lv)             ((SoftSPI_t *)me)->ClkCtl(Lv)
#define Mosi_Set(me, Lv)            ((SoftSPI_t *)me)->MosiCtl(Lv)


/* Extern variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void NOP_SpendingTimes(uint16_t Delay);

/* Function implementation ---------------------------------------------------*/
void SoftSPI_Init(SoftSPI_t *me, pf_IO_Init Init, pf_Clk ClkCtl, pf_Mosi MosiCtl, pf_Miso MisoCtl) {
    /* Default setting */
    me->DelayTime = 2;
    me->Cpha = 0;
    me->Cpol = 0;
    me->Odrbit = MSB_FIRST;

    /* Inital function control */
    me->Init = Init;
    me->ClkCtl = ClkCtl;
    me->MisoCtl = MisoCtl;
    me->MosiCtl = MosiCtl;

    me->Init();
}

void SoftSPI_SetBitOrder(SoftSPI_t *me, eBitOrder_t Odr) {
    me->Odrbit = Odr;
}


void SoftSPI_DataMode(SoftSPI_t *me, eDataMode_t Mode) {
    switch (Mode) {
        case SPI_MODE0: {
            me->Cpha = 0;
            me->Cpol = 0;
        }
        break;

        case SPI_MODE1: {
            me->Cpha = 1;
            me->Cpol = 0;
        }
        break;

        case SPI_MODE2: {
            me->Cpha = 0;
            me->Cpol = 1;
        }
        break;

        case SPI_MODE3: {
            me->Cpha = 1;
            me->Cpol = 1;
        }
        break;

        default: {
            me->Cpha = 0;
            me->Cpol = 0;
        }
        break;
    }

    Clk_Set(me, me->Cpol);
}

void SoftSPI_SetClockDivider(SoftSPI_t *me, eSpiClkDivision_t Div) {
    switch (Div) {
        case SPI_CLOCK_DIV2: {
            me->DelayTime = 2;
        }
        break;
            
        case SPI_CLOCK_DIV4: {
            me->DelayTime = 4;
        }
        break;

        case SPI_CLOCK_DIV8: {
            me->DelayTime = 8;
        }
        break;

        case SPI_CLOCK_DIV16: {
            me->DelayTime = 16;
        }
        break;

        case SPI_CLOCK_DIV32: {
            me->DelayTime = 32;
        }
        break;

        case SPI_CLOCK_DIV64: {
            me->DelayTime = 64;
        }
        break;

        case SPI_CLOCK_DIV128: {
            me->DelayTime = 128;
        }
        break;

        default: {
            me->DelayTime = 2;
        }
        break;
    }
}

uint8_t SoftSPI_Transfer8Bits(SoftSPI_t *me, uint8_t txData) {
    uint8_t __txData = txData;
    uint8_t rxData = 0;

    if (me->Odrbit == MSB_FIRST) {
        __txData = ((txData & 0x01) << 7) |
                    ((txData & 0x02) << 5) |
                    ((txData & 0x04) << 3) |
                    ((txData & 0x08) << 1) |
                    ((txData & 0x10) >> 1) |
                    ((txData & 0x20) >> 3) |
                    ((txData & 0x40) >> 5) |
                    ((txData & 0x80) >> 7);

    }

    uint8_t del = me->DelayTime >> 1;
    uint8_t bval = 0;
    /*
     * CPOL := 0, CPHA := 0 => INIT = 0, PRE = Z|0, MID = 1, POST =  0
     * CPOL := 1, CPHA := 0 => INIT = 1, PRE = Z|1, MID = 0, POST =  1
     * CPOL := 0, CPHA := 1 => INIT = 0, PRE =  1 , MID = 0, POST = Z|0
     * CPOL := 1, CPHA := 1 => INIT = 1, PRE =  0 , MID = 1, POST = Z|1
     */

    int __Clk = (me->Cpol) ? 1 : 0;

    for (uint8_t bit = 0; bit < 8; ++bit) {
        if (me->Cpha) {
            __Clk ^= 1;
            Clk_Set(me, __Clk);

            NOP_SpendingTimes(del);
        }

        /* ... Write bit */
        Mosi_Set(me, __txData & (1 << bit));
        NOP_SpendingTimes(del);

        __Clk ^= 1; 
        Clk_Set(me, __Clk);

        /* ... Read bit */
        {
            bval = me->MisoCtl();

            if (me->Odrbit == MSB_FIRST) {
                rxData <<= 1;
                rxData |= bval;
            } 
            else {
                rxData >>= 1;
                rxData |= bval << 7;
            }
        }

        NOP_SpendingTimes(del);

        if (!me->Cpha) {
            __Clk ^= 1u;
            Clk_Set(me, __Clk);
        }
    }

    return rxData;
}

uint8_t SoftSPI_Transfer16Bits(SoftSPI_t *me, uint8_t txData) {
    union {
		uint16_t txData;
		struct {
			uint8_t lsb;
			uint8_t msb;
		};
	} in, __txData;
  
	in.txData = txData;

	if ( me->Odrbit == MSB_FIRST ) {
		__txData.msb = SoftSPI_Transfer8Bits(me, in.msb);
		__txData.lsb = SoftSPI_Transfer8Bits(me, in.lsb);
	} 
    else {
		__txData.lsb = SoftSPI_Transfer8Bits(me, in.lsb);
		__txData.msb = SoftSPI_Transfer8Bits(me, in.msb);
	}

	return __txData.txData;
}


void NOP_SpendingTimes(uint16_t Delay) {
    for (uint16_t i = 0; i < Delay; i++) {
   
    }
}