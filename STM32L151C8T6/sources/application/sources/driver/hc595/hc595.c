//=============================================================================
//
//=============================================================================
// Project   :  74HC595.h
// Author    :  HungPNQ
// Date      :  2022-07-22
// Brief     :  Shift register output
//=============================================================================

#include <string.h>

#include "HC595.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define Clk_Set(Obj)     	        ((HC595_Ctl_t *)Obj)->Clk(1)
#define Clk_Reset(Obj)   	        ((HC595_Ctl_t *)Obj)->Clk(0)

#define Dat_Set(Obj)     	        ((HC595_Ctl_t *)Obj)->Data(1)
#define Dat_Reset(Obj)   	        ((HC595_Ctl_t *)Obj)->Data(0)

#define Lat_Set(Obj)    	        ((HC595_Ctl_t *)Obj)->Latch(1)
#define Lat_Reset(Obj)  	        ((HC595_Ctl_t *)Obj)->Latch(0)

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void Shift_Out(HC595_Ctl_t *Obj);

/* Function implementation ---------------------------------------------------*/
void HC595_IO_Setup(HC595_Ctl_t *Obj, pf_IO_Init Init,	pf_Ctl_Clk Clk, pf_Ctl_Dat Data, pf_Ctl_Lat Latch)
{
	Obj->Shift_Byte_Data = 0;
	Obj->Init  		= Init;
	Obj->Clk 	    = Clk;
	Obj->Data 		= Data;
	Obj->Latch 		= Latch;

	Obj->Init();
}


int8_t HC595_IO_Shift_Byte(HC595_Ctl_t *Obj, uint8_t byte, uint8_t Opt) {
	if (byte < 0 || byte > 0xFF) {
		return (HC595_NG);
	}

	if (Opt == LSB_FIRST) {
		byte = (byte & 0xF0) >> 4 | (byte & 0x0F) << 4;
		byte = (byte & 0xCC) >> 2 | (byte & 0x33) << 2;
		byte = (byte & 0xAA) >> 1 | (byte & 0x55) << 1;
	}

	Obj->Shift_Byte_Data = byte;
	Shift_Out(Obj);

	return (HC595_OK);
}

void HC595_IO_Set_Bit(HC595_Ctl_t *Obj, uint8_t bitiDx, uint8_t valBit) {
	uint8_t shiftBit = 0;

	if (bitiDx >= 0 && bitiDx < HC595_TOTAL_OUTPUT_GATE) {
		shiftBit = (HC595_TOTAL_OUTPUT_GATE - 1) - bitiDx;
		valBit = ~((!valBit) << shiftBit);

		if (HC595_IO_Read_Bit(Obj, bitiDx) == 1) {
			Obj->Shift_Byte_Data &= valBit;
		}
		else {
			Obj->Shift_Byte_Data |= valBit;
		}

		Shift_Out(Obj);
	}
}

uint8_t  HC595_IO_Read_Byte(HC595_Ctl_t *Obj) {
	return (Obj->Shift_Byte_Data);
}

uint8_t  HC595_IO_Read_Bit(HC595_Ctl_t *Obj, uint8_t bitiDx) {
	uint8_t valBit = 0;
	uint8_t shiftBit;

	shiftBit = (HC595_TOTAL_OUTPUT_GATE - 1) - bitiDx;
	valBit = (Obj->Shift_Byte_Data >> shiftBit) & (0x01);
	
	return (valBit);
}

void Shift_Out(HC595_Ctl_t *Obj) {
	uint8_t iDxBit;

	for (iDxBit = 0; iDxBit < HC595_TOTAL_OUTPUT_GATE; ++iDxBit) {
		if (((Obj->Shift_Byte_Data) >> iDxBit) & (0x01)) {
			Dat_Set(Obj);
		}
		else {
			Dat_Reset(Obj);
		}

		Clk_Set(Obj);
		Clk_Reset(Obj);
	}

	Lat_Set(Obj);
	Lat_Reset(Obj);	
}
