/*------------------------------------------------------------------------/
/  Common libraries
/-------------------------------------------------------------------------/
/ @author: ThanNT
/ @date: 13/08/2016
/-------------------------------------------------------------------------*/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define RET_RING_BUFFER_OK				(0x01)
#define RET_RING_BUFFER_NG				(0x00)

/* Typedef -------------------------------------------------------------------*/
typedef struct {
	uint16_t Tail_index;
	uint16_t Head_index;
	uint16_t Fill_Size;
	uint16_t Buffer_Size;
	uint16_t Element_Size;
	uint8_t* pBuf;
} Ring_Buffer_t;

typedef struct {
	uint16_t Tail_index;
	uint16_t Head_index;
	uint16_t Fill_Size;
	uint16_t Buffer_Size;
	uint8_t* pBuf;
} Ring_Buffer_Char_t;

/* Function prototypes -------------------------------------------------------*/
extern void		Ring_Buffer_Init(Ring_Buffer_t* pRing_Buf, void* pBuf, uint16_t Buffer_Size, uint16_t Element_Size);
extern uint16_t	Ring_Buffer_Available(Ring_Buffer_t* pRing_Buf);
extern bool		Ring_Buffer_isEmpty(Ring_Buffer_t* pRing_Buf);
extern bool		Ring_Buffer_isFull(Ring_Buffer_t* pRing_Buf);
extern uint8_t	Ring_Buffer_Put(Ring_Buffer_t* pRing_Buf, void* pData);
extern uint8_t	Ring_Buffer_Get(Ring_Buffer_t* pRing_Buf, void* pData);

extern void		Ring_Buffer_Char_Init(Ring_Buffer_Char_t* pRing_Buf, void* pBuf, uint16_t Buffer_Size);
extern uint16_t	Ring_Buffer_Char_Available(Ring_Buffer_Char_t* pRing_Buf);
extern bool		Ring_Buffer_Char_isEmpty(Ring_Buffer_Char_t* pRing_Buf);
extern bool		Ring_Buffer_Char_isFull(Ring_Buffer_Char_t* pRing_Buf);
extern void		Ring_Buffer_Char_Put(Ring_Buffer_Char_t* pRing_Buf, uint8_t ch);
extern uint8_t	Ring_Buffer_Char_Get(Ring_Buffer_Char_t* pRing_Buf);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUFFER_H__ */
