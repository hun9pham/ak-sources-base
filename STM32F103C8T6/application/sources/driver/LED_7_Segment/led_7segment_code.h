//=============================================================================
//
//=============================================================================
// Project   :  led_7segment_code.h
// Author    :  HungPNQ
// Date      :  2022-07-22
// Brief     :  Hex code display number of LED 7 Segment
//=============================================================================

#ifndef __LED_7SEGMENT_CODE_H
#define __LED_7SEGMENT_CODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define NbrOfLetterDisplay     ( 26 )


/*----------------------------------------------------------------------------*
/    LED 7 SEGMENT CODE COMMON CATHODE
/----------------------------------------------------------------------------*
/ order     :  G - F - E - D - C - B - A
/*----------------------------------------------------------------------------*/
#define CC_NUMBER_0             ( 0x3F )
#define CC_NUMBER_1             ( 0x06 )
#define CC_NUMBER_2             ( 0x5B )
#define CC_NUMBER_3             ( 0x4F )
#define CC_NUMBER_4             ( 0x66 )
#define CC_NUMBER_5             ( 0x6D )
#define CC_NUMBER_6             ( 0x7D )
#define CC_NUMBER_7             ( 0x07 )
#define CC_NUMBER_8             ( 0x7F )
#define CC_NUMBER_9             ( 0x6F )

#define CC_LETTER_A             ( 0x77 )
#define CC_LETTER_b             ( 0x7C )
#define CC_LETTER_C             ( 0x39 )
#define CC_LETTER_d             ( 0x5E )
#define CC_LETTER_E             ( 0x79 )
#define CC_LETTER_F             ( 0x71 )
#define CC_LETTER_H             ( 0x76 )
#define CC_LETTER_J             ( 0x1E )
#define CC_LETTER_L             ( 0x38 )
#define CC_LETTER_n             ( 0x54 )
#define CC_LETTER_U             ( 0x3E )
#define CC_LETTER_u             ( 0x1C )
#define CC_LETTER_Y             ( 0x6E )
#define CC_LETTER_o             ( 0x5C )
#define CC_LETTER_i             ( 0x10 )
#define CC_LETTER_q             ( 0x67 )

#define CC_DOT_EN(x)            ( x & (0x80))

/*----------------------------------------------------------------------------*
/    LED 7 SEGMENT CODE COMMON ANODE
/----------------------------------------------------------------------------*
/ order     :  G - F - E - D - C - B - A
/*----------------------------------------------------------------------------*/
#define CA_NUMBER_0             ( ~(CC_NUMBER_0) )
#define CA_NUMBER_1             ( ~(CC_NUMBER_1) )
#define CA_NUMBER_2             ( ~(CC_NUMBER_2) )
#define CA_NUMBER_3             ( ~(CC_NUMBER_3) ) /* 10110000 */
#define CA_NUMBER_4             ( ~(CC_NUMBER_4) )
#define CA_NUMBER_5             ( ~(CC_NUMBER_5) )
#define CA_NUMBER_6             ( ~(CC_NUMBER_6) )
#define CA_NUMBER_7             ( ~(CC_NUMBER_7) )
#define CA_NUMBER_8             ( ~(CC_NUMBER_8) )
#define CA_NUMBER_9             ( ~(CC_NUMBER_9) )

#define CA_LETTER_A             ( ~(CC_LETTER_A) )
#define CA_LETTER_b             ( ~(CC_LETTER_b) )
#define CA_LETTER_C             ( ~(CC_LETTER_C) )
#define CA_LETTER_d             ( ~(CC_LETTER_d) )
#define CA_LETTER_E             ( ~(CC_LETTER_E) )
#define CA_LETTER_F             ( ~(CC_LETTER_F) )
#define CA_LETTER_H             ( ~(CC_LETTER_H) )
#define CA_LETTER_J             ( ~(CC_LETTER_J) )
#define CA_LETTER_L             ( ~(CC_LETTER_L) )
#define CA_LETTER_n             ( ~(CC_LETTER_n) )
#define CA_LETTER_U             ( ~(CC_LETTER_U) )
#define CA_LETTER_u             ( ~(CC_LETTER_u) )
#define CA_LETTER_Y             ( ~(CC_LETTER_Y) )
#define CA_LETTER_o             ( ~(CC_LETTER_o) )
#define CA_LETTER_i             ( ~(CC_LETTER_i) )
#define CA_LETTER_q             ( ~(CC_LETTER_q) )

#define CA_DOT_EN(x)            ( x & (0x7F))


/* Typedef -------------------------------------------------------------------*/
struct LED_7_SEGMENT {
    const char Ch;
    uint8_t HexCode;
};


/* Extern variables ----------------------------------------------------------*/
extern const struct LED_7_SEGMENT CC_Led_7_Segment_tbl[];
extern const struct LED_7_SEGMENT CA_Led_7_Segment_tbl[];

#ifdef __cplusplus
}
#endif

#endif /* __LED_7SEGMENT_CODE_H */