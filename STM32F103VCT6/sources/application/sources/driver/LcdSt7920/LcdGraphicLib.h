/*
 * File: LCD_graphics_lib.h
 *
 * API Library definitions for Monochrome Graphic LCD module, 128 x 64 pixels.
 */
 
#ifndef __LCD_GRAPHICS_LIB_H
#define __LCD_GRAPHICS_LIB_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "LcdSt7920.h"

// Rendering modes for LCD write functions...
#define CLEAR_PIXELS          0
#define SET_PIXELS            1
#define FLIP_PIXELS           2

// Character font styles;  size is cell height in pixels.
// Use one of the font names defined here as the arg value in function: LCD_SetFont(arg).
// Note:  Font size 16 is monospace only -- N/A in proportional spacing.
//        Font sizes 12 and 24 use proportional spacing -- N/A in monospace.
//
enum  Graphics_character_fonts
{
    MONO_8_NORM = 0,   // (0) Mono-spaced font;  char width is 5 pix
    MONO_8_BOLD_X,     // (1) N/A 
    PROP_8_NORM,       // (2) Proportional font;  char width is 3..5 pix
    PROP_8_BOLD_X,     // (3) N/A 

    MONO_12_NORM_X,    // (4) N/A
    MONO_12_BOLD_X,    // (5) N/A
    PROP_12_NORM,      // (6) Proportional font;  char width is 4..7 pix
    PROP_12_BOLD,      // (7) as above, but bold weight

    MONO_16_NORM,      // (8) Mono-spaced font;  char width is 10 pix
    MONO_16_BOLD,      // (9) as above, but bold weight
    PROP_16_NORM_X,    // (10) N/A
    PROP_16_BOLD_X,    // (10) N/A

    MONO_24_NORM_X,    // (12) N/A
    MONO_24_BOLD_X,    // (13) N/A
    PROP_24_NORM,      // (14) Proportional font;  char width is 7..15 pix
    PROP_24_BOLD_X     // (15) N/A  (already bold!)
};


typedef  const unsigned char  bitmap_t;  // Bitmap image is an array of bytes in flash


//---------- Controller low-level functions, defined in driver module -------------------
//
extern  void  LCD_Init(void);      // LCD controller initialisation
extern  void  LCD_ClearGDRAM();
extern  void  LCD_WriteBlock(uint16_t *scnBuf, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

//---------- LCD function & macro library (API) -----------------------------------------
//
#define LCD_GetMaxX()       (127)
#define LCD_GetMaxY()       (63)

extern void    LCD_ClearScreen(void);            // Clear LCD GDRAM and MCU RAM buffers
extern void    LCD_Mode(uint8_t mode);             // Set pixel write mode (set, clear, flip)
extern void    LCD_PosXY(uint16_t x, uint16_t y);    // Set graphics cursor position to (x, y)
extern uint16_t  LCD_GetX(void);                   // Get graphics cursor pos x-coord
extern uint16_t  LCD_GetY(void);                   // Get graphics cursor pos y-coord
extern void    LCD_SetFont(uint8_t font_ID);       // Set font for char or text display
extern uint8_t   LCD_GetFont();                    // Get current font ID
extern void    LCD_PutChar(char uc);             // Show ASCII char at (x, y)
extern void    LCD_PutText(char *str);           // Show text string at (x, y)
extern void    LCD_PutDigit(uint8_t bDat);         // Show hex/decimal digit value (1 char)
extern void    LCD_PutHexByte(uint8_t bDat);       // Show hexadecimal byte value (2 chars)

extern void    LCD_PutDecimalWord(uint16_t val, uint8_t fieldSize);  // Show uint16_t in decimal
extern void    LCD_BlockFill(uint16_t w, uint16_t h);   // Fill area, w x h pixels, at cursor (x, y)
extern uint8_t   LCD_PutImage(bitmap_t *image, uint16_t w, uint16_t h);  // Show bitmap image at (x, y)
extern uint16_t *LCD_ScreenCapture();              // Return a pointer to the screen buffer

// These macros draw various objects at the current graphics cursor position...
#define LCD_PutPixel()           LCD_BlockFill(1, 1)
#define LCD_DrawBar(w, h)        LCD_BlockFill(w, h)
#define LCD_DrawLineHoriz(len)   LCD_BlockFill(len, 1)
#define LCD_DrawLineVert(len)    LCD_BlockFill(1, len)


//---------- Aliases for OLED (or other 128 x 64 pixel display) ----------------------------
//
#define Disp_GetMaxX()      (127)                   // Screen width, pixels
#define Disp_GetMaxY()      (63)                    // Screen height, pixels
#define Disp_Init()         LCD_Init()              // Controller initialisation
#define Disp_ClearScreen()  LCD_ClearScreen()       // Clear GDRAM and MCU RAM buffers
#define Disp_Mode(mode)     LCD_Mode(mode)          // Set pixel write mode (set, clear, flip)
#define Disp_PosXY(x, y)    LCD_PosXY(x, y)         // Set graphics cursor position
#define Disp_GetX()         LCD_GetX()              // Get cursor pos'n x-coord
#define Disp_GetY()         LCD_GetY()              // Get cursor pos'n y-coord
#define Disp_SetFont(font)  LCD_SetFont(font)       // Set font for text
#define Disp_GetFont()      LCD_GetFont()           // Get current font ID
#define Disp_PutChar(c)     LCD_PutChar(c)          // Show ASCII char at (x, y)
#define Disp_PutText(s)     LCD_PutText(s)          // Show text string at (x, y)
#define Disp_PutDigit(d)    LCD_PutDigit(d)         // Show hex/decimal digit (1 char)
#define Disp_PutHexByte(h)  LCD_PutHexByte(h)       // Show hexadecimal byte (2 chars)

#define Disp_PutDecimal(w, n)     LCD_PutDecimalWord(w, n)  // Show uint16_t in decimal (n places)
#define Disp_BlockFill(w, h)      LCD_BlockFill(w, h)       // Fill area w x h pixels at (x, y)
#define Disp_PutImage(img, w, h)  LCD_PutImage(img, w, h)   // Show bitmap image at (x, y)
#define Disp_ScreenCapture()      LCD_ScreenCapture()       // Return a pointer to the screen buffer

#define Disp_PutPixel()           LCD_BlockFill(1, 1)
#define Disp_DrawBar(w, h)        LCD_BlockFill(w, h)
#define Disp_DrawLineHoriz(len)   LCD_BlockFill(len, 1)
#define Disp_DrawLineVert(len)    LCD_BlockFill(1, len)

#ifdef __cplusplus
}
#endif

#endif  // LCD_GRAPHICS_LIB_H
