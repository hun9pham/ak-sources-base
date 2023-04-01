#ifndef __SYS_BOOT_H
#define __SYS_BOOT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
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

/* Typedef -------------------------------------------------------------------*/
typedef struct {
	uint32_t Psk;
	uint32_t binLen;
	uint16_t fCs; /* Firmware Checksum */
} firmwareHeader_t;

typedef struct {
	uint32_t addrDes;
	uint32_t addrSrc;
	uint8_t target;
} firmwareAssert_t;

typedef struct {
	/* Current firmware header, that field will be update when system start */
	firmwareHeader_t bootCurrent;
	firmwareHeader_t appCurrent;

	/* New firmwre header, that field contain new firmware header */
	firmwareHeader_t bootUpdate;
	firmwareHeader_t appUpdate;

	/* Assertion */
	firmwareAssert_t assert;
} sysBoot_t;

/* Function prototypes -------------------------------------------------------*/
extern void sysBootInit(void);
extern void getSysBoot(sysBoot_t* sysBoot);
extern uint8_t setSysBoot(sysBoot_t* sysBoot);
extern sysBoot_t* readSysBoot(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_BOOT_H */
