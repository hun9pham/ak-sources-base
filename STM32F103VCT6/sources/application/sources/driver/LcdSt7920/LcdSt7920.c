/*
 * File:    LCD_ST7920.c
 *
 * Low-level driver functions for monochrome Graphic LCD module, 128 x 64 pixels.
 *
 * Customized for LCD controller silicon:  ST7920.
 */
#include "LcdSt7920.h"



/*----------------------------------------------------------------------------------
 * Name               :  LCD_WriteCommand()
 * Function           :  Write 8-bit command to LCD controller
 *                       Delay 100us (minimum) before exit.
 * Entry arg(s)       :  cmd
 * Return             :  --
------------------------------------------------------------------------------------*/
static  void  LCD_WriteCommand(uint8_t cmd)
{
    LcdCsHigh();

	LcdSendData(0xF8 + (0 << 1));  // send the SYNC + RS(0)
	LcdSendData(cmd & 0xF0);  // send the higher nibble first
	LcdSendData((cmd << 4) & 0xF0);  // send the lower nibble
	LcdDelayUs(50);

	LcdCsLow();
}


/*----------------------------------------------------------------------------------
 * Name               :  LCD_WriteData()
 * Function           :  Write 8-bit data to LCD controller
 * Entry arg(s)       :  data
 * Return             :  --
------------------------------------------------------------------------------------*/
static  void  LCD_WriteData(uint8_t data)
{
    LcdCsHigh();

	LcdSendData(0xF8 + (1 << 1));  // send the SYNC + RS(1)
	LcdSendData(data & 0xF0);  // send the higher nibble first
	LcdSendData((data << 4) & 0xF0);  // send the lower nibble
	LcdDelayUs(50);
	LcdCsLow();
}


/*----------------------------------------------------------------------------------
 * Name               :  LCD_Init()
 * Function           :  Initialize ST7920 LCD controller in graphics mode.
 * Input              :  none.
 * Return             :  False (0) if ST7920 not detected, else True (1)
------------------------------------------------------------------------------------*/
void  LCD_Init(void)
{
    LcdEnLow();
    LcdCsHigh();

    LcdRstLow();             // LCD controller hard reset
    LcdDelayMs(10);
    LcdRstHigh();
    LcdDelayMs(50);

    LCD_WriteCommand(0x30);    // Function Set: basic
    LCD_WriteCommand(0x30);    // Function Set: basic commands
    LCD_WriteCommand(0x0C);    // LCD on/off control: display ON, no blink
    LCD_WriteCommand(0x30);    // Function Set: basic
    LCD_WriteCommand(0x34);    // Function Set: extended commands
    LCD_WriteCommand(0x36);    // Function Set: select graphic mode
    LcdDelayMs(10);
    
    // Write test data to GDRAM to detect if LCD module is connected...
    LCD_WriteCommand(0x80);    // set vert addr = 0
    LCD_WriteCommand(0x80);    // set horiz addr = 0
    LCD_WriteData(0x69);       // write GDRAM data MSB
    LCD_WriteData(0xC3);       // write GDRAM data LSB
    
    LCD_WriteCommand(0x80);    // set vert addr = 0
    LCD_WriteCommand(0x80);    // set horiz addr = 0
}


/*----------------------------------------------------------------------------------
 * Name               :  LCD_ClearGDRAM()
 *
 * Function           :  Clear entire LCD graphics data RAM.
 *              
 * GDRAM format in ST7920 controller:
 * ```````````````````````````````````
 *       UPPER HALF SCREEN           LOWER HALF SCREEN
 *     ----------------------      ----------------------
 *     Vert Addr   Horiz Addr      Vert Addr   Horiz Addr
 *        0         0 .. 7            0         8 .. 15
 *        1         0 .. 7            1         8 .. 15
 *        2         0 .. 7            2         8 .. 15
 *        3         0 .. 7            3         8 .. 15
 *        .            .              .            .
 *        .            .              .            .
 *       31         0 .. 7           31         8 .. 15
 * 
------------------------------------------------------------------------------------*/
void   LCD_ClearGDRAM(void)
{
    uint8_t    x, y;

    for (y = 0;  y < 64;  y++)
    {
        if (y < 32)
        {
            LCD_WriteCommand(0x80 | y);
            LCD_WriteCommand(0x80);
        }
        else
        {
            LCD_WriteCommand(0x80 | (y - 32));
            LCD_WriteCommand(0x80 + 8);
        }

        for (x = 0;  x < 8;  x++)
        {
            LCD_WriteData(0);
            LCD_WriteData(0);
        }
    }
}


