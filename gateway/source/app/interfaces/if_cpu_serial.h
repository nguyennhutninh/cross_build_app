#ifndef __IF_CPU_SERIAL_H__
#define __IF_CPU_SERIAL_H__

#include <stdint.h>

#include "../ak/message.h"

#include "app.h"
#include "app_data.h"
#include "app_dbg.h"

extern q_msg_t mt_task_if_cpu_serial_mailbox;
extern void* mt_task_if_cpu_serial_entry(void*);

#endif //__IF_CPU_SERIAL_H__
