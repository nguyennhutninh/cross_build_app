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
#include "task_sm.h"

q_msg_t mt_task_sm_mailbox;

tsm_tbl_t tsm_mt_sl;

#define SM_RETRY_COUNTER_MAX			5
static uint32_t sm_retry_sync_counter = 0;

static void mt_sm_sl_sync_req(ak_msg_t*);
static void mt_sm_sl_sync_req_to(ak_msg_t*);
static void mt_sm_sl_sync_res(ak_msg_t*);
static void mt_sm_sensor_report_req(ak_msg_t*);
static void mt_sm_sl_sensor_report_res(ak_msg_t*);
static void mt_sm_sl_init_setting_req(ak_msg_t*);
static void mt_sm_sl_init_setting_res(ak_msg_t*);
static void mt_sm_sl_set_settings_req(ak_msg_t*);
static void mt_sm_sl_set_settings_res(ak_msg_t*);
static void mt_sm_sl_get_settings_req(ak_msg_t*);
static void mt_sm_sl_get_settings_res(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_mode_switch_req(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_mode_switch_res_ok(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_mode_switch_res_err(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_control_req(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_control_res_ok(ak_msg_t*);
static void mt_sm_sl_pop_ctrl_control_res_err(ak_msg_t*);
static void mt_sm_sl_io_control_req(ak_msg_t*);
static void mt_sm_sl_io_control_res(ak_msg_t*);
static void mt_sm_sl_sensor_dbg_report_res(ak_msg_t*);
static void mt_sm_dbg_fatal_test_req(ak_msg_t*);

static void mt_sm_sl_fw_update_req(ak_msg_t*);
static void mt_sm_sl_fw_no_need_update_req(ak_msg_t*);
static void mt_sm_sl_fw_need_update_req(ak_msg_t*);
static void mt_sm_sl_fw_start_tranf_rep(ak_msg_t*);
static void mt_sm_sl_fw_tranf_data_rep(ak_msg_t*);
static void mt_sm_sl_fw_tranf_data_completed_rep(ak_msg_t*);
static void mt_sm_sl_fw_update_completed_rep(ak_msg_t*);
static void mt_sm_sl_fw_update_failed_res(ak_msg_t*);

static void mt_sm_sl_keep_alive(ak_msg_t*);
static void mt_sm_sl_sensor_calib_trans_req(ak_msg_t*);
static void mt_sm_sl_sensor_calib_report_req(ak_msg_t*);
static void mt_sm_sl_sensor_calib_report_res(ak_msg_t*);
static void mt_sm_sensor_dbg_report_req(ak_msg_t*);
static void mt_sm_dbg_fatal_log_read_req(ak_msg_t*);
static void mt_sm_dbg_fatal_log_read_res(ak_msg_t*);
static void mt_sm_dbg_fatal_log_reset_req(ak_msg_t*);
static void mt_sm_dbg_modbus_write_single_register_res(ak_msg_t*);
static void mt_sm_dbg_modbus_slave_list_info_res(ak_msg_t* msg);
static void mt_sm_dbg_modbus_coils_status_info_res(ak_msg_t* msg);
static void mt_sm_sl_modbus_control_req(ak_msg_t* msg);

tsm_t mt_sl_power_on[] = {
	{ MT_SM_SL_SYNC_REQ								,	MT_SL_POWER_ON				,	mt_sm_sl_sync_req							},
	{ MT_SM_SL_SYNC_REQ_TO							,	MT_SL_POWER_ON				,	mt_sm_sl_sync_req_to						},
	{ MT_SM_SL_SYNC_RES								,	MT_SL_IDLE					,	mt_sm_sl_sync_res							},
	{ MT_SM_SENSOR_REPORT_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_REPORT_RES					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SENSOR_DBG_REPORT_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_DBG_REPORT_RES				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_TEST_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_READ_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_READ_RES					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_RESET_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES	,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES			,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_COILS_STATUS_INFO_RES		,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_MODBUS_CONTROL_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_INIT_SETTING_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_INIT_SETTING_RES						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SET_SETTINGS_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SET_SETTINGS_RES						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_GET_SETTINGS_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_GET_SETTINGS_RES						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_REQ				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_OK			,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_ERR			,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_OK				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_ERR				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_IO_CONTROL_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_IO_CONTROL_RES						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_REQ						,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_NO_NEED_UPDATE_REQ				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_NEED_UPDATE_REQ					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_START_TRANF_REP					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_TRANF_DATA_REP					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_TRANF_DATA_COMPLETED_REP			,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_COMPLETED_REP				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_FAILED_RES					,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_KEEP_ALIVE							,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_TRANS_REQ				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_REQ				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_RES				,	MT_SL_POWER_ON				,	TSM_FUNCTION_NULL							},
};

tsm_t mt_sl_idle[] = {
	{ MT_SM_SL_SYNC_REQ								,	MT_SL_POWER_ON				,	mt_sm_sl_sync_req							},
	{ MT_SM_SL_SYNC_REQ_TO							,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SYNC_RES								,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SENSOR_REPORT_REQ						,	MT_SL_IDLE					,	mt_sm_sensor_report_req						},
	{ MT_SM_SL_SENSOR_REPORT_RES					,	MT_SL_IDLE					,	mt_sm_sl_sensor_report_res					},
	{ MT_SM_SENSOR_DBG_REPORT_REQ					,	MT_SL_IDLE					,	mt_sm_sensor_dbg_report_req					},
	{ MT_SM_SL_SENSOR_DBG_REPORT_RES				,	MT_SL_IDLE					,	mt_sm_sl_sensor_dbg_report_res				},
	{ MT_SM_DBG_FATAL_TEST_REQ						,	MT_SL_IDLE					,	mt_sm_dbg_fatal_test_req					},
	{ MT_SM_DBG_FATAL_LOG_READ_REQ					,	MT_SL_IDLE					,	mt_sm_dbg_fatal_log_read_req				},
	{ MT_SM_DBG_FATAL_LOG_READ_RES					,	MT_SL_IDLE					,	mt_sm_dbg_fatal_log_read_res				},
	{ MT_SM_DBG_FATAL_LOG_RESET_REQ					,	MT_SL_IDLE					,	mt_sm_dbg_fatal_log_reset_req				},
	{ MT_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES	,	MT_SL_IDLE					,	mt_sm_dbg_modbus_write_single_register_res	},
	{ MT_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES			,	MT_SL_IDLE					,	mt_sm_dbg_modbus_slave_list_info_res		},
	{ MT_SM_DBG_MODBUS_COILS_STATUS_INFO_RES		,	MT_SL_IDLE					,	mt_sm_dbg_modbus_coils_status_info_res		},
	{ MT_SM_SL_MODBUS_CONTROL_REQ					,	MT_SL_IDLE					,	mt_sm_sl_modbus_control_req					},
	{ MT_SM_SL_INIT_SETTING_REQ						,	MT_SL_IDLE					,	mt_sm_sl_init_setting_req					},
	{ MT_SM_SL_INIT_SETTING_RES						,	MT_SL_IDLE					,	mt_sm_sl_init_setting_res					},
	{ MT_SM_SL_SET_SETTINGS_REQ						,	MT_SL_IDLE					,	mt_sm_sl_set_settings_req					},
	{ MT_SM_SL_SET_SETTINGS_RES						,	MT_SL_IDLE					,	mt_sm_sl_set_settings_res					},
	{ MT_SM_SL_GET_SETTINGS_REQ						,	MT_SL_IDLE					,	mt_sm_sl_get_settings_req					},
	{ MT_SM_SL_GET_SETTINGS_RES						,	MT_SL_IDLE					,	mt_sm_sl_get_settings_res					},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_REQ				,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_mode_switch_req			},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_OK			,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_mode_switch_res_ok		},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_ERR			,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_mode_switch_res_err		},
	{ MT_SM_SL_POP_CTRL_CONTROL_REQ					,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_control_req				},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_OK				,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_control_res_ok			},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_ERR				,	MT_SL_IDLE					,	mt_sm_sl_pop_ctrl_control_res_err			},
	{ MT_SM_SL_IO_CONTROL_REQ						,	MT_SL_IDLE					,	mt_sm_sl_io_control_req						},
	{ MT_SM_SL_IO_CONTROL_RES						,	MT_SL_IDLE					,	mt_sm_sl_io_control_res						},
	{ MT_SM_SL_FW_UPDATE_REQ						,	MT_SL_IDLE					,	mt_sm_sl_fw_update_req						},
	{ MT_SM_SL_FW_NO_NEED_UPDATE_REQ				,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_NEED_UPDATE_REQ					,	MT_SL_FW					,	mt_sm_sl_fw_need_update_req					},
	{ MT_SM_SL_FW_START_TRANF_REP					,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_TRANF_DATA_REP					,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_TRANF_DATA_COMPLETED_REP			,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_COMPLETED_REP				,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_FAILED_RES					,	MT_SL_IDLE					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_KEEP_ALIVE							,	MT_SL_IDLE					,	mt_sm_sl_keep_alive							},
	{ MT_SM_SL_SENSOR_CALIB_TRANS_REQ				,	MT_SL_IDLE					,	mt_sm_sl_sensor_calib_trans_req				},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_REQ				,	MT_SL_IDLE					,	mt_sm_sl_sensor_calib_report_req			},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_RES				,	MT_SL_IDLE					,	mt_sm_sl_sensor_calib_report_res			},
};

