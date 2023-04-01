#ifndef __APP_DATA_H
#define __APP_DATA_H

#include <iostream>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#if defined(NON_CROSSCOMPILE)
    #define GW_SERIAL_PORT_INTERFACE           (char*)"/dev/ttyUSB1"
    #define GW_SERIRAL_PORT_MODBUS             (char*)"/dev/ttyUSB0"
    #define GW_ROOT_PATH_DISK                  "/home/hung/burning"
    #define GW_ROOT_PATH_DISK                  "/home/hung/burning"
#else
    #define GW_ROOT_PATH_DISK                  "/home/device/fpt_gateway"
    #define GW_ROOT_PATH_RAM					"/run/fpt_gateway"
    #define GW_MAC_ADDRESS_PATH                "/sys/class/net/eth0/address"
    #define GW_SERIAL_PORT_INTERFACE           (char*)"/dev/ttyS2"
    #define GW_SERIRAL_PORT_MODBUS             (char*)"/dev/ttyS0"
#endif

/* Location Path */
#define GW_FIRMWARE_OTA_PATH                   GW_ROOT_PATH_DISK "/firmware_ota"
#define GW_HARDWARE_INFO_PATH                  GW_ROOT_PATH_DISK "/hardware"
#define GW_APP_CONFIG_PATH                     GW_ROOT_PATH_DISK "/config"

/* File Path */
#define GW_HARDWARE_INFO_FILE_PATH             GW_HARDWARE_INFO_PATH "/info.txt"
#define DEVICE_FIRMWARE_BOOT_FILE_PATH         GW_FIRMWARE_OTA_PATH "/boot-slave-roof-security-system.bin"
#define DEVICE_FIRMWARE_APP_FILE_PATH          GW_FIRMWARE_OTA_PATH "/app-slave-roof-security-system.bin"

/* Enumarics -----------------------------------------------------------------*/
typedef enum eFilePathIdx {
    SERIAL_PORT_INTERFACE,
    SERIAL_PORT_MODBUS,
    GATEWAY_HARDWARE_INFO,
    FIRMWARE_OTA_SL_BOOT,
    FIRMWARE_OTA_SL_APP,
    
    END_FILE_PATH_ID,
} filePathIdx_t;

/* Typedef -------------------------------------------------------------------*/
typedef struct t_fileManager {
    uint8_t id;
    char* pathStr;
} fileManager_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void filePathDirInit(void);
extern char* filePathRetStr(filePathIdx_t id);

#endif /* __APP_DATA_H */
