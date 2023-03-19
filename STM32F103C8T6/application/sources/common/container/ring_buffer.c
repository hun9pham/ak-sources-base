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
void Ring_Buffer_Init(Ring_Buffer_t* pRing_Buf, void* pBuf, uint16_t Buffer_Size, uint16_t Element_Size) {
	pRing_Buf->Tail_index = 0;
	pRing_Buf->Head_index = 0;
	pRing_Buf->Fill_Size = 0;

	pRing_Buf->Buffer_Size = Buffer_Size;
	pRing_Buf->pBuf = pBuf;
	pRing_Buf->Element_Size = Element_Size;
}

/*----------------------------------------------------------------------------*/
uint16_t Ring_Buffer_Available(Ring_Buffer_t* pRing_Buf) {
	return pRing_Buf->Fill_Size;
}

/*----------------------------------------------------------------------------*/
bool Ring_Buffer_isEmpty(Ring_Buffer_t* pRing_Buf) {
	return (pRing_Buf->Fill_Size == 0) ? true : false;
}

/*----------------------------------------------------------------------------*/
bool Ring_Buffer_isFull(Ring_Buffer_t* pRing_Buf) {
	return (pRing_Buf->Fill_Size == pRing_Buf->Buffer_Size) ? true : false;
}

/*----------------------------------------------------------------------------*/
uint8_t Ring_Buffer_Put(Ring_Buffer_t* pRing_Buf, void* pData) {
	uint16_t Next_Tail_Index;
	uint16_t Next_Head_Index;

	if (pData != NULL) {
		memcpy((uint8_t*)(pRing_Buf->pBuf + pRing_Buf->Tail_index * pRing_Buf->Element_Size), (uint8_t*)pData, pRing_Buf->Element_Size);

		Next_Tail_Index = (++pRing_Buf->Tail_index) % pRing_Buf->Buffer_Size;
		pRing_Buf->Tail_index = Next_Tail_Index;

		if (pRing_Buf->Fill_Size == pRing_Buf->Buffer_Size) {
			Next_Head_Index = (++pRing_Buf->Head_index) % pRing_Buf->Buffer_Size;
			pRing_Buf->Head_index = Next_Head_Index;
		}
		else {
			pRing_Buf->Fill_Size++;
		}
	}
	else {
		return RET_RING_BUFFER_NG;
	}

	return RET_RING_BUFFER_OK;
}

/*----------------------------------------------------------------------------*/
uint8_t Ring_Buffer_Get(Ring_Buffer_t* pRing_Buf, void* pData) {
	uint16_t Next_Head_Index;

	if (Ring_Buffer_isEmpty(pRing_Buf)) {
		return RET_RING_BUFFER_NG;
	}

	if (pData != NULL) {
		memcpy((uint8_t*)pData, (uint8_t*)(pRing_Buf->pBuf + pRing_Buf->Head_index * pRing_Buf->Element_Size), pRing_Buf->Element_Size);

		Next_Head_Index = (++pRing_Buf->Head_index) % pRing_Buf->Buffer_Size;
		pRing_Buf->Head_index = Next_Head_Index;

		pRing_Buf->Fill_Size--;
	}
	else {
		return RET_RING_BUFFER_NG;
	}

	return RET_RING_BUFFER_OK;
}

/*----------------------------------------------------------------------------*/
void Ring_Buffer_Char_Init(Ring_Buffer_Char_t* pRing_Buf, void* pBuf, uint16_t Buffer_Size) {
	pRing_Buf->Tail_index = 0;
	pRing_Buf->Head_index = 0;
	pRing_Buf->Fill_Size = 0;

	pRing_Buf->Buffer_Size = Buffer_Size;
	pRing_Buf->pBuf = pBuf;
}

/*----------------------------------------------------------------------------*/
uint16_t Ring_Buffer_Char_Available(Ring_Buffer_Char_t* pRing_Buf) {
	return pRing_Buf->Fill_Size;
}

/*----------------------------------------------------------------------------*/
bool Ring_Buffer_Char_isEmpty(Ring_Buffer_Char_t* pRing_Buf) {
	return (pRing_Buf->Fill_Size == 0) ? true : false;
}

/*----------------------------------------------------------------------------*/
bool Ring_Buffer_Char_isFull(Ring_Buffer_Char_t* pRing_Buf) {
	return (pRing_Buf->Fill_Size == pRing_Buf->Buffer_Size) ? true : false;
}

/*----------------------------------------------------------------------------*/
void Ring_Buffer_Char_Put(Ring_Buffer_Char_t* pRing_Buf, uint8_t ch) {
	uint16_t Next_Tail_Index;
	uint16_t Next_Head_Index;

	pRing_Buf->pBuf[pRing_Buf->Tail_index] = ch;

	Next_Tail_Index = (++pRing_Buf->Tail_index) % pRing_Buf->Buffer_Size;
	pRing_Buf->Tail_index = Next_Tail_Index;

	if (pRing_Buf->Fill_Size == pRing_Buf->Buffer_Size) {
		Next_Head_Index = (++pRing_Buf->Head_index) % pRing_Buf->Buffer_Size;
		pRing_Buf->Head_index = Next_Head_Index;
	}
	else {
		pRing_Buf->Fill_Size++;
	}
}

/*----------------------------------------------------------------------------*/
uint8_t	Ring_Buffer_Char_Get(Ring_Buffer_Char_t* pRing_Buf) {
	uint16_t ret = 0;
	uint16_t Next_Head_Index;

	if (pRing_Buf->Fill_Size) {
		ret = pRing_Buf->pBuf[pRing_Buf->Head_index];

		Next_Head_Index = (++pRing_Buf->Head_index) % pRing_Buf->Buffer_Size;
		pRing_Buf->Head_index = Next_Head_Index;

		pRing_Buf->Fill_Size--;
	}

	return ret;
}
