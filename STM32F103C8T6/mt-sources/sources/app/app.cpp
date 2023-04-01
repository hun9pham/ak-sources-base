#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include "ak.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void task_init() {
    APP_PRINT(
			KBLU"\r\n"
			KYEL"+--------------------------------------------------+\n"
			KYEL"+ " KRED "ROOF SECURITY SYSTEM MASTER APPLICATION\n"
			KYEL"+ " KRED "Kernel: %s\n"
			KYEL"+ " KRED "Platform: %s\n"
			KYEL"+ " KRED "Version: %s\n"
			KYEL"+--------------------------------------------------+\n"
			KNRM, AK_VERSION, PLATFORM_RUNNING, APP_VERSION
			);

	filePathDirInit();
}