tsm_t mt_sl_fw[] = {
	{ MT_SM_SL_SYNC_REQ								,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SYNC_REQ_TO							,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SYNC_RES								,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SENSOR_REPORT_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_REPORT_RES					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SENSOR_DBG_REPORT_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_DBG_REPORT_RES				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_TEST_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_READ_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_READ_RES					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_FATAL_LOG_RESET_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES	,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES			,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_DBG_MODBUS_COILS_STATUS_INFO_RES		,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_MODBUS_CONTROL_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_INIT_SETTING_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_INIT_SETTING_RES						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SET_SETTINGS_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SET_SETTINGS_RES						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_GET_SETTINGS_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_GET_SETTINGS_RES						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_REQ				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_OK			,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_ERR			,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_OK				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_POP_CTRL_CONTROL_RES_ERR				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_IO_CONTROL_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_IO_CONTROL_RES						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_UPDATE_REQ						,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_NO_NEED_UPDATE_REQ				,	MT_SL_FW					,	mt_sm_sl_fw_no_need_update_req				},
	{ MT_SM_SL_FW_NEED_UPDATE_REQ					,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_FW_START_TRANF_REP					,	MT_SL_FW					,	mt_sm_sl_fw_start_tranf_rep					},
	{ MT_SM_SL_FW_TRANF_DATA_REP					,	MT_SL_FW					,	mt_sm_sl_fw_tranf_data_rep					},
	{ MT_SM_SL_FW_TRANF_DATA_COMPLETED_REP			,	MT_SL_FW					,	mt_sm_sl_fw_tranf_data_completed_rep		},
	{ MT_SM_SL_FW_UPDATE_COMPLETED_REP				,	MT_SL_IDLE					,	mt_sm_sl_fw_update_completed_rep			},
	{ MT_SM_SL_FW_UPDATE_FAILED_RES					,	MT_SL_IDLE					,	mt_sm_sl_fw_update_failed_res				},
	{ MT_SM_SL_KEEP_ALIVE							,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_TRANS_REQ				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_REQ				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
	{ MT_SM_SL_SENSOR_CALIB_REPORT_RES				,	MT_SL_FW					,	TSM_FUNCTION_NULL							},
};

tsm_t* tsm_mt_sl_table[] {
	mt_sl_power_on,
	mt_sl_idle,
	mt_sl_fw
};

void* mt_task_sm_entry(void*) {
	wait_all_tasks_started();

	/* init table MT_SL state-machine */
	tsm_init(&tsm_mt_sl, tsm_mt_sl_table, MT_SL_POWER_ON);
	tsm_mt_sl.on_state = tsm_mt_sl_on_state;

	timer_set(MT_TASK_SM_ID, MT_SM_SL_SYNC_REQ, MT_SM_SL_SYNC_REQ_INTERVAL, TIMER_ONE_SHOT);

	timer_set(MT_TASK_SM_ID, MT_SM_SL_KEEP_ALIVE, MT_SM_SL_KEEP_ALIVE_INTERVAL, TIMER_PERIODIC);

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_SM_ID);

		tsm_dispatch(&tsm_mt_sl, msg);

		/* free message */
		msg_free(msg);
	}

	return (void*)0;
}

