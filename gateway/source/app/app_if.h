/**
 ******************************************************************************
 * @Author: ThanNT
 * @Date:   13/08/2016
 ******************************************************************************
**/

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "app_if.h"

/*****************************************************************************/
/* task SL_LIFE define
 */
/*****************************************************************************/
/* define timer */
#define SL_LIFE_TASK_TIMER_LED_LIFE_INTERVAL		(1000)

/* define signal */
#define SL_LIFE_SYSTEM_CHECK						(0)

/*****************************************************************************/
/* task SL_SHELL define
 */
/*****************************************************************************/
/* define timer */

/* define signal */
#define SL_SHELL_LOGIN_CMD							(0)

/*****************************************************************************/
/* task SL_CPU_SERIAL define
 */
/*****************************************************************************/
/* timer signal */
/* define signal */
#define SL_CPU_SERIAL_IF_PURE_MSG_OUT				(1)
#define SL_CPU_SERIAL_IF_COMMON_MSG_OUT				(2)

/*****************************************************************************/
/* task SL_IF define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
#define SL_IF_PURE_MSG_IN							(1)
#define SL_IF_PURE_MSG_OUT							(2)
#define SL_IF_COMMON_MSG_IN							(3)
#define SL_IF_COMMON_MSG_OUT						(4)

/*****************************************************************************/
/* task SL_SM define
 */
/*****************************************************************************/
/* define timer */
#define SL_SM_MASTER_KEEP_ALIVE_TO_INTERVAL			(200)

/* define signal */
#define SL_SM_MT_SYNC_REQ							(1)
#define SL_SM_FIRMWARE_UPDATE_REQ					(2)
#define SL_SM_FIRMWARE_UPDATE_TIMEOUT_REQ			(3)
#define SL_SM_FIRMWARE_UPDATE_COMPLETE_REQ			(4)
#define SL_SM_FIRMWARE_UPDATE_ERROR_REQ				(5)
#define SL_SM_MASTER_KEEP_ALIVE						(6)
#define SL_SM_MASTER_KEEP_ALIVE_TO					(7)

#define SL_SM_SENSOR_CALIB_TRANS_REQ				(8)
#define SL_SM_SENSOR_CALIB_TRANS_RES				(9)

#define SL_SM_SENSOR_CALIB_REPORT_REQ				(10)
#define SL_SM_SENSOR_CALIB_REPORT_RES				(11)

/*****************************************************************************/
/* task SL_SENSOR define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
#define SL_SENSOR_REPORT_REQ						(1)
#define SL_SENSOR_DBG_REPORT_REQ					(2)

#define SL_SENSOR_CALIB_TRANS_REQ					(3)
#define SL_SENSOR_CALIB_TRANS_RES					(4)

#define SL_SENSOR_CALIB_REPORT_REQ					(5)
#define SL_SENSOR_CALIB_REPORT_RES					(6)

#define SL_SENSOR_DBG_TEST_REPORT_REQ				(7)
#define SL_SENSOR_DBG_TEST_REPORT_RES				(8)

/*****************************************************************************/
/* task firmware define
 */
/*****************************************************************************/
/* define timer */
#define SL_FIRMWARE_CHECKING_REQ_INTERVAL			(500)
#define SL_FIRMWARE_PACKED_TIMEOUT_REQ_INTERVAL		(10000)

/* define signal */
#define SL_FIRMWARE_FW_INFO_REQ						(1)
#define SL_FIRMWARE_SL_FW_TRANF_REQ					(2)
#define SL_FIRMWARE_SL_FW_TRANF_DATA_REQ			(3)
#define SL_FIRMWARE_SL_FW_CALC_CHECKSUM_REQ			(4)
#define SL_FIRMWARE_SL_FW_INTERNAL_UPDATE_REQ		(5)
#define SL_FIRMWARE_CHECKING_REQ					(6)
#define SL_FIRMWARE_PACKED_TIMEOUT_REQ				(7)

/*****************************************************************************/
/* task SL_POP_CTRL define
 */
/*****************************************************************************/
/* define timer */
#define SL_POP_CTRL_AUTO_CONTROL_REQ_INTERVAL		(10000)
#define SL_POP_CTRL_AC_DOWN_CHECK_REQ_INTERVAL		(11000)

/* define signal */
#define SL_POP_CTRL_MODE_SWITCH_REQ					(1)
#define SL_POP_CTRL_CONTROL_REQ						(2)
#define SL_POP_CTRL_CONTROL_INFO_REQ				(3)
#define SL_POP_CTRL_AUTO_CONTROL_REQ				(4)
#define SL_POP_CTRL_AC_DOWN_CHECK_REQ				(5)

/*****************************************************************************/
/* task SL_IO_POP_CTRL define
 */
/*****************************************************************************/
/* define timer */
/* define signal */
#define SL_IO_CTRL_CONTROL_REQ						(1)
#define SL_IO_CTRL_CONTROL_INFO_REQ					(2)

/*****************************************************************************/
/* task SL_DEV_CTRL define
 */
/*****************************************************************************/
/* define timer */
#define SL_DEV_CTRL_FAN_AUTO_CONTROL_REQ_INTERVAL	(3000)

/* define signal */
#define SL_DEV_CTRL_FAN_SPEED_SET_REQ				(1)
#define SL_DEV_CTRL_FAN_AUTO_CONTROL_REQ			(2)

/*****************************************************************************/
/*  global define variable
 */
/*****************************************************************************/
#define APP_OK										(0x00)
#define APP_NG										(0x01)

