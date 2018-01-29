#ifndef __IF_CONSOLE_H__
#define __IF_CONSOLE_H__

#include "../ak/message.h"

#define CMD_BUFFER_SIZE						32

extern q_msg_t mt_task_if_console_mailbox;
extern void* mt_task_if_console_entry(void*);

#endif //__IF_CONSOLE_H__
