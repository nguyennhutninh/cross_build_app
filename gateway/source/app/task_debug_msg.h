#ifndef __TASK_DEBUG_MSG_H__
#define __TASK_DEBUG_MSG_H__

#include "../ak/message.h"
#include "../app.h"

extern q_msg_t mt_task_debug_msg_mailbox;
extern void* mt_task_debug_msg_entry(void*);
extern void APP_UI_LOG(const char* signal, ui_setting_t* enable);
extern void APP_UI_FW(const char* signal, ui_setting_t* enable);

extern ui_setting_t ui_setting;
extern ui_log_t ui_log;

#endif //__TASK_DEBUG_MSG_H__
