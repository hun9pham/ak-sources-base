//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  Mechanism of task scheduler is referenced by doc/Samek0607.pdf
// Update	 :
//	 -Author	: HungPNQ
//	 -Date		: 02/12/2022
//	 -Modify	: Monitoring task polling id
//=============================================================================

#include "ak.h"
#include "ak_dbg.h"

#include "task.h"
#include "timer.h"
#include "message.h"

#include "sys_dbg.h"

#include "task_list.h"


/* Typedef -------------------------------------------------------------------*/
typedef struct {
	task_pri_t  pri;
	uint8_t     mask;
	ak_msg_t*   qhead;
	ak_msg_t*   qtail;
} tcb_t;

/* Private variables ---------------------------------------------------------*/
static task_id_t current_task_id;
static task_t current_task_info;
static ak_msg_t current_active_object;

/*----------------------------*
/	HungPNQ - Begin coding
/-----------------------------*/
static task_id_t currentTaskPollingId;
/*----------------------------*
/	HungPNQ - End coding
/-----------------------------*/


static tcb_t	task_pri_queue[TASK_PRI_MAX_SIZE];
static task_t*	task_table = (task_t*)0;
static uint8_t	task_table_size = 0;
static uint8_t	task_current = 0;
static uint8_t	task_ready = 0;

static task_polling_t* task_polling_table = (task_polling_t*)0;
static uint8_t	task_polling_table_size = 0;

/* Private function prototypes -----------------------------------------------*/
static void task_sheduler();


/* Function implementation ---------------------------------------------------*/
void task_create(task_t* task_tbl) {
	uint8_t idx = 0;
	if (task_tbl) {
		task_table = task_tbl;
		while (task_tbl[idx].id != SL_TASK_EOT_ID) {
			idx++;
		}
		task_table_size = idx;
	}
	else {
		FATAL("TK", 0x01);
	}
}

void task_polling_create(task_polling_t* task_polling_tbl) {
	uint8_t idx = 0;
	if (task_polling_tbl) {
		task_polling_table = task_polling_tbl;
		while (task_polling_tbl[idx].id != SL_TASK_POLLING_EOT_ID) {
			idx++;
		}
		task_polling_table_size = idx;
	}
	else {
		FATAL("TK", 0x06);
	}
}

void task_post(task_id_t des_task_id, ak_msg_t* msg) {
	tcb_t* t_tcb;

	if (des_task_id >= task_table_size) {
		FATAL("TK", 0x02);
	}
	
	t_tcb = &task_pri_queue[task_table[des_task_id].pri - 1];

	ENTRY_CRITICAL();

	msg->next = AK_MSG_NULL;
	msg->des_task_id = des_task_id;

	if (t_tcb->qtail == AK_MSG_NULL) {
		/* put message to queue */
		t_tcb->qtail = msg;
		t_tcb->qhead = msg;

		/* change status task to ready*/
		task_ready |= t_tcb->mask;
	}
	else {
		/* put message to queue */
		t_tcb->qtail->next = msg;
		t_tcb->qtail = msg;
	}

	EXIT_CRITICAL();
}

uint8_t task_remove_msg(task_id_t task_id, uint8_t sig) {
    tcb_t* t_tcb;
        uint8_t total_rm_msg = 0;

        ak_msg_t* del_msg = AK_MSG_NULL; /* MUST-BE initialized AK_MSG_NULL */
        ak_msg_t* trace_msg = AK_MSG_NULL;
        ak_msg_t* traverse_msg;

        if (task_id >= SL_TASK_EOT_ID) {
            FATAL("TK", 0x05);
        }

        ENTRY_CRITICAL();

        /* get task table control */
		t_tcb = &task_pri_queue[task_table[task_id].pri - 1];

        /* check task queue available */
        if (task_ready & t_tcb->mask) {

            /* get first message of queue */
            traverse_msg = t_tcb->qhead;

            while (traverse_msg != AK_MSG_NULL) {

                /* check message task id and signal */
                if (traverse_msg->des_task_id == task_id && traverse_msg->sig == sig) {

                    /* assign remove message */
                    del_msg = traverse_msg;

                    if (del_msg == t_tcb->qhead) {
                        t_tcb->qhead = traverse_msg->next;
                    }
                    else if (del_msg != t_tcb->qhead) {
                        trace_msg->next = traverse_msg->next;
                    }

                    /* last message of queue */
                    if (del_msg->next == AK_MSG_NULL) {
                        t_tcb->qtail = trace_msg;

                        /* Check if no message exist after remove current message */
                        if (t_tcb->qhead == AK_MSG_NULL) {

                            /* change status of task to inactive */
                            task_ready &= ~t_tcb->mask;
                        }
                    }
                }
                else {
                    trace_msg = traverse_msg;
                }

                /* consider the next message */
                traverse_msg = traverse_msg->next;

                /* free the message if it's found */
				if (del_msg != AK_MSG_NULL) {
					msg_force_free(del_msg);
                    del_msg = AK_MSG_NULL;
                    total_rm_msg++;
                }
            }
        }

        EXIT_CRITICAL();
        return total_rm_msg;
}

