#ifndef __FIRMWARE_H
#define __FIRMWARE_H

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define SYS_BOOT_OK							( 0x00 )
#define SYS_BOOT_NG							( 0x01 )
#define FIRMWARE_PSK						( 0x1A2B3C4D )

#define SYS_TARGET_FIRMWARE_BOOT			(0xEF)
#define SYS_TARGET_FIRMWARE_APP				(0xFF)

enum eFirmwareRequest {
	BOOT_CMD_NONE = 0x01,
	BOOT_CMD_UPDATE_BOOT_REQ,
	BOOT_CMD_UPDATE_BOOT_RES,
	BOOT_CMD_UPDATE_APP_REQ,
	BOOT_CMD_UPDATE_APP_RES,
};

/* Enumarics -----------------------------------------------------------------*/
enum eFileHandleReturn {
	FILE_OPEN_FAILED = -3,
	FILE_READ_FAILED = -2,
	FILE_READ_OVERFLOW = -1,
	FILE_NOERROR = 0,
};

/* Typedef -------------------------------------------------------------------*/
typedef struct {
	uint32_t Psk;
	uint32_t binLen;
	uint16_t fCs;
} firmwareHeader_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern int8_t firmwareBinGetInfo(firmwareHeader_t *firmwareHeader, const char *path);
extern int8_t firmwareReadFile(uint8_t *buf, uint32_t cursor, uint32_t len, const char* path);

#endif /* __FIRMWARE_H */
