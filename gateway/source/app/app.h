#ifndef __APP_H__
#define __APP_H__

#include <string>
#include "app_config.h"

using namespace std;

/*****************************************************************************/
/* task MT_SYS define.
 */
/*****************************************************************************/
/* define timer */
/* define signal */
#define MT_SYS_WATCH_DOG_REPORT_REQ					(1)

/*****************************************************************************/
/*  task MT_RF24 define.
 */
/*****************************************************************************/
/* define timer */
#define MT_RF24_IF_TIMER_PACKET_DELAY_INTERVAL		(100)

/* define signal */
#define MT_RF24_IF_PURE_MSG_OUT						(1)
#define MT_RF24_IF_COMMON_MSG_OUT					(2)
#define MT_RF24_IF_TIMER_PACKET_DELAY				(3)

/*****************************************************************************/
/*  task MT_CONSOLE define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
#define MT_CONSOLE_INTERNAL_LOGIN_CMD				(1)

/*****************************************************************************/
/* task MT_IF define
 */
/*****************************************************************************/
/* define timer */
enum {
	/* define signal */
	MT_IF_PURE_MSG_IN = 1,
	MT_IF_PURE_MSG_OUT,
	MT_IF_COMMON_MSG_IN,
	MT_IF_COMMON_MSG_OUT,
	MT_IF_DYNAMIC_MSG_IN,
	MT_IF_DYNAMIC_MSG_OUT
};
/*****************************************************************************/
/* task MT_CLOUD define.
 */
/*****************************************************************************/
/* define timer */

enum {
	/* define signal */
	MT_CLOUD_SL_SYNC_OK = 1					,
	MT_CLOUD_SL_SYNC_ERR					,
	MT_CLOUD_SL_SENSOR_REPORT_REP			,
	MT_CLOUD_MQTT_AIRCOND_CONTROL_REQ		,
	MT_CLOUD_MQTT_SET_SL_SETTINGS_REQ		,
	MT_CLOUD_SYNC_LS_SETTINGS_REP			,
	MT_CLOUD_POP_CTRL_MODE_SWITCH_RES_OK	,
	MT_CLOUD_POP_CTRL_MODE_SWITCH_RES_ERR,
	MT_CLOUD_POP_CTRL_CONTROL_RES_OK		,
	MT_CLOUD_POP_CTRL_CONTROL_RES_ERR		,
	MT_CLOUD_POP_CTRL_GET_INFO_REQ			,
	MT_CLOUD_POP_CTRL_GET_INFO_RES			,
	MT_CLOUD_IO_CTRL_GET_INFO_REQ			,
	MT_CLOUD_IO_CTRL_GET_INFO_RES			,
	MT_CLOUD_IO_CTRL_CONTROL_REQ			,
	MT_CLOUD_MQTT_FLOOD_SENSOR_REP			,
	MT_CLOUD_MQTT_FLOOD_BAT_REP
};

/*****************************************************************************/
/* task MT_SNMP define.
 */
/*****************************************************************************/
/* define timer */
/* define signal */
enum {
	MT_SNMP_SL_SYNC_OK = 1,
	MT_SNMP_SL_SYNC_ERR,
	MT_SNMP_SL_SENSOR_REPORT_REP,
	MT_SNMP_SYNC_LS_SETTINGS_REP
};
/*****************************************************************************/
/* task MT_SM define
 */
/*****************************************************************************/
/* define timer */
#define MT_SM_SL_SYNC_REQ_INTERVAL							(3000)
#define MT_SM_SL_SYNC_REQ_TO_INTERVAL						(2000)
#define MT_SM_SL_KEEP_ALIVE_INTERVAL						(1000)