/*----------------------------------------------------------------------------------
 * Function   :  LCD_WriteBlock()
 *
 * Overview   :  Copies a rectangular block of pixels from an external screen
 *               buffer in MCU RAM to the LCD controller GDRAM (graphics data RAM).
 *               In general, the GDRAM pixel format differs from the MCU RAM screen
 *               buffer, so a transformation of some sort needs to be performed.
 *
 * Input      :  scnBuf = address of screen buffer in MCU RAM
 *               x, y = pixel coords of upper LHS of block to be copied
 *               w, h = width and height (pixels) of block to be copied
 *  
 * Return     :  --
------------------------------------------------------------------------------------*/
void   LCD_WriteBlock(uint16_t *scnBuf, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint8_t   vertAddr, horizAddr;    // row and column address to be sent to ST7920
    int     row, col;               // row and column word index for screen buffer
    int     firstCol;               // first (leftmost) column-word in a row to be modified
    int     lastCol;                // last (rightmost) column-word in a row to be modified
    uint16_t  pixelWord;              // row of 16 pixels in column-word to be modified
    uint16_t  *pBuf;

    if (x > 127)  x = 0;            // prevent writing past end-of-row
    if ((x + w) > 128) w = 128 - x;

    firstCol = x / 16;   
    lastCol = (x + w - 1) / 16;

    for (row = y;  row < (y + h) && row < 64;  row++)
    {
        for (col = firstCol;  col <= lastCol && col < 8;  col++)
        {
            pBuf = scnBuf + (row * 8 + col);
            pixelWord = *pBuf;

            if (row < 32)  { vertAddr = row;  horizAddr = col; }  // Top half of screen
            else  { vertAddr = row - 32;  horizAddr = col + 8; }  // Bottom half

            LCD_WriteCommand(0x80 | vertAddr);
            LCD_WriteCommand(0x80 | horizAddr);
            LCD_WriteData(HI_BYTE(pixelWord));
            LCD_WriteData(LO_BYTE(pixelWord));
        }
    }
}


/*----------------------------------------------------------------------------------
 * Name               :  LCD_Test()
 * Function           :  Low-level test of ST7920 graphics write sequence.
 *                       The screen should show a pattern of diagonal lines,
 *                       slope 45 degrees, thickness 2 pixels, spaced 16 pixels apart.
 * Input              :  --
------------------------------------------------------------------------------------*/
void  LCD_Test()
{
    uint8_t   vertAddr, horizAddr;    // row and column address to be sent to ST7920
    int     row, col;               // row and column word index for screen buffer
    uint16_t  pixelWord;

    for (row = 0;  row < 64;  row++)
    {
        if ((row % 16) == 0) pixelWord = 0xC000;

        for (col = 0;  col < 8;  col++)
        {
            if (row < 32)  { vertAddr = row;  horizAddr = col; }  // Top half of screen
            else  { vertAddr = row - 32;  horizAddr = col + 8; }  // Bottom half

            LCD_WriteCommand(0x80 | vertAddr);
            LCD_WriteCommand(0x80 | horizAddr);
            LCD_WriteData(HI_BYTE(pixelWord));
            LCD_WriteData(LO_BYTE(pixelWord));
        }
        
        pixelWord = pixelWord >> 1;
        if (pixelWord == 0x0001) pixelWord = 0x8001;
    }
}

