/*------------------------------------------------------------------------/
/  Common libraries
/-------------------------------------------------------------------------/
/ @author: ThanNT
/ @date: 13/08/2016
/-------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdlib.h>

#include "ring_buffer.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void ringBufferInit(ringBuffer_t* pRingBuf, void* pBuf, uint16_t bufSize, uint16_t elementSize) {
	pRingBuf->tailId = 0;
	pRingBuf->headId = 0;
	pRingBuf->fillSize = 0;

	pRingBuf->bufSize = bufSize;
	pRingBuf->pBuf = pBuf;
	pRingBuf->elementSize = elementSize;
}

/*----------------------------------------------------------------------------*/
uint16_t ringBufferAvailable(ringBuffer_t* pRingBuf) {
	return pRingBuf->fillSize;
}

/*----------------------------------------------------------------------------*/
bool isRingBufferEmpty(ringBuffer_t* pRingBuf) {
	return (pRingBuf->fillSize == 0) ? true : false;
}

/*----------------------------------------------------------------------------*/
bool isRingBufferFull(ringBuffer_t* pRingBuf) {
	return (pRingBuf->fillSize == pRingBuf->bufSize) ? true : false;
}

/*----------------------------------------------------------------------------*/
uint8_t ringBufferPut(ringBuffer_t* pRingBuf, void* pData) {
	uint16_t Next_Tail_Index;
	uint16_t Next_Head_Index;

	if (pData != NULL) {
		memcpy((uint8_t*)(pRingBuf->pBuf + pRingBuf->tailId * pRingBuf->elementSize), (uint8_t*)pData, pRingBuf->elementSize);

		Next_Tail_Index = (++pRingBuf->tailId) % pRingBuf->bufSize;
		pRingBuf->tailId = Next_Tail_Index;

		if (pRingBuf->fillSize == pRingBuf->bufSize) {
			Next_Head_Index = (++pRingBuf->headId) % pRingBuf->bufSize;
			pRingBuf->headId = Next_Head_Index;
		}
		else {
			pRingBuf->fillSize++;
		}
	}
	else {
		return RET_RING_BUFFER_NG;
	}

	return RET_RING_BUFFER_OK;
}

/*----------------------------------------------------------------------------*/
uint8_t ringBufferGet(ringBuffer_t* pRingBuf, void* pData) {
	uint16_t Next_Head_Index;

	if (isRingBufferEmpty(pRingBuf)) {
		return RET_RING_BUFFER_NG;
	}

	if (pData != NULL) {
		memcpy((uint8_t*)pData, (uint8_t*)(pRingBuf->pBuf + pRingBuf->headId * pRingBuf->elementSize), pRingBuf->elementSize);

		Next_Head_Index = (++pRingBuf->headId) % pRingBuf->bufSize;
		pRingBuf->headId = Next_Head_Index;

		pRingBuf->fillSize--;
	}
	else {
		return RET_RING_BUFFER_NG;
	}

	return RET_RING_BUFFER_OK;
}

/*----------------------------------------------------------------------------*/
void ringBufferCharInit(ringBufferChar_t* pRingBuf, void* pBuf, uint16_t bufSize) {
	pRingBuf->tailId = 0;
	pRingBuf->headId = 0;
	pRingBuf->fillSize = 0;

	pRingBuf->bufSize = bufSize;
	pRingBuf->pBuf = pBuf;
}

/*----------------------------------------------------------------------------*/
uint16_t ringBufferCharAvailable(ringBufferChar_t* pRingBuf) {
	return pRingBuf->fillSize;
}

/*----------------------------------------------------------------------------*/
bool isRingBufferCharEmpty(ringBufferChar_t* pRingBuf) {
	return (pRingBuf->fillSize == 0) ? true : false;
}

/*----------------------------------------------------------------------------*/
bool isRingBufferCharFull(ringBufferChar_t* pRingBuf) {
	return (pRingBuf->fillSize == pRingBuf->bufSize) ? true : false;
}

/*----------------------------------------------------------------------------*/
void ringBufferCharPut(ringBufferChar_t* pRingBuf, uint8_t ch) {
	uint16_t Next_Tail_Index;
	uint16_t Next_Head_Index;

	pRingBuf->pBuf[pRingBuf->tailId] = ch;

	Next_Tail_Index = (++pRingBuf->tailId) % pRingBuf->bufSize;
	pRingBuf->tailId = Next_Tail_Index;

	if (pRingBuf->fillSize == pRingBuf->bufSize) {
		Next_Head_Index = (++pRingBuf->headId) % pRingBuf->bufSize;
		pRingBuf->headId = Next_Head_Index;
	}
	else {
		pRingBuf->fillSize++;
	}
}

/*----------------------------------------------------------------------------*/
uint8_t	ringBufferCharGet(ringBufferChar_t* pRingBuf) {
	uint16_t ret = 0;
	uint16_t Next_Head_Index;

	if (pRingBuf->fillSize) {
		ret = pRingBuf->pBuf[pRingBuf->headId];

		Next_Head_Index = (++pRingBuf->headId) % pRingBuf->bufSize;
		pRingBuf->headId = Next_Head_Index;

		pRingBuf->fillSize--;
	}

	return ret;
}
