#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ak/ak.h"

#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_data.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_snmp.h"
#include "if_rf24.h"

#include "task_if.h"

q_msg_t mt_task_if_mailbox;

#if (IF_RF24_ENABLE == 1)
static void rf24_if_forward_msg(ak_msg_t* msg);
#endif

#if (IF_APP_ENABLE==1)
static void app_if_forward_msg(ak_msg_t* msg);
#endif

#if (IF_CPU_SERIAL_ENABLE==1)
static void cpu_serial_if_forward_msg(ak_msg_t* msg);
#endif

void* mt_task_if_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_if_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_IF_ID);

		/* handler message */
		if (msg->header->if_des_type == IF_TYPE_RF24_MT) {
#if (IF_RF24_ENABLE == 1)
			rf24_if_forward_msg(msg);
#endif
		}
		else if (msg->header->if_des_type == IF_TYPE_APP_GMNG ||
				 msg->header->if_des_type == IF_TYPE_APP_MT ||
				 msg->header->if_des_type == IF_TYPE_APP_GU) {
#if (IF_APP_ENABLE == 1)
			app_if_forward_msg(msg);
#endif
		}
		else if (msg->header->if_des_type == IF_TYPE_CPU_SERIAL_MT ||
				 msg->header->if_des_type == IF_TYPE_CPU_SERIAL_SL) {
#if (IF_CPU_SERIAL_ENABLE == 1)
			cpu_serial_if_forward_msg(msg);
#endif
		}

		/* free message */
		msg_free(msg);
	}

	return (void*)0;
}

#if (IF_RF24_ENABLE==1)
void rf24_if_forward_msg(ak_msg_t* msg) {
	switch (msg->header->sig) {
	case MT_IF_PURE_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(msg, msg->header->if_sig);
		set_msg_src_task_id(msg, msg->header->if_src_task_id);
		task_post(msg->header->if_des_task_id, msg);
	}
		break;

	case MT_IF_COMMON_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(msg, msg->header->if_sig);
		set_msg_src_task_id(msg, msg->header->if_src_task_id);
		task_post(msg->header->if_des_task_id, msg);
	}
		break;

	case MT_IF_PURE_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(msg, MT_RF24_IF_PURE_MSG_OUT);
		task_post(MT_TASK_IF_RF24_ID, msg);
	}
		break;

	case MT_IF_COMMON_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(msg, MT_RF24_IF_COMMON_MSG_OUT);
		task_post(MT_TASK_IF_RF24_ID, msg);
	}
		break;

	default:
		break;
	}
}
#endif


#if (IF_APP_ENABLE==1)
void app_if_forward_msg(ak_msg_t* msg) {
	switch (msg->header->sig) {
	case MT_IF_PURE_MSG_IN:	{
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;

	case MT_IF_COMMON_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;
	case MT_IF_DYNAMIC_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;

	case MT_IF_PURE_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_APP_ID, s_msg);
	}
		break;

	case MT_IF_COMMON_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_APP_ID, s_msg);
	}
		break;

	case MT_IF_DYNAMIC_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_APP_ID, s_msg);
	}
		break;

	default:
		break;
	}
}
#endif

#if (IF_CPU_SERIAL_ENABLE==1)
void cpu_serial_if_forward_msg(ak_msg_t* msg) {
	switch (msg->header->sig) {
	case MT_IF_PURE_MSG_IN:	{
		//APP_DBG("MT_IF_PURE_MSG_IN\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;

	case MT_IF_COMMON_MSG_IN: {
		//APP_DBG("MT_IF_COMMON_MSG_IN\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;

	case MT_IF_DYNAMIC_MSG_IN: {
		//APP_DBG("MT_IF_COMMON_MSG_IN\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
		break;

	case MT_IF_PURE_MSG_OUT: {
		//APP_DBG("MT_IF_PURE_MSG_OUT\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
		break;

	case MT_IF_COMMON_MSG_OUT: {
		//APP_DBG("MT_IF_COMMON_MSG_OUT\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
		break;

	case MT_IF_DYNAMIC_MSG_OUT: {
		//APP_DBG("MT_IF_COMMON_MSG_OUT\n");
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
		break;

	default:
		break;
	}
}
#endif