enum {
	/* define signal */
	MT_SM_SL_SYNC_REQ = 1,
	MT_SM_SL_SYNC_REQ_TO,
	MT_SM_SL_SYNC_RES,
	MT_SM_SENSOR_REPORT_REQ,
	MT_SM_SL_SENSOR_REPORT_RES,
	MT_SM_SL_INIT_SETTING_REQ,
	MT_SM_SL_INIT_SETTING_RES,
	MT_SM_SL_SET_SETTINGS_REQ,
	MT_SM_SL_SET_SETTINGS_RES,
	MT_SM_SL_GET_SETTINGS_REQ,
	MT_SM_SL_GET_SETTINGS_RES,

	MT_SM_SL_POP_CTRL_MODE_SWITCH_REQ,
	MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_OK,
	MT_SM_SL_POP_CTRL_MODE_SWITCH_RES_ERR,
	MT_SM_SL_POP_CTRL_CONTROL_REQ,
	MT_SM_SL_POP_CTRL_CONTROL_RES_OK,
	MT_SM_SL_POP_CTRL_CONTROL_RES_ERR,
	MT_SM_SL_IO_CONTROL_REQ,
	MT_SM_SL_IO_CONTROL_RES,

	MT_SM_SL_FW_UPDATE_REQ,
	MT_SM_SL_FW_NO_NEED_UPDATE_REQ,
	MT_SM_SL_FW_NEED_UPDATE_REQ,
	MT_SM_SL_FW_START_TRANF_REP,
	MT_SM_SL_FW_TRANF_DATA_REP,
	MT_SM_SL_FW_TRANF_DATA_COMPLETED_REP,
	MT_SM_SL_FW_UPDATE_COMPLETED_REP,
	MT_SM_SL_FW_UPDATE_FAILED_RES,

	MT_SM_SL_KEEP_ALIVE,

	MT_SM_SL_SENSOR_CALIB_TRANS_REQ,
	MT_SM_SL_SENSOR_CALIB_REPORT_REQ,
	MT_SM_SL_SENSOR_CALIB_REPORT_RES,
	MT_SM_SENSOR_DBG_REPORT_REQ,
	MT_SM_SL_SENSOR_DBG_REPORT_RES,
	MT_SM_DBG_FATAL_TEST_REQ,
	MT_SM_DBG_FATAL_LOG_READ_REQ,
	MT_SM_DBG_FATAL_LOG_READ_RES,
	MT_SM_DBG_FATAL_LOG_RESET_REQ,
	MT_SM_MODBUS_WRITE_SINGLE_REGISTER_REQ,
	MT_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES,
	MT_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES,
	MT_SM_SL_LOG_DBG_SHOW_RES,
	MT_SM_DBG_MODBUS_COILS_STATUS_INFO_RES,
	MT_SM_SL_MODBUS_CONTROL_REQ,
	MT_SM_MODBUS_READ_COIL_RES
};

/*****************************************************************************/
/* task MT_FIRMWARE define.
 */
/*****************************************************************************/
/* define timer */
#define MT_FIRMWARE_SL_FW_TRANF_DATA_REQ_INTERVAL			(100)
#define MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ_INTERVAL		(8000)
#define MT_FIRMWARE_SL_FW_CHECKSUM_TIMEOUT_REQ_INTERVAL		(10000)
enum {
	/* define signal */
	MT_FIRMWARE_SL_FW_UPDATE_REQ = 1			,
	MT_FIRMWARE_SL_FW_INFO_RES					,
	MT_FIRMWARE_SL_FW_TRANF_REQ					,
	MT_FIRMWARE_SL_FW_TRANF_RES					,
	MT_FIRMWARE_SL_FW_TRANF_DATA_RES			,
	MT_FIRMWARE_SL_FW_TRANF_DATA_REQ			,
	MT_FIRMWARE_SL_FW_CHECKSUM_CORRECT_RES		,
	MT_FIRMWARE_SL_FW_CHECKSUM_INCORRECT_RES	,
	MT_FIRMWARE_SL_FW_UPDATE_COMPLETED_REP		,
	MT_FIRMWARE_SL_FW_PACKED_TIMEOUT_REQ		,
	MT_FIRMWARE_SL_FW_OTA_REQ
};
/*****************************************************************************/
/* task MT_SENSOR define.
 */
