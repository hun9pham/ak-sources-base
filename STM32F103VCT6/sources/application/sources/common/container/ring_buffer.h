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
	uint16_t tailId;
	uint16_t headId;
	uint16_t fillSize;
	uint16_t bufSize;
	uint16_t elementSize;
	uint8_t* pBuf;
} ringBuffer_t;

typedef struct {
	uint16_t tailId;
	uint16_t headId;
	uint16_t fillSize;
	uint16_t bufSize;
	uint8_t* pBuf;
} ringBufferChar_t;

/* Function prototypes -------------------------------------------------------*/
extern void	ringBufferInit(ringBuffer_t* pRingBuf, void* pBuf, uint16_t bufSize, uint16_t elementSize);
extern uint16_t	ringBufferAvailable(ringBuffer_t* pRingBuf);
extern bool isRingBufferEmpty(ringBuffer_t* pRingBuf);
extern bool	isRingBufferFull(ringBuffer_t* pRingBuf);
extern uint8_t ringBufferPut(ringBuffer_t* pRingBuf, void* pData);
extern uint8_t ringBufferGet(ringBuffer_t* pRingBuf, void* pData);

extern void	ringBufferCharInit(ringBufferChar_t* pRingBuf, void* pBuf, uint16_t bufSize);
extern uint16_t	ringBufferCharAvailable(ringBufferChar_t* pRingBuf);
extern bool	isRingBufferCharEmpty(ringBufferChar_t* pRingBuf);
extern bool	isRingBufferCharFull(ringBufferChar_t* pRingBuf);
extern void	ringBufferCharPut(ringBufferChar_t* pRingBuf, uint8_t ch);
extern uint8_t ringBufferCharGet(ringBufferChar_t* pRingBuf);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUFFER_H__ */