void tsm_mt_sl_on_state(tsm_state_t state) {
	switch(state) {
	case MT_SL_POWER_ON: {
		//APP_DBG("[tsm_mt_sl_on_state] MT_SL_POWER_ON\n");
	}
		break;

	case MT_SL_IDLE: {
		//APP_DBG("[tsm_mt_sl_on_state] MT_SL_IDLE\n");
	}
		break;

	case MT_SL_FW: {
		//APP_DBG("[tsm_mt_sl_on_state] MT_SL_FW\n");
	}
		break;

	default:
		break;
	}
}

void mt_sm_sl_sync_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_sync_req\n");
	uint8_t sync_data = 0xEF;

	ak_msg_t* s_msg = get_common_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_SM_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, MT_SM_SL_SYNC_REQ);
	set_if_data_common_msg(s_msg, (uint8_t*)&sync_data, sizeof(uint8_t));

	set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);

	timer_set(MT_TASK_SM_ID, MT_SM_SL_SYNC_REQ_TO, MT_SM_SL_SYNC_REQ_TO_INTERVAL, TIMER_ONE_SHOT);
}

void mt_sm_sl_sync_req_to(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_sync_req_to\n");

	if (sm_retry_sync_counter++ < SM_RETRY_COUNTER_MAX) {
		timer_set(MT_TASK_SM_ID, MT_SM_SL_SYNC_REQ, MT_SM_SL_SYNC_REQ_INTERVAL, TIMER_ONE_SHOT);
	}
	else {
		sm_retry_sync_counter = 0;

		uint8_t error_code = APP_ERROR_CODE_TIMEOUT;

		{
			ak_msg_t* s_msg = get_common_msg();
			set_msg_sig(s_msg, MT_CLOUD_SL_SYNC_ERR);
			set_data_common_msg(s_msg, (uint8_t*)&error_code, sizeof(uint8_t));
			set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
			task_post(MT_TASK_CLOUD_ID, s_msg);
		}

		{
			ak_msg_t* s_msg = get_common_msg();
			set_msg_sig(s_msg, MT_SNMP_SL_SYNC_ERR);
			set_data_common_msg(s_msg, (uint8_t*)&error_code, sizeof(uint8_t));
			set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
			task_post(MT_TASK_SNMP_ID, s_msg);
		}

		timer_set(MT_TASK_SM_ID, MT_SM_SL_SYNC_REQ, MT_SM_SL_SYNC_REQ_INTERVAL, TIMER_ONE_SHOT);
	}
}

