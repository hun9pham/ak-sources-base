#include <stdbool.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "timer.h"

#include "cmd_line.h"
#include "xprintf.h"

#include "led.h"
#include "button.h"
#include "flash.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"
#include "app_mbmaster.h"
#include "task_list.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG		"appMBMaster"

/* Extern variables ----------------------------------------------------------*/
MB_InitStruct_t MB_InitStructure = {
	.tMode 		= mbmtDEFAULT_MODE,
	.tPort 		= mbmtDEFAULT_PORT,
	.tBaudRate 	= mbmtDEFAULT_BAUD,
	.tDatabits 	= mbmtDEFAULT_DATABITS,
	.tParity 	= mbmtDEFAULT_PARITY,
	.tStopBits 	= mbmtDEFAULT_STOPBIT
};

/* Private variables ---------------------------------------------------------*/
static xMBHandle xMBMMaster;
static eMBErrorCode MBErrorCode;

static MBReportStruct_t MB_ReportList[] = {
    {   MB_ENOERR,                      (char *)"No error"                                  },
    {   MB_ENOREG,                      (char *)"Illegal register address"                  },
    {   MB_EINVAL,                      (char *)"Illegal argument"                          },
    {   MB_EPORTERR,                    (char *)"Porting layer error"                       },
    {   MB_ENORES,                      (char *)"Insufficient resources"                    },
    {   MB_EIO,                         (char *)"I/O error"                                 },
    {   MB_EILLSTATE,                   (char *)"Protocol stack in illegal state"           },
    {   MB_EAGAIN,                      (char *)"Retry I/O operation"                       },
    {   MB_ETIMEDOUT,                   (char *)"Timeout error occurred"                    },
    {   (eMBErrorCode)9,                (char *)NULL                                        },
    {   MB_EX_ILLEGAL_FUNCTION,         (char *)"Illegal function exception"                },
    {   MB_EX_ILLEGAL_DATA_ADDRESS,     (char *)"Illegal data address"                      },
    {   MB_EX_ILLEGAL_DATA_VALUE,       (char *)"Illegal data value"                        },
    {   MB_EX_SLAVE_DEVICE_FAILURE,     (char *)"Slave device failure"                      },
    {   MB_EX_ACKNOWLEDGE,              (char *)"Slave acknowledge"                         },
    {   MB_EX_SLAVE_BUSY,               (char *)"Slave device busy"                         },
    {   MB_EX_MEMORY_PARITY_ERROR,      (char *)"Memory parity error"                       },
    {   MB_EX_GATEWAY_PATH_UNAVAILABLE, (char *)"Gateway path unavailable"                  },
    {   MB_EX_GATEWAY_TARGET_FAILED,    (char *)"Gateway target device failed to respond"   }
};

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void appMBMasterInitial() {
    eMBMClose(xMBMMaster);
	MBErrorCode = eMBMSerialInitExt(&xMBMMaster, 
                                    MB_InitStructure.tMode, 
                                    MB_InitStructure.tPort, 
                                    MB_InitStructure.tBaudRate, 
                                    MB_InitStructure.tParity, 
                                    MB_InitStructure.tStopBits);

	if (MBErrorCode != MB_ENOERR) {
		LOG_ERR(TAG, "appMBMasterInitial() | ERR: %s\n", MB_ReportList[MBErrorCode].tComment);
	}
}

/*----------------------------------------------------------------------------*/
int8_t appMBMasterRead(UCHAR slAddr, uint8_t funCode, USHORT addReg, USHORT *buf) {
    MBErrorCode = MB_ENOERR;

	switch (funCode) {
	case MODBUS_FUNCTION_READ_COILS: {
		MBErrorCode = eMBMReadCoils(xMBMMaster, slAddr, addReg, 1, (UBYTE*)buf);
	}
	break;

	case MODBUS_FUNCTION_READ_DISCRETE_INPUT: {
		MBErrorCode = eMBMReadDiscreteInputs(xMBMMaster, slAddr, addReg, 1, (UBYTE*)buf);
	}
	break;

	case MODBUS_FUNCTION_READ_REGISTERS: {
		MBErrorCode = eMBMReadHoldingRegisters(xMBMMaster, slAddr, addReg, 1, buf);
	}
	break;

	case MODBUS_FUNCTION_READ_INPUT_REGISTER: {
		MBErrorCode = eMBMReadInputRegisters(xMBMMaster, slAddr, addReg, 1, buf);
	}
	break;

	default:
		MBErrorCode = MB_EINVAL;
	break;
	}

	/* Check ERROR */
	if (MBErrorCode != MB_ENOERR) {
		LOG_ERR(TAG, "\nId: %d\nFunc: %d\nReg: %d\nERR: %s\n", slAddr, 
                                                            funCode, 
                                                            addReg,
                                                            MB_ReportList[MBErrorCode].tComment);
		return (-1);
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
int8_t appMBMasterWrite(UCHAR slAddr, uint8_t funCode, USHORT addReg, USHORT val) {
	MBErrorCode = MB_ENOERR;

	switch (funCode) {
	case MODBUS_FUNCTION_WRITE_REGISTER: {
		MBErrorCode = eMBMWriteSingleRegister(xMBMMaster, slAddr, addReg, val);
	}
	break;

	case MODBUS_FUNCTION_WRITE_COIL: {
		MBErrorCode = eMBMWriteSingleCoil(xMBMMaster, slAddr, addReg, val);
	}

	case MODBUS_FUNCTION_WRITE_MULTIPLE_COILS: {
		
	}
	break;

	case MODBUS_FUNCTION_WRITE_MULTIPLE_REGISTERS: {
		
	}
	break;

	default:
		MBErrorCode = MB_EINVAL;
	break;
	}

	if (MBErrorCode != MB_ENOERR) {
		LOG_ERR(TAG, "\nId: %d\nFunc: %d\nReg: %d\nERR: %s\n", slAddr, 
                                                            funCode, 
                                                            addReg,
                                                            MB_ReportList[MBErrorCode].tComment);

		return (-1);
	}

	return (1);
}

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMWriteSingleRegister()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: The register address to write
 *  \param_4: The value which should be written to the register
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMReadHoldingRegisters()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: The first holding register to be read
 *  \param_4: Number of registers to read
 *  \param_5: An array of USHORT values
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMWriteMultipleRegisters()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: First register to write to
 *  \param_4: Number of registers to write
 *  \param_5: An Array of USHORT values to write
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMReadInputRegisters()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: First register to read
 *  \param_4: Numer of registers to read
 *  \param_5: An array of USHORT values
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMReadDiscreteInputs()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: Address of first discrete input
 *  \param_4: Number of discrete inputs to read
 *  \param_5: An array with a size of at least 8 bytes
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 *  FUNCTION: eMBMReadCoils()
 *  \param_1: A valid MODBUS master handle
 *  \param_2: Slave address
 *  \param_3: Address of first coil
 *  \param_4: Number of coils to read
 *  \param_5: An array with a size of at least 8 bytes
 *----------------------------------------------------------------------------*/