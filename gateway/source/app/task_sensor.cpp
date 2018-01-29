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
#include "task_sensor.h"
#include "task_debug_msg.h"

#include "cmd_line.h"

#define SL_SENSOR_REQ_RETRY_MAX					5

sl_sensor_calib_t sl_sensor_calib;

q_msg_t mt_task_sensor_mailbox;

uint8_t sensor_sl_sensor_req_failed_counter  = 0;

void* mt_task_sensor_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_sensor_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_SENSOR_ID);

		switch (msg->header->sig) {
		case MT_SENSOR_SL_SENSOR_REPORT_REQ: {
			APP_DBG("MT_SENSOR_SL_SENSOR_REPORT_REQ\n");

			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_SENSOR_REPORT_REQ);
			set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
			task_post(MT_TASK_SM_ID, s_msg);

			timer_set(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ_TO, MT_SENSOR_SL_SENSOR_REPORT_REQ_TO_INTERVAL, TIMER_ONE_SHOT);

			if (ui_setting.sensor_report_en == 1) {
				char signal[] = "MT_SENSOR_SL_SENSOR_REPORT_REQ";
				memcpy(ui_log.log, signal, sizeof(signal));

				{
					ak_msg_t* s_msg = get_common_msg();

					set_if_src_type(s_msg, IF_TYPE_APP_MT);
					set_if_des_type(s_msg, IF_TYPE_APP_GU);
					set_if_src_task_id(s_msg,MT_TASK_SENSOR_ID);
					set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
					set_if_sig(s_msg, GU_SM_SL_LOG_DBG_SHOW_RES);
					set_if_data_common_msg(s_msg, (uint8_t*)&ui_log, sizeof(ui_log_t));

					set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
					set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
					task_post(MT_TASK_IF_ID, s_msg);
				}
			}
		}
			break;

		case MT_SENSOR_SL_SENSOR_DBG_REPORT_REQ: {
			//APP_DBG("MT_SENSOR_SL_SENSOR_DBG_REPORT_REQ\n");
			ak_msg_t* s_msg = get_pure_msg();
			set_msg_sig(s_msg, MT_SM_SENSOR_DBG_REPORT_REQ);
			set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_SENSOR_SL_SENSOR_REPORT_REQ_TO: {
			APP_DBG("MT_SENSOR_SL_SENSOR_REPORT_REQ_TO\n");
			APP_LOG("MT_SENSOR_SL_SENSOR_REPORT_REQ_TO");

			APP_UI_LOG("MT_SENSOR_SL_SENSOR_REPORT_REQ_TO", &ui_setting);

			if (sensor_sl_sensor_req_failed_counter++ < SL_SENSOR_REQ_RETRY_MAX) {
				APP_LOG("sensor_sl_sensor_req_failed_counter++");
				timer_set(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ, MT_SENSOR_SL_SENSOR_REPORT_REQ_INTERVAL, TIMER_ONE_SHOT);
			}
			else {
				sensor_sl_sensor_req_failed_counter = 0;
				timer_remove_attr(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ);

				{
					APP_LOG("MT_SM_SL_SYNC_REQ mess to SM task");
					ak_msg_t* s_msg = get_pure_msg();
					set_msg_sig(s_msg, MT_SM_SL_SYNC_REQ);
					set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
					task_post(MT_TASK_SM_ID, s_msg);
				}
			}
		}
			break;

		case MT_SENSOR_SL_SENSOR_CALIB_TRANS_REQ: {

			sl_sensor_calib_t sl_sensor_calib;

			get_data_common_msg(msg, (uint8_t*)&sl_sensor_calib, sizeof(sl_sensor_calib_t));

			APP_DBG("[SENSOR] TEMP calib value: [%c%2d - %c%2d - %c%2d - %c%2d]\n",
					sl_sensor_calib.temp_calib_opt[0],sl_sensor_calib.temp_calib_val[0],
					sl_sensor_calib.temp_calib_opt[1],sl_sensor_calib.temp_calib_val[1],
					sl_sensor_calib.temp_calib_opt[2],sl_sensor_calib.temp_calib_val[2],
					sl_sensor_calib.temp_calib_opt[3],sl_sensor_calib.temp_calib_val[3]
					);
			APP_DBG("[SENSOR] HUMI calib value: [%c%2d - %c%2d]\n",
					sl_sensor_calib.humi_calib_opt[0],sl_sensor_calib.humi_calib_val[0],
					sl_sensor_calib.humi_calib_opt[1],sl_sensor_calib.humi_calib_val[1]
					);

			ak_msg_t* s_msg = ak_memcpy_msg(msg);
			set_msg_sig(msg, MT_SM_SL_SENSOR_CALIB_TRANS_REQ);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_SENSOR_SL_SENSOR_REPORT_RES: {
			APP_DBG("MT_SENSOR_SL_SENSOR_REPORT_RES\n");
			timer_remove_attr(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ_TO);
			timer_set(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ, MT_SENSOR_SL_SENSOR_REPORT_REQ_INTERVAL, TIMER_ONE_SHOT);

			{
				ak_msg_t* s_msg = ak_memcpy_msg(msg);
				set_msg_sig(s_msg, MT_SNMP_SL_SENSOR_REPORT_REP);
				set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
				task_post(MT_TASK_SNMP_ID, s_msg);
			}

			if (ui_setting.sensor_report_en == 1) {

				{
					ak_msg_t* s_msg = ak_memcpy_msg(msg);

					set_if_src_type(s_msg, IF_TYPE_APP_MT);
					set_if_des_type(s_msg, IF_TYPE_APP_GU);
					set_if_src_task_id(s_msg,MT_TASK_SENSOR_ID);
					set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
					set_if_sig(s_msg, MT_SM_SL_SENSOR_DBG_REPORT_RES);

					set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
					set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
					task_post(MT_TASK_IF_ID, s_msg);
				}
			}
		}
			break;

		case MT_SENSOR_SL_SENSOR_CALIB_INFO_REQ: {
			ak_msg_t* s_msg = get_pure_msg();

			set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
			set_msg_sig(s_msg, MT_SM_SL_SENSOR_CALIB_REPORT_REQ);
			task_post(MT_TASK_SM_ID, s_msg);
		}
			break;

		case MT_SENSOR_SL_SENSOR_CALIB_INFO_RES: {
			sl_sensor_calib_t sl_sensor_calib;

			get_data_common_msg(msg, (uint8_t*)&sl_sensor_calib, sizeof(sl_sensor_calib_t));

			APP_DBG("[SENSOR] TEMP calib value: [%c%2d - %c%2d - %c%2d - %c%2d]\n",
					sl_sensor_calib.temp_calib_opt[0],sl_sensor_calib.temp_calib_val[0],
					sl_sensor_calib.temp_calib_opt[1],sl_sensor_calib.temp_calib_val[1],
					sl_sensor_calib.temp_calib_opt[2],sl_sensor_calib.temp_calib_val[2],
					sl_sensor_calib.temp_calib_opt[3],sl_sensor_calib.temp_calib_val[3]
					);
			APP_DBG("[SENSOR] HUMI calib value: [%c%2d - %c%2d]\n",
					sl_sensor_calib.humi_calib_opt[0],sl_sensor_calib.humi_calib_val[0],
					sl_sensor_calib.humi_calib_opt[1],sl_sensor_calib.humi_calib_val[1]
					);
		}
			break;

		case MT_SENSOR_SL_SENSOR_DBG_REPORT_RES: {
			//APP_DBG("MT_SENSOR_SL_SENSOR_DBG_REPORT_RES\n");
			sl_sensors_t st_sensors;

			get_data_common_msg(msg, (uint8_t*)&st_sensors, sizeof(sl_sensors_t));

			APP_PRINT("[-----------------------------]\n");
			APP_DBG("temperature      : [%2d : %2d :%2d :%2d]\n",
					st_sensors.temperature[0],
					st_sensors.temperature[1],
					st_sensors.temperature[2],
					st_sensors.temperature[3]);
			APP_DBG("himidity         : [%2d : %2d]\n",
					st_sensors.humidity[0],
					st_sensors.humidity[1]);
			APP_DBG("general_output   : [%2d : %2d :%2d :%2d]\n",
					st_sensors.general_output[0],
					st_sensors.general_output[1],
					st_sensors.general_output[2],
					st_sensors.general_output[3])
					;
			APP_DBG("general_input    : [%2d : %2d :%2d :%2d :%2d : %2d :%2d :%2d]\n",
					st_sensors.general_input[0],
					st_sensors.general_input[1],
					st_sensors.general_input[2],
					st_sensors.general_input[3],
					st_sensors.general_input[4],
					st_sensors.general_input[5],
					st_sensors.general_input[6],
					st_sensors.general_input[7]);
			APP_DBG("Fan speed        : [%2d : %2d :%2d :%2d]\n",
					st_sensors.fan_pop[0],
					st_sensors.fan_pop[1],
					st_sensors.fan_pop[2],
					st_sensors.fan_pop[3]);
			APP_DBG("power_output_status_1:  %d\n",	 st_sensors.power_output_status[0]);
			APP_DBG("power_output_current_1: %d\n",	st_sensors.power_output_current[0]);
			APP_DBG("power_output_status_2:	 %d\n",	 st_sensors.power_output_status[1]);
			APP_DBG("power_output_current_2: %d\n",	st_sensors.power_output_current[1]);
			APP_PRINT("[-----------------------------]\n");
			APP_DBG("						\n");

			if (ui_setting.sensor_report_en == 1) {

				ak_msg_t* s_msg = ak_memcpy_msg(msg);

				set_if_src_type(s_msg, IF_TYPE_APP_MT);
				set_if_des_type(s_msg, IF_TYPE_APP_GU);
				set_if_src_task_id(s_msg,MT_TASK_SENSOR_ID);
				set_if_des_task_id(s_msg, GU_TASK_CTRL_POP_ID);
				set_if_sig(s_msg, 33);

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
			break;

		case MT_SENSOR_GU_SENSOR_REPORT_UI_REQ: {
			//APP_UI_LOG("MT_SENSOR_GU_SENSOR_REPORT_UI_RES", &ui_setting);
			get_data_common_msg(msg, (uint8_t*)&ui_setting, sizeof(ui_setting_t));
		}
			break;

		case MT_SENSOR_SL_START_GET_INFO_REQ: {
			timer_set(MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ, 1000, TIMER_ONE_SHOT);
		}
			break;

		case MT_SENSOR_SL_STOP_GET_INFO_REQ: {
			timer_remove_attr (MT_TASK_SENSOR_ID, MT_SENSOR_SL_SENSOR_REPORT_REQ);
		}
			break;

		default:
			break;
		}
		/* free message */
		APP_DBG("task sensor: ");
		msg_free(msg);
	}

	return (void*)0;
}