/*****************************************************************************/
/* define timer */
#define MT_SENSOR_SL_SENSOR_REPORT_REQ_INTERVAL				(10000)
#define MT_SENSOR_SL_SENSOR_REPORT_REQ_TO_INTERVAL			(2000)
#define MT_SENSOR_SL_SENSOR_START_REPORT_REQ				(1000)

enum{
	/* define signal */
	MT_SENSOR_SL_SENSOR_REPORT_REQ = 1,
	MT_SENSOR_SL_SENSOR_REPORT_REQ_TO,
	MT_SENSOR_SL_SENSOR_REPORT_RES,
	MT_SENSOR_SL_SENSOR_CALIB_TRANS_REQ,
	MT_SENSOR_SL_SENSOR_CALIB_INFO_REQ,
	MT_SENSOR_SL_SENSOR_CALIB_INFO_RES,
	MT_SENSOR_SL_SENSOR_MODBUS_RES,
	MT_SENSOR_SL_SENSOR_DBG_REPORT_REQ,
	MT_SENSOR_SL_SENSOR_DBG_REPORT_RES,
	MT_SENSOR_GU_SENSOR_REPORT_UI_REQ,
	MT_SENSOR_SL_START_GET_INFO_REQ,
	MT_SENSOR_SL_STOP_GET_INFO_REQ
};

/*****************************************************************************/
/* task dbg msg define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
enum {
	MT_DBG_SL_FATAL_TEST_REQ = 1	,
	MT_DBG_SL_FATAL_LOG_READ_REQ	,
	MT_DBG_SL_FATAL_LOG_READ_RES	,
	MT_DBG_SL_FATAL_LOG_RESET_REQ
};

/*****************************************************************************/
/* task SL_DBG define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
#define SL_DBG_FATAL_TEST_REQ						(1)

#define SL_DBG_FATAL_LOG_READ_REQ					(2)
#define SL_DBG_FATAL_LOG_READ_RES					(3)

#define SL_DBG_FATAL_LOG_RESET_REQ					(4)

/*****************************************************************************/
/* task RS485_MODBUS define
 */
/*****************************************************************************/
/* define timer */
#define MT_MODBUS_SLAVE_RESPONES_TIMEOUT_INTERVAL	(30)
#define MT_SENSOR_MODBUS_GET_INFO_REQ_INTERVAL		(250)
enum {
	/* define signal */
	MT_SENSOR_MODBUS_REGISTER_INIT_REQ = 1,
	MT_MODBUS_SINGLE_REGISTER_WRITE_REQ			,
	MT_SENSOR_MODBUS_QUERY_REQ					,
	MT_SENSOR_MODBUS_GET_INFO_REQ				,
	MT_DBG_MODBUS_WRITE_SINGLE_REGISTER_REQ		,
	MT_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES		,
	MT_DBG_MODBUS_RESET_SLAVE_REQ				,
	MT_DBG_MODBUS_SLAVE_LIST_INFO_REQ			,
	MT_DBG_MODBUS_SLAVE_LIST_INFO_RES			,
	MT_MODBUS_TOGGLE_SINGLE_REGISTER_REQ		,
	MT_MODBUS_FUNC_1_RES						,
	MT_MODBUS_FUNC_3_RES						,
	MT_DBG_MODBUS_WRITE_SINGLE_COIL_REQ			,
	MT_MODBUS_READ_COIL_REQ,
	MT_MODBUS_READ_COIL_RES
};

/*****************************************************************************/
/*  global define variable
 */
/*****************************************************************************/
#define APP_OK												(0x00)
#define APP_NG												(0x01)

#define APP_FLAG_OFF										(0x00)
#define APP_FLAG_ON											(0x01)

#define APP_ROOT_PATH_DISK									"/root/fpt_gateway"
#define APP_ROOT_PATH_RAM									"/run/fpt_gateway"

extern app_config gateway_configure;
extern app_config_parameter_t gateway_configure_parameter;

#endif // __APP_H__
