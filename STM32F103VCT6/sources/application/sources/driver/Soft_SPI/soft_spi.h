#ifndef __SOFTSPI_H
#define __SOFTSPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define SPI_MODE_MASK           ( 0x0C )  /* CPOL = bit 3, CPHA = bit 2 on SPCR */
#define SPI_CLOCK_MASK          ( 0x03 )  /* SPR1 = bit 1, SPR0 = bit 0 on SPCR */
#define SPI_2XCLOCK_MASK        ( 0x01 )  /* SPI2X = bit 0 on SPSR              */

/* Typedef -------------------------------------------------------------------*/
typedef void    (*pf_IO_Init)(void);
typedef void    (*pf_Clk)(uint8_t);
typedef void    (*pf_Mosi)(uint8_t);
typedef uint8_t (*pf_Miso)(void);


typedef enum {
    LSB_FIRST = 0,
    MSB_FIRST,
} eBitOrder_t;

typedef enum {
    SPI_MODE0 = 0x00,
    SPI_MODE1 = 0x04,
    SPI_MODE2 = 0x08,
    SPI_MODE3 = 0x0C,
} eDataMode_t;

typedef enum {
    SPI_CLOCK_DIV2 = 0,
    SPI_CLOCK_DIV4,
    SPI_CLOCK_DIV8,
    SPI_CLOCK_DIV16,
    SPI_CLOCK_DIV32,
    SPI_CLOCK_DIV64,
    SPI_CLOCK_DIV128,
} eSpiClkDivision_t;

typedef struct {
    uint8_t DelayTime;
    uint8_t Cpol;
    uint8_t Cpha;
    eBitOrder_t Odrbit;

    pf_IO_Init Init;
    pf_Clk ClkCtl;
    pf_Mosi MosiCtl;
    pf_Miso MisoCtl;
} SoftSPI_t;

/* Extern variables ----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/
extern void SoftSPI_Init(SoftSPI_t *SoftSpi, pf_IO_Init Init, pf_Clk ClkCtl, pf_Mosi MosiCtl, pf_Miso MisoCtl);

extern void SoftSPI_SetBitOrder(SoftSPI_t *SoftSpi, eBitOrder_t Odr);
extern void SoftSPI_DataMode(SoftSPI_t *SoftSpi, eDataMode_t Mode);
extern void SoftSPI_SetClockDivider(SoftSPI_t *SoftSpi, eSpiClkDivision_t Div);
extern uint8_t SoftSPI_Transfer8Bits(SoftSPI_t *SoftSpi, uint8_t txData);
extern uint8_t SoftSPI_Transfer16Bits(SoftSPI_t *SoftSpi, uint8_t txData);


#ifdef __cplusplus
}
#endif

#endif /* __SOFTSPI_H */