#ifndef __TASK_SM_H__
#define __TASK_SM_H__

#include "../ak/ak.h"
#include "../ak/message.h"

#define MT_SL_POWER_ON		0
#define MT_SL_IDLE			1
#define MT_SL_FW			2

extern void tsm_mt_sl_on_state(tsm_state_t);

extern tsm_t* tsm_mt_sl_table[];
extern tsm_tbl_t tsm_mt_sl;

extern q_msg_t mt_task_sm_mailbox;
extern void* mt_task_sm_entry(void*);

#endif //__TASK_SM_H__