void mt_sm_sl_sync_res(ak_msg_t*) {
	APP_DBG("[SL_SM] MT_SM_SL_SYNC_RES\n");

	sm_retry_sync_counter = 0;
	timer_remove_attr(MT_TASK_SM_ID, MT_SM_SL_SYNC_REQ_TO);

	{
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_CLOUD_SL_SYNC_OK);
		set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
		task_post(MT_TASK_CLOUD_ID, s_msg);
	}

	{
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_SNMP_SL_SYNC_OK);
		set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
		task_post(MT_TASK_SNMP_ID, s_msg);
	}

	{
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_SENSOR_SL_START_GET_INFO_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
		task_post(MT_TASK_SENSOR_ID, s_msg);
	}
}

void mt_sm_sensor_report_req(ak_msg_t*) {
	//APP_DBG("mt_sm_sensor_report_req\n");
	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_SENSOR_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_SENSOR_REPORT_REQ);

	set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_sensor_dbg_report_req(ak_msg_t*) {

	//APP_DBG("[SL_SM] mt_sm_sensor_dbg_report_req\n");
	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_SENSOR_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_SENSOR_DBG_REPORT_REQ);

	set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_sl_sensor_dbg_report_res(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_sl_sensor_dbg_report_res\n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);

	set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_DBG_REPORT_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_SENSOR_ID, s_msg);
}

