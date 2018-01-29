#ifndef __TASK_MODBUS_H__
#define __TASK_MODBUS_H__

#include "../ak/message.h"

#define MAX_MODBUS_SLAVE_NAME		20

extern q_msg_t mt_task_modbus_mailbox;
extern void* mt_task_modbus_entry(void*);

typedef struct {
	char  name[MAX_MODBUS_SLAVE_NAME];
	uint16_t id;
} slave_t;

typedef struct {
	slave_t slave1;
	slave_t slave2;
} modbus_slave_list;

#endif //__TASK_MODBUS_H__
