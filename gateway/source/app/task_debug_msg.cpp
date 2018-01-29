#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "app_if.h"
#include "app_data.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_list_if.h"
#include "if_rf24.h"

#include "task_debug_msg.h"

q_msg_t mt_task_debug_msg_mailbox;

uint8_t PLATFROM_MACHINE = 0;

uint8_t get_platform_machine(void);

ui_setting_t ui_setting;

ui_log_t ui_log;

uint8_t coil_irc = 0;

void* mt_task_debug_msg_entry(void*) {
	wait_all_tasks_started();

	PLATFROM_MACHINE = get_platform_machine();

	APP_DBG("[STARTED] mt_task_debug_msg_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_DEBUG_MSG_ID);

		switch (msg->header->sig) {

		case MT_DBG_SL_FATAL_TEST_REQ: {
			APP_DBG("TASK DBG FATAL REQ\n");
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_DBG_FATAL_TEST_REQ);
			set_msg_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_DBG_SL_FATAL_LOG_READ_REQ: {
			APP_DBG("MT_DBG_SL_FATAL_LOG_READ_REQ\n");
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_DBG_FATAL_LOG_READ_REQ);
			set_msg_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_DBG_SL_FATAL_LOG_READ_RES: {
			APP_DBG("TASK DBG FATAL LOG READ RES\n");

			fatal_log_t login_fatal_log;

			memset((uint8_t*)&login_fatal_log, 0, sizeof(fatal_log_t));

			if(PLATFROM_MACHINE == 32)
				get_data_common_msg(msg, (uint8_t*)&login_fatal_log, sizeof(fatal_log_t));
			else if(PLATFROM_MACHINE == 64) {
				get_data_common_msg(msg, (uint8_t*)&login_fatal_log, 70);
			}

			APP_DBG("[times] fatal: %d\n",			login_fatal_log.fatal_times);
			APP_DBG("[times] restart: %d\n",		login_fatal_log.restart_times);

			APP_PRINT("\n");
			APP_PRINT("[fatal] type: %s\n",			login_fatal_log.string);
			APP_PRINT("[fatal] code: 0x%02X\n",		login_fatal_log.code);

			APP_PRINT("\n");
			APP_PRINT("[task] id: %d\n",			login_fatal_log.current_task.id);
			APP_PRINT("[task] pri: %d\n",			login_fatal_log.current_task.pri);
			APP_DBG("[task] entry: 0x%x\n",			login_fatal_log.current_task.task);

			APP_PRINT("\n");
			APP_PRINT("[obj] task: %d\n",			login_fatal_log.current_active_object.des_task_id);
			APP_PRINT("[obj] sig: %d\n",			login_fatal_log.current_active_object.sig);
			APP_PRINT("[obj] type: 0x%x\n",			get_fatal_msg_type(&login_fatal_log.current_active_object));
			APP_PRINT("[obj] ref count: %d\n",		get_fatal_msg_ref_count(&login_fatal_log.current_active_object));
			APP_PRINT("[obj] wait time: %d\n",		login_fatal_log.current_active_object.dbg_handler.start_exe -  \
					  login_fatal_log.current_active_object.dbg_handler.start_post);

			APP_PRINT("\n");
			APP_PRINT("[core] ipsr: %d\n",			login_fatal_log.m3_core_reg.ipsr);
			APP_PRINT("[core] primask: 0x%08X\n",	login_fatal_log.m3_core_reg.primask);
			APP_PRINT("[core] faultmask: 0x%08X\n",	login_fatal_log.m3_core_reg.faultmask);
			APP_PRINT("[core] basepri: 0x%08X\n",	login_fatal_log.m3_core_reg.basepri);
			APP_PRINT("[core] control: 0x%08X\n",	login_fatal_log.m3_core_reg.control);
			APP_PRINT("\n");
			APP_PRINT("[irq] IRQ number: %d\n",		(int32_t)((int32_t)login_fatal_log.m3_core_reg.ipsr - \
															  (int32_t)SYS_IRQ_EXCEPTION_NUMBER_IRQ0_NUMBER_RESPECTIVE));
			{
				ak_msg_t* s_msg = ak_memcpy_msg(msg);

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg,MT_TASK_SENSOR_ID);
				set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
				set_if_sig(s_msg, GU_SM_DBG_FATAL_LOG_READ_RES);

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_DBG_SL_FATAL_LOG_RESET_REQ: {
			APP_DBG("MT_DBG_SL_FATAL_LOG_READ_REQ\n");
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_DBG_FATAL_LOG_RESET_REQ);
			set_msg_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case 11: {
			timer_set(MT_TASK_DEBUG_MSG_ID, 12, 200, TIMER_PERIODIC);
			timer_set(MT_TASK_DEBUG_MSG_ID, 20, 1, TIMER_ONE_SHOT);
		}
			break;

		case 12: {

			coil_irc = (coil_irc >=5)?1:coil_irc;
			coil_irc++;

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));
			generator.slaveID		  = 11;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_WRITE_COIL;
			generator.reg[0]          = coil_irc;
			generator.reg_value[0]    = 0x00FF;

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
				set_if_sig(s_msg, SL_DBG_MODBUS_WRITE_SINGLE_COIL_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case 13: {
			timer_set(MT_TASK_DEBUG_MSG_ID, 14, 400, TIMER_PERIODIC);
			timer_set(MT_TASK_DEBUG_MSG_ID, 19, 1, TIMER_PERIODIC);
		}
			break;

		case 14: {

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));
			generator.slaveID		  = 11;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_READ_COILS;
			generator.reg[0]          = 0x0000;
			generator.reg_value[0]    = 0x000C;

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
				set_if_sig(s_msg, SL_MODBUS_READ_COIL_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_DEBUG_MSG_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case 17: {
			timer_remove_attr (MT_TASK_DEBUG_MSG_ID, 12);
			timer_remove_attr (MT_TASK_DEBUG_MSG_ID, 14);
		}
			break;
		case 19: {
		}
			break;

		case 20: {
			timer_set(MT_TASK_DEBUG_MSG_ID, 20, 1, TIMER_ONE_SHOT);
		}
			break;

		default:
			break;
		}
		/* free message */
		msg_free(msg);
	}
	return (void*)0;
}

uint8_t get_platform_machine(void) {

	switch(sizeof(void*)){
	case 4: {
		APP_DBG("32 bit platform\n");
		return 32;
	}
		break;
	case 8:{
		APP_DBG("64 bit platform\n");
		return 64;
	}
		break;

	default:
		APP_DBG("unknow platfrom\n");
		FATAL("AK",0x007);
		break;
	}
}

void APP_UI_LOG(const char* signal, ui_setting_t* enable) {
	if (enable->sensor_report_en == 1) {
		strcpy(ui_log.log, signal);

		{
			ak_msg_t* s_msg = get_common_msg();

			set_if_src_type(s_msg, IF_TYPE_APP_MT);
			set_if_des_type(s_msg, IF_TYPE_APP_GU);
			set_if_src_task_id(s_msg,MT_TASK_DEBUG_MSG_ID);
			set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
			set_if_sig(s_msg, GU_SM_SL_LOG_DBG_SHOW_RES);
			set_if_data_common_msg(s_msg, (uint8_t*)&ui_log, sizeof(ui_log_t));

			set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
	}
}

void APP_UI_FW(const char* signal, ui_setting_t* enable) {
	if (enable->sensor_report_en == 1) {
		strcpy(ui_log.log, signal);

		{
			ak_msg_t* s_msg = get_common_msg();

			set_if_src_type(s_msg, IF_TYPE_APP_MT);
			set_if_des_type(s_msg, IF_TYPE_APP_GU);
			set_if_src_task_id(s_msg,MT_TASK_FIRMWARE_ID);
			set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
			set_if_sig(s_msg, GU_FIRMWARE_TRANSFER_STATUS);
			set_if_data_common_msg(s_msg, (uint8_t*)&ui_log, sizeof(ui_log_t));

			set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_FIRMWARE_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
	}
}
