#ifndef __APP_MBMASTER_H
#define __APP_MBMASTER_H

#include <stdio.h>
#include <stdint.h>

#include "mbport.h"
#include "mbm.h"
#include "mbtypes.h"
#include "mbportlayer.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
/* csModbus master default serial initialization */
#define mbmtDEFAULT_MODE				( MB_RTU )
#define mbmtDEFAULT_PORT				( MB_UART_2 )
#define mbmtDEFAULT_BAUD				( 9600 )
#define mbmtDEFAULT_PARITY				( MB_PAR_NONE )
#define mbmtDEFAULT_DATABITS			( 8 )
#define mbmtDEFAULT_STOPBIT			    ( 1 )


/* csModbus master function code */
enum {
	MODBUS_FUNCTION_NONE 						= 0,
	MODBUS_FUNCTION_READ_COILS 					= 1,
	MODBUS_FUNCTION_READ_DISCRETE_INPUT 		= 2,
	MODBUS_FUNCTION_READ_REGISTERS 				= 3,
	MODBUS_FUNCTION_READ_INPUT_REGISTER 		= 4,
	MODBUS_FUNCTION_WRITE_COIL 					= 5,
	MODBUS_FUNCTION_WRITE_REGISTER 				= 6,
	MODBUS_FUNCTION_WRITE_MULTIPLE_COILS 		= 15,
	MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS	= 16
};

/* Typedef -------------------------------------------------------------------*/
typedef struct {
	eMBSerialMode tMode;
	UCHAR tPort;
	ULONG tBaudRate;
    UCHAR tDatabits;
	eMBSerialParity tParity;
	UCHAR tStopBits;
} MB_InitStruct_t;

typedef struct {
    eMBErrorCode code;
    char *tComment;
} MBReportStruct_t;


/* Extern variables ----------------------------------------------------------*/
extern MB_InitStruct_t MB_InitStructure;

/* Function prototypes -------------------------------------------------------*/
extern void appMBMasterInitial(void);
extern int8_t appMBMasterRead(UCHAR slAddr, uint8_t funCode, USHORT addReg, USHORT *buf);
extern int8_t appMBMasterWrite(UCHAR slAddr, uint8_t funCode, USHORT addReg, USHORT val);


#endif /* __APP_MBMASTER_H */