#define APP_FLAG_OFF								(0x00)
#define APP_FLAG_ON									(0x01)

/*****************************************************************************/
/* task RS485_MODBUS define
 */
/*****************************************************************************/
/* define timer */
#define SL_MODBUS_SLAVE_RESPONES_TIMEOUT_INTERVAL	(30)

/* define signal */
#define SL_SENSOR_MODBUS_REGISTER_INIT_REQ			(1)
#define SL_MODBUS_SINGLE_REGISTER_WRITE_REQ			(2)
#define SL_SENSOR_MODBUS_QUERY_REQ					(3)
#define SL_SENSOR_MODBUS_GET_INFO_REQ				(4)
#define SL_DBG_MODBUS_WRITE_SINGLE_REGISTER_REQ		(5)
#define SL_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES		(6)
#define SL_DBG_MODBUS_RESET_SLAVE_REQ				(7)
#define SL_DBG_MODBUS_SLAVE_LIST_INFO_REQ			(8)
#define SL_DBG_MODBUS_SLAVE_LIST_INFO_RES			(9)
#define SL_MODBUS_TOGGLE_SINGLE_REGISTER_REQ		(10)
#define SL_MODBUS_FUNC_1_RES						(11)
#define SL_MODBUS_FUNC_3_RES						(12)
#define SL_DBG_MODBUS_WRITE_SINGLE_COIL_REQ			(13)
#define SL_MODBUS_READ_COIL_REQ						(14)
#define SL_MODBUS_READ_COIL_RES						(15)

/* define signal */
enum {
	/* define signal */
	GU_SM_SL_SYNC_REQ = 1,
	GU_SM_SL_SYNC_REQ_TO,
	GU_SM_SL_SYNC_RES,
	GU_SM_SENSOR_REPORT_REQ,
	GU_SM_SL_SENSOR_REPORT_RES,
	GU_SM_SL_INIT_SETTING_REQ,
	GU_SM_SL_INIT_SETTING_RES,
	GU_SM_SL_SET_SETTINGS_REQ,
	GU_SM_SL_SET_SETTINGS_RES,
	GU_SM_SL_GET_SETTINGS_REQ,
	GU_SM_SL_GET_SETTINGS_RES,

	GU_SM_SL_POP_CTRL_MODE_SWITCH_REQ,
	GU_SM_SL_POP_CTRL_MODE_SWITCH_RES_OK,
	GU_SM_SL_POP_CTRL_MODE_SWITCH_RES_ERR,
	GU_SM_SL_POP_CTRL_CONTROL_REQ,
	GU_SM_SL_POP_CTRL_CONTROL_RES_OK,
	GU_SM_SL_POP_CTRL_CONTROL_RES_ERR,
	GU_SM_SL_IO_CONTROL_REQ,
	GU_SM_SL_IO_CONTROL_RES,

	GU_SM_SL_FW_UPDATE_REQ,
	GU_SM_SL_FW_NO_NEED_UPDATE_REQ,
	GU_SM_SL_FW_NEED_UPDATE_REQ,
	GU_SM_SL_FW_START_TRANF_REP,
	GU_SM_SL_FW_TRANF_DATA_REP,
	GU_SM_SL_FW_TRANF_DATA_COMPLETED_REP,
	GU_SM_SL_FW_UPDATE_COMPLETED_REP,
	GU_SM_SL_FW_UPDATE_FAILED_RES,

	GU_SM_SL_KEEP_ALIVE,

	GU_SM_SL_SENSOR_CALIB_TRANS_REQ,
	GU_SM_SL_SENSOR_CALIB_REPORT_REQ,
	GU_SM_SL_SENSOR_CALIB_REPORT_RES,
	GU_SM_SENSOR_DBG_REPORT_REQ,
	GU_SM_SL_SENSOR_DBG_REPORT_RES,
	GU_SM_DBG_FATAL_TEST_REQ,
	GU_SM_DBG_FATAL_LOG_READ_REQ,
	GU_SM_DBG_FATAL_LOG_READ_RES,
	GU_SM_DBG_FATAL_LOG_RESET_REQ,
	GU_SM_MODBUS_WRITE_SINGLE_REGISTER_REQ,
	GU_SM_DBG_MODBUS_WRITE_SINGLE_REGISTER_RES,
	GU_SM_DBG_MODBUS_SLAVE_LIST_INFO_RES,
	GU_SM_SL_LOG_DBG_SHOW_RES,
	GU_SM_DBG_MODBUS_COILS_STATUS_INFO_RES,
	GU_SM_SL_MODBUS_CONTROL_REQ,
	GU_SM_MODBUS_READ_COIL_RES
};

enum {
	GU_FIRMWARE_UPDATE_ERR = 1,
	GU_FIRMWARE_NO_NEED_TO_UPDATE,
	GU_FIRMWARE_STARTED_TRANSFER,
	GU_FIRMWARE_DEVICE_BUSY,
	GU_FIRMWARE_PACKED_TIMEOUT,
	GU_FIRMWARE_TRANSFER_STATUS,
	GU_FIRMWARE_CHECKSUM_ERR,
	GU_FIRMWARE_DEVICE_INTERNAL_UPDATE_STARTED,
	GU_FIRMWARE_UPDATE_COMPLETED,
	GU_FIRMWARE_UPDATE_FAILED
};

/*****************************************************************************/
/*  app function declare
 */
/*****************************************************************************/
#define SL_NUMBER_SAMPLE_CT_SENSOR					(3000)

extern int  main_app();

#ifdef __cplusplus
}
#endif

#endif //APP_H
