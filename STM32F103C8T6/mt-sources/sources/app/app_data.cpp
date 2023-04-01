#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"

#define TAG "appData"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static fileManager_t filePathList[] = {
    {   SERIAL_PORT_INTERFACE   ,   (char*)GW_SERIAL_PORT_INTERFACE             },
    {   SERIAL_PORT_MODBUS      ,   (char*)GW_SERIRAL_PORT_MODBUS               },
    {   GATEWAY_HARDWARE_INFO   ,   (char*)GW_HARDWARE_INFO_FILE_PATH           },
    {   FIRMWARE_OTA_SL_BOOT    ,   (char*)DEVICE_FIRMWARE_BOOT_FILE_PATH       },
    {   FIRMWARE_OTA_SL_APP     ,   (char*)DEVICE_FIRMWARE_APP_FILE_PATH        }
};

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void filePathDirInit() {
    FILE *fp = NULL;
    string cmdStr;

    cmdStr.assign("mkdir -p ");
    cmdStr.append(GW_HARDWARE_INFO_PATH);
    fp = popen(cmdStr.c_str(), "r");
    if (fp == NULL) {
        APP_DBG(TAG, "[NG] %s", cmdStr.c_str());
    }

    cmdStr.assign("mkdir -p ");
    cmdStr.append(GW_FIRMWARE_OTA_PATH);
    fp = popen(cmdStr.c_str(), "r");
    if (fp == NULL) {
        APP_DBG(TAG, "[NG] %s", cmdStr.c_str());
    }

    cmdStr.assign("mkdir -p ");
    cmdStr.append(GW_APP_CONFIG_PATH);
    fp = popen(cmdStr.c_str(), "r");
    if (fp == NULL) {
        APP_DBG(TAG, "[NG] %s", cmdStr.c_str());
    }

    pclose(fp);
}

char* filePathRetStr(filePathIdx_t id) {
    return (id < END_FILE_PATH_ID) ? filePathList[id].pathStr : NULL;
}