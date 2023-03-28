//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  05/09/2016
// Brief     :  Kernel timer
// Update    : 
//		Author: HungPNQ
//		Date:	27/09/2022
//		Brief: 	Adding function timer_reload()
//=============================================================================

#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ak.h"
#include "timer.h"
#include "task.h"
#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define TIMER_TICK					(0x01)

#define TIMER_MSG_NULL				((ak_timer_t*)0)
#define TIMER_RET_OK				(1)
#define TIMER_RET_NG				(0)

/* Timer pool size */
#ifndef AK_TIMER_POOL_SIZE
#define AK_TIMER_POOL_SIZE			(16)
#endif

/* Typedef -------------------------------------------------------------------*/
typedef uint8_t						timer_sig_t;

typedef enum {
	TIMER_ONE_SHOT,
	TIMER_PERIODIC
} timer_type_t;

typedef struct ak_timer_t {
	struct ak_timer_t*	next;			/* Manage timer message */

	task_id_t			des_task_id;	/* Destination task id */
	timer_sig_t			sig;			/* Signal for application */

	uint32_t			counter;		/* Decrease each timer stick */
	uint32_t			period;			/* Case one-shot timer, this field is equa 0 */
} ak_timer_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void timer_init();
extern void timer_tick(uint32_t t);
extern void task_timer_tick(ak_msg_t* msg);

extern uint8_t timer_set(task_id_t des_task_id, timer_sig_t sig, uint32_t duty, timer_type_t type);
extern uint8_t timer_remove_attr(task_id_t des_task_id, timer_sig_t sig);
extern void    timer_reload(task_id_t des_task_id, timer_sig_t sig, uint32_t reload);

extern uint32_t get_timer_msg_pool_used();
extern uint32_t get_timer_msg_pool_used_max();

#ifdef __cplusplus
}
#endif

#endif //__TIMER_H__