void task_post_pure_msg(task_id_t des_task_id, uint8_t sig) {
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, sig);
	task_post(des_task_id, s_msg);
}

void task_post_common_msg(task_id_t des_task_id, uint8_t sig, uint8_t* data, uint8_t len) {
	ak_msg_t* s_msg = get_common_msg();
	set_msg_sig(s_msg, sig);
	set_data_common_msg(s_msg, data, len);
	task_post(des_task_id, s_msg);
}

void task_post_dynamic_msg(task_id_t des_task_id, uint8_t sig, uint8_t* data, uint32_t len) {
	ak_msg_t* s_msg = get_dynamic_msg();
	set_msg_sig(s_msg, sig);
	set_data_dynamic_msg(s_msg, data, len);
	task_post(des_task_id, s_msg);
}

void task_entry_interrupt() {

	ENTRY_CRITICAL();
	current_task_id = AK_TASK_INTERRUPT_ID;
	EXIT_CRITICAL();
}

void task_exit_interrupt() {

	ENTRY_CRITICAL();
	current_task_id = current_task_info.id;
	EXIT_CRITICAL();
}

int task_init() {
	uint8_t pri;
	tcb_t* t_tcb;

	/* init task manager variable */
	task_current = 0;
	task_ready = 0;

	/* init kernel queue */
	for (pri = 1; pri <= TASK_PRI_MAX_SIZE; pri++) {
		t_tcb = &task_pri_queue[pri - 1];
		t_tcb->mask     = (1 << (pri - 1));
		t_tcb->qhead    = AK_MSG_NULL;
		t_tcb->qtail    = AK_MSG_NULL;
	}

	/* message manager must be initial fist */
	msg_init();

	/* init timer manager */
	timer_init();

	return 0;
}

int task_run() {

	for (;;) {
		task_sheduler();
        task_polling_run();
	}
}

void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability) {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < SL_TASK_POLLING_EOT_ID) {

		if (__task_polling_table->id == task_polling_id) {

			ENTRY_CRITICAL();

			__task_polling_table->ability = ability;

			EXIT_CRITICAL();

			break;
		}

		__task_polling_table++;
	}

	if (__task_polling_table->id == SL_TASK_POLLING_EOT_ID) {
		FATAL("TK", 0x07);
	}
}

void task_polling_run() {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < SL_TASK_POLLING_EOT_ID) {

		ENTRY_CRITICAL();
		if (__task_polling_table->ability == AK_ENABLE) {

			EXIT_CRITICAL();

			/*----------------------------*
			/	HungPNQ - Begin coding
			/-----------------------------*/
			currentTaskPollingId = __task_polling_table->id;
			/*----------------------------*
			/	HungPNQ - End coding
			/-----------------------------*/

			__task_polling_table->task_polling();
		}
		else {
			EXIT_CRITICAL();
		}
		__task_polling_table++;
    }
}

void task_sheduler() {

#if 0
	static uint8_t const log2lkup[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};
#endif

	uint8_t t_task_new;

	ENTRY_CRITICAL();

	uint8_t t_task_current = task_current;

	while ((t_task_new = LOG2LKUP(task_ready)) > t_task_current) {
		/* get task */
		tcb_t* t_tcb = &task_pri_queue[t_task_new - 1];

		/* get message */
		ak_msg_t* t_msg = t_tcb->qhead;
		t_tcb->qhead = t_msg->next;

		/* last message of queue */
		if (t_msg->next == AK_MSG_NULL) {
			t_tcb->qtail = AK_MSG_NULL;
			/* change status of task to inactive */
			task_ready &= ~t_tcb->mask;
		}

		/* Update current task */
		task_current = t_task_new;

		/* Update current ak object */
		memcpy(&current_task_info, &task_table[t_msg->des_task_id], sizeof(task_t));
		memcpy(&current_active_object, t_msg, sizeof(ak_msg_t));

		/* Update current task id NOTE: current task id will be change when entry interrupt handler */
		current_task_id = t_msg->des_task_id;

		EXIT_CRITICAL();
		/*---------------------------------------------*/
		/*		Task scheduler starts execution		   */
		/*---------------------------------------------*/
		task_table[t_msg->des_task_id].task(t_msg);
		/*---------------------------------------------*/
		/*		End of task scheduler execution		   */
		/*---------------------------------------------*/
		ENTRY_CRITICAL();

		/* Check and free message */
		msg_free(t_msg);
	}

	task_current = t_task_current;

	current_task_id = AK_TASK_IDLE_ID;

	EXIT_CRITICAL();
}

task_id_t task_self() {
	return current_task_info.id;
}

task_id_t get_current_task_id() {
	return current_task_id;
}

task_t*	get_current_task_info() {
	return (task_t*)&current_task_info;
}

ak_msg_t* get_current_active_object() {
	return (ak_msg_t*)&current_active_object;
}

/*----------------------------*
/	HungPNQ - Begin coding
/-----------------------------*/
task_id_t getLastTaskPollId(void) {
	return currentTaskPollingId;
}
/*----------------------------*
/	HungPNQ - End coding
/-----------------------------*/