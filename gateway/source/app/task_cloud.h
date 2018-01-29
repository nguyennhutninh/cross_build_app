#ifndef __TASK_CLOUD_H__
#define __TASK_CLOUD_H__

#include <mosquittopp.h>
#include "../ak/message.h"

#define MQTT_HOST			"118.69.135.199"
#define MQTT_PORT			1883
#define DOOR_CLOUD_HOST		"iot.888999.vn"

extern q_msg_t mt_task_cloud_mailbox;
extern void* mt_task_cloud_entry(void*);

#endif //__TASK_CLOUD_H__
