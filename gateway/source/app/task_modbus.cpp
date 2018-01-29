#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_modbus.h"

q_msg_t mt_task_modbus_mailbox;

void modbus_status(int8_t data);

void* mt_task_modbus_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_modbus_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */

		msg = msg_get(MT_TASK_MODBUS_ID);

		switch (msg->header->sig) {

		case MT_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES: {
			APP_DBG("SL_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES\n");

			int16_t status;

			get_data_common_msg(msg, (uint8_t *)&status, sizeof(int8_t));
			modbus_status(status);

			{
				ak_msg_t* s_msg = ak_memcpy_msg(msg);

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg,MT_TASK_MODBUS_ID);
				set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
				set_if_sig(s_msg, GU_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES);

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_MODBUS_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_DBG_MODBUS_SLAVE_LIST_INFO_REQ: {
			APP_DBG("SL_DBG_MODBUS_SLAVE_LIST_INFO_REQ\n");

			ak_msg_t* s_msg = get_pure_msg();
			set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
			set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
			set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
			set_if_src_task_id(s_msg, MT_TASK_MODBUS_ID);
			set_if_sig(s_msg, SL_DBG_MODBUS_SLAVE_LIST_INFO_REQ);

			set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_MODBUS_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		case MT_DBG_MODBUS_SLAVE_LIST_INFO_RES: {
			//APP_DBG("SL_DBG_MODBUS_SLAVE_LIST_INFO_RES\n");

			modbus_slave_list slave_list;
			memset(&slave_list, 0, sizeof(modbus_slave_list));

			get_data_common_msg(msg ,(uint8_t *)&slave_list, sizeof(modbus_slave_list));

			APP_PRINT("[-----------------------------]\n");
			APP_PRINT("SLAVE LIST:\n");
			APP_PRINT(" -Slave1:\n");
			APP_PRINT("   +Name: %s\n",slave_list.slave1.name);
			APP_PRINT("   +Address: %02X\n",slave_list.slave1.id);

			APP_PRINT(" -Slave2:\n");
			APP_PRINT("   +Name: %s\n",slave_list.slave2.name);
			APP_PRINT("   +Address: %02X\n",slave_list.slave2.id);
			APP_PRINT("[-----------------------------]\n");
			APP_PRINT("									\n");

			{
				ak_msg_t* s_msg = ak_memcpy_msg(msg);

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg, MT_TASK_MODBUS_ID);
				set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
				set_data_common_msg(s_msg, (uint8_t *)&slave_list, sizeof(modbus_slave_list));
				set_if_sig(s_msg, GU_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES);

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_MODBUS_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_MODBUS_READ_COIL_RES: {
			APP_DBG("MT_MODBUS_READ_COIL_RES\n");

			power_reset_registers_t coil_list;
			memset(&coil_list, 0, sizeof(power_reset_registers_t));

			get_data_common_msg(msg ,(uint8_t *) &coil_list, sizeof(power_reset_registers_t));

			for(int i = 0; i < SL_MAX_MODBUS_COIL; i++) {
				APP_PRINT("Device[%d]:	", (i+1));
				if( coil_list.coil[i].state == 0) {
					APP_PRINT("ON\n");
				}
				else {
					APP_PRINT("OFF\n");
				}
			}

			{
				ak_msg_t* s_msg = ak_memcpy_msg(msg);

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg, MT_TASK_MODBUS_ID);
				set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
				set_if_sig(s_msg, GU_SM_MODBUS_READ_COIL_RES);

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(msg, MT_TASK_MODBUS_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		default:
			break;
		}

		msg_free(msg);
	}

	return (void*)0;
}

void modbus_status(int8_t data) {

	APP_PRINT("[-----------------------------]\n");
	APP_PRINT("MODBUS COMMAND STATUS: ");
	switch(data) {
	case COM_SUCCESSFULL: {
		APP_PRINT("COMMAND SUCCESSFUL\n");
	}
		break;

	case COM_IDLE: {
		APP_PRINT("COM_IDLE\n");
	}
		break;

	case COM_WAITING: {
		APP_PRINT("COM_WAITING\n");
	}
		break;

	case NO_REPLY: {
		APP_PRINT("NO_REPLY\n");
	}
		break;
	case EXC_FUNC_CODE: {
		APP_PRINT("EXC_FUNC_CODE\n");
	}
		break;

	case EXC_ADDR_RANGE: {
		APP_PRINT("EXC_ADDR_RANGE\n");
	}
		break;

	case ERR_EXCEPTION: {
		APP_PRINT("ERR_EXCEPTION\n");
	}
		break;

	case EXC_BUFFER: {
		APP_PRINT("EXC_BUFFER\n");
	}
		break;

	case EXC_ID: {
		APP_PRINT("EXC_ID\n");
	}
		break;

	case EXC_BAD_CRC: {
		APP_PRINT("EXC_BAD_CRC\n");
	}
		break;

	default:
		break;
	}
	APP_PRINT("[-----------------------------]\n");
	APP_PRINT("									\n");
}
