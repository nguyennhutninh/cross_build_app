#ifndef __TASK_IF_H__
#define __TASK_IF_H__

#include "../ak/message.h"

#define IF_RF24_ENABLE				0
#define IF_APP_ENABLE				1
#define IF_CPU_SERIAL_ENABLE		1

extern q_msg_t mt_task_if_mailbox;
extern void* mt_task_if_entry(void*);

#endif //__TASK_IF_H__
