#ifndef __TASK_LIST_IF_H
#define __TASK_LIST_IF_H

/*---------------------------------------------------------------------------*
 *  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
    /* SYSTEM TASKS */
    MT_TASK_TIMER_ID,

    /* APP TASKS */
    MT_TASK_IF_ID,
    MT_TASK_IF_CPU_SERIAL_ID,
    MT_TASK_SM_ID,
    MT_TASK_IF_CONSOLE_ID,

    /* LINK TASKS */
    MT_LINK_PHY_ID,
    MT_LINK_MAC_ID,
    MT_LINK_ID,

    /* EOT task ID */
    MT_TASK_LIST_LEN
};

#endif /* __TASK_LIST_IF_H */