void mt_sm_sl_sensor_report_res(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_sl_sensor_report_res\n");

	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_REPORT_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_SENSOR_ID, s_msg);
}

void mt_sm_dbg_fatal_test_req(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_dbg_fatal_test_req \n");
	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_DBG_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_DBG_FATAL_TEST_REQ);

	set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_dbg_fatal_log_read_req(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_dbg_fatal_log_read_req \n");
	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_DBG_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_DBG_FATAL_LOG_READ_REQ);

	set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_dbg_fatal_log_read_res(ak_msg_t* msg) {

	//APP_DBG("[SL_SM] mt_sm_dbg_fatal_log_read_res \n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_DBG_SL_FATAL_LOG_READ_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
}

void mt_sm_dbg_fatal_log_reset_req(ak_msg_t* msg) {

	//APP_DBG("[SL_SM] mt_sm_dbg_fatal_log_reset_req \n");
	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_des_task_id(s_msg, SL_TASK_DBG_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_DBG_FATAL_LOG_RESET_REQ);

	set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_dbg_modbus_write_single_register_res(ak_msg_t* msg) {

	//APP_DBG("[SL_SM] mt_sm_dbg_modbus_write_single_register_res \n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_MODBUS_ID, s_msg);
}

void mt_sm_dbg_modbus_slave_list_info_res(ak_msg_t* msg) {

	//APP_DBG("[SL_SM] mt_sm_dbg_modbus_slave_list_info_res \n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_DBG_MODBUS_SLAVE_LIST_INFO_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_MODBUS_ID, s_msg);
}

void mt_sm_dbg_modbus_coils_status_info_res(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_dbg_modbus_coils_status_info_res \n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_MODBUS_READ_COIL_RES);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_MODBUS_ID, s_msg);
}

void mt_sm_sl_modbus_control_req(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_sl_modbus_control_req \n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);

	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
	set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_sig(s_msg, SL_SENSOR_MODBUS_REGISTER_INIT_REQ);

	set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_sl_init_setting_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_init_setting_req\n");
}

void mt_sm_sl_init_setting_res(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_init_setting_res\n");
}

void mt_sm_sl_set_settings_req(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_set_settings_req\n");
}

void mt_sm_sl_set_settings_res(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_set_settings_res\n");
}

void mt_sm_sl_get_settings_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_get_settings_req\n");
}

void mt_sm_sl_get_settings_res(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_get_settings_res\n");
}

void mt_sm_sl_pop_ctrl_mode_switch_req(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_mode_switch_req\n");
}

void mt_sm_sl_pop_ctrl_mode_switch_res_ok(ak_msg_t*) {
	//APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_mode_switch_res_ok\n");
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, MT_CLOUD_POP_CTRL_MODE_SWITCH_RES_OK);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_CLOUD_ID, s_msg);
}

void mt_sm_sl_pop_ctrl_mode_switch_res_err(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_mode_switch_res_err\n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_CLOUD_POP_CTRL_MODE_SWITCH_RES_ERR);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_CLOUD_ID, s_msg);
}

void mt_sm_sl_pop_ctrl_control_req(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_control_req\n");
}

void mt_sm_sl_pop_ctrl_control_res_ok(ak_msg_t*) {
	//APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_control_res_ok\n");
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, MT_CLOUD_POP_CTRL_CONTROL_RES_OK);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_CLOUD_ID, s_msg);
}

void mt_sm_sl_pop_ctrl_control_res_err(ak_msg_t* msg) {
	//APP_DBG("[SL_SM] mt_sm_sl_pop_ctrl_control_res_err\n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);
	set_msg_sig(s_msg, MT_CLOUD_POP_CTRL_CONTROL_RES_ERR);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_CLOUD_ID, s_msg);
}

