//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     : 
//=============================================================================

#ifndef __PORT_H
#define __PORT_H

#include "sys_ctl.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define __AK_PACKETED	        __attribute__((__packed__))
#define __AK_WEAK		        __attribute__((__weak__))

#define AkCtl_Millis()          millisTick()

#define __AK_MALLOC_CTRL_SIZE	( 8 )

#endif /* __PORT_H */
