#include "../ak/timer.h"

#include "task_list.h"

ak_task_t task_list[] = {
	{	AK_TASK_TIMER_ID,				TASK_PRI_LEVEL_1,	timer_entry						,	&timer_mailbox						,	"timer service"			},
	{	MT_TASK_IF_CONSOLE_ID,			TASK_PRI_LEVEL_1,	mt_task_if_console_entry		,	&mt_task_if_console_mailbox			,	"terminal gate"			},
	{	MT_TASK_CONSOLE_ID,				TASK_PRI_LEVEL_1,	mt_task_console_entry			,	&mt_task_console_mailbox			,	"handle commands"		},
	{	MT_TASK_SNMP_ID,				TASK_PRI_LEVEL_1,	mt_task_snmp_entry				,	&mt_task_snmp_mailbox				,	"task snmp"				},
	{	MT_TASK_CLOUD_ID,				TASK_PRI_LEVEL_1,	mt_task_cloud_entry				,	&mt_task_cloud_mailbox				,	"task mqtt"				},
	{	MT_TASK_IF_ID,					TASK_PRI_LEVEL_1,	mt_task_if_entry				,	&mt_task_if_mailbox					,	"task if"				},
	{	MT_TASK_DEBUG_MSG_ID,			TASK_PRI_LEVEL_1,	mt_task_debug_msg_entry			,	&mt_task_debug_msg_mailbox			,	"task debug message"	},
	{	MT_TASK_IF_APP_ID,				TASK_PRI_LEVEL_1,	mt_task_if_app_entry			,	&mt_task_if_app_mailbox				,	"if app socket"			},
	{	MT_TASK_IF_CPU_SERIAL_ID,		TASK_PRI_LEVEL_1,	mt_task_if_cpu_serial_entry		,	&mt_task_if_cpu_serial_mailbox		,	"cpu serial interface"	},
	{	MT_TASK_SM_ID,					TASK_PRI_LEVEL_1,	mt_task_sm_entry				,	&mt_task_sm_mailbox					,	"master state machine"	},
	{	MT_TASK_SENSOR_ID,				TASK_PRI_LEVEL_2,	mt_task_sensor_entry			,	&mt_task_sensor_mailbox				,	"sensor task"			},
	{	MT_TASK_SYS_ID,					TASK_PRI_LEVEL_1,	mt_task_sys_entry				,	&mt_task_sys_mailbox				,	"app system task"		},
	{	MT_TASK_FIRMWARE_ID,			TASK_PRI_LEVEL_1,	mt_task_firmware_entry			,	&mt_task_firmware_mailbox			,	"firmware task"			},
	{	MT_TASK_MODBUS_ID,				TASK_PRI_LEVEL_1,	mt_task_modbus_entry			,	&mt_task_modbus_mailbox				,	"modbus task"			},
};