void mt_sm_sl_io_control_req(ak_msg_t* msg) {
	APP_DBG("[SL_SM] mt_sm_sl_io_control_req\n");

	ak_msg_t* s_msg = ak_memcpy_msg(msg);

	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
	set_if_des_task_id(s_msg, SL_TASK_IO_CTRL_ID);
	set_if_src_task_id(s_msg, MT_TASK_SM_ID);
	set_if_sig(s_msg, SL_IO_CTRL_CONTROL_REQ);

	set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);

}

void mt_sm_sl_io_control_res(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_io_control_res\n");
}

static void mt_sm_sl_fw_update_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_update_req\n");
}

static void mt_sm_sl_fw_no_need_update_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_no_need_update_req\n");
}

static void mt_sm_sl_fw_need_update_req(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_need_update_req\n");
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, MT_FIRMWARE_SL_FW_TRANF_REQ);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_FIRMWARE_ID, s_msg);
}

void mt_sm_sl_fw_start_tranf_rep(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_start_tranf_rep\n");
}

void mt_sm_sl_fw_tranf_data_rep(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_tranf_data_rep\n");
}

void mt_sm_sl_fw_tranf_data_completed_rep(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_tranf_data_completed_rep\n");
}

void mt_sm_sl_fw_update_completed_rep(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_update_completed_rep\n");

	{
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_SENSOR_SL_START_GET_INFO_REQ);
		task_post(MT_TASK_SENSOR_ID, s_msg);
	}

	{
		ak_msg_t* s_msg = get_pure_msg();
		set_if_src_task_id(s_msg, MT_TASK_SM_ID);
		set_if_des_task_id(s_msg, GU_TASK_FIRMWARE_ID);
		set_if_des_type(s_msg, IF_TYPE_APP_GU);
		set_if_sig(s_msg, GU_FIRMWARE_UPDATE_COMPLETED);

		set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
		task_post(MT_TASK_IF_ID, s_msg);
	}
}

void mt_sm_sl_fw_update_failed_res(ak_msg_t*) {
	APP_DBG("[SL_SM] mt_sm_sl_fw_update_failed_res\n");
}

void mt_sm_sl_keep_alive(ak_msg_t*) {
}

void mt_sm_sl_sensor_calib_trans_req(ak_msg_t* msg) {
	APP_DBG("[MT_SM] mt_sm_sl_sensor_calib_trans_req\n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);

	set_if_src_task_id(s_msg, MT_TASK_SENSOR_ID);
	set_if_des_task_id(s_msg, SL_TASK_SM_ID);
	set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg, SL_SM_SENSOR_CALIB_TRANS_REQ);

	set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_sl_sensor_calib_report_req(ak_msg_t*) {
	APP_DBG("[MT_SM] mt_sm_sl_sensor_calib_report_req\n");

	ak_msg_t* s_msg = get_pure_msg();

	set_if_src_task_id(s_msg,	MT_TASK_SM_ID);
	set_if_des_task_id(s_msg,	SL_TASK_SM_ID);
	set_if_src_type(s_msg,		IF_TYPE_CPU_SERIAL_MT);
	set_if_des_type(s_msg,		IF_TYPE_CPU_SERIAL_SL);
	set_if_sig(s_msg,			SL_SM_SENSOR_CALIB_REPORT_REQ);

	set_msg_sig(s_msg,			MT_IF_PURE_MSG_OUT);
	set_msg_src_task_id(s_msg,	MT_TASK_SM_ID);
	task_post(MT_TASK_IF_ID, s_msg);
}

void mt_sm_sl_sensor_calib_report_res(ak_msg_t* msg) {
	APP_DBG("[MT_SM] mt_sm_sl_sensor_calib_report_res\n");
	ak_msg_t* s_msg = ak_memcpy_msg(msg);

	set_msg_src_task_id(s_msg, MT_TASK_SM_ID);
	set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_CALIB_INFO_RES);
	task_post(MT_TASK_SENSOR_ID, s_msg);
}
