#ifndef __TASK_SENSOR_H__
#define __TASK_SENSOR_H__

#include "../ak/message.h"

extern q_msg_t mt_task_sensor_mailbox;
extern void* mt_task_sensor_entry(void*);
extern void APP_UI_LOG(const char* signal, ui_setting_t* enable);

#endif //__TASK_SENSOR_H__
