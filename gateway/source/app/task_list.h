#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "../ak/ak.h"
#include "../ak/message.h"

#include "app_data.h"

#include "if_console.h"
#include "if_rf24.h"
#include "if_rf433.h"
#include "if_app.h"
#include "if_cpu_serial.h"
#include "if_lora_usb_stick.h"

#include "task_console.h"
#include "task_snmp.h"
#include "task_cloud.h"
#include "task_if.h"
#include "task_debug_msg.h"
#include "task_sm.h"
#include "task_sensor.h"
#include "task_sys.h"
#include "task_firmware.h"
#include "task_modbus.h"

/** default if_des_type when get pool memory
 * this define MUST BE coresponding with app.
 */
#define AK_APP_TYPE_IF						IF_TYPE_APP_MT

enum {
		/* SYSTEM TASKS */
		AK_TASK_TIMER_ID,

		/* APP TASKS */
		MT_TASK_IF_CONSOLE_ID,
		MT_TASK_CONSOLE_ID,
		MT_TASK_SNMP_ID,
		MT_TASK_CLOUD_ID,
		MT_TASK_IF_ID,
		MT_TASK_DEBUG_MSG_ID,
		MT_TASK_IF_APP_ID,
		MT_TASK_IF_CPU_SERIAL_ID,
		MT_TASK_SM_ID,
		MT_TASK_SENSOR_ID,
		MT_TASK_SYS_ID,
		MT_TASK_FIRMWARE_ID,
		MT_TASK_MODBUS_ID,
		/* EOT task ID */
		AK_TASK_LIST_LEN
};
extern ak_task_t task_list[];

#endif //__TASK_LIST_H__
