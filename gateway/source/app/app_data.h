#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include "port.h"

#include <stdint.h>
#include <string>

//#include "../common/json.hpp"

//using namespace std;
//using json = nlohmann::json;

#define AK_ENABLE						(0x01)
#define AK_TASK_DEBUG					AK_ENABLE

/******************************************************************************
* interface type
*******************************************************************************/
/** RF24 interface for modules
*/
#define IF_TYPE_RF24_MT						(0)

/******************************************************************************
* Data type of RF24Network
*******************************************************************************/
#define RF24_DATA_COMMON_MSG_TYPE			(1)
#define RF24_DATA_PURE_MSG_TYPE				(2)
#define RF24_DATA_REMOTE_CMD_TYPE			(3)

/** APP interface, communication via socket interface
 *
 */
#define IF_TYPE_APP_START					(100)
#define IF_TYPE_APP_GMNG					(100)
#define IF_TYPE_APP_MT						(101)
#define IF_TYPE_APP_GU						(102)

/** CPU SERIAL interface, communication via uart serial interface
 *
 */
#define IF_TYPE_CPU_SERIAL_MT				(120)
#define IF_TYPE_CPU_SERIAL_SL				(121)

#define ENABLE_LAUNCHER						(0x01)
#define DISABLE_LAUNCHER					(0x00)

typedef struct {
	uint32_t id;
	char* socket_path;
	char* location_path;
	uint8_t enable_launcher;
	__pid_t pid;
} if_app_t;

extern if_app_t if_app_list[];
extern uint32_t if_app_list_size;

/******************************************************************************
* Common define
*******************************************************************************/
#define APP_ERROR_CODE_TIMEOUT		0x01
#define APP_ERROR_CODE_BUSY			0x02
#define APP_ERROR_CODE_STATE		0x03

/******************************************************************************
* Commom data structure for transceiver data
*******************************************************************************/
#define CONFIGURE_PARAMETER_BUFFER_SIZE		256
#define RF24_ENCRYPT_DECRYPT_KEY_SIZE		16
extern uint8_t rf24_encrypt_decrypt_key[];

#define FIRMWARE_PSK				0x1A2B3C4D
#define FIRMWARE_LOK				0x1234ABCD

typedef struct {
	uint32_t psk;
	uint32_t bin_len;
	uint16_t checksum;
} firmware_header_t;

#define SENSOR_STATUS_UNKOWN				0x00
#define SENSOR_STATUS_NORMAL				0x01
#define SENSOR_STATUS_WARNING				0x02
#define SENSOR_STATUS_DISCONNECTED			0x03

#define SENSOR_STATUS_CLOSED				0x01
#define SENSOR_STATUS_OPENED				0x02

#define SENSOR_STATUS_HIGH					0x01
#define SENSOR_STATUS_LOW					0x02

#define SL_TOTAL_SENSOR_TEMPERATURE			(4)
#define SL_TOTAL_SENSOR_HUMIDITY			(2)
#define SL_TOTAL_GENERAL_INPUT				(8)
#define SL_TOTAL_GENERAL_OUTPUT				(4)
#define SL_TOTAL_FAN_POP					(4)
#define SL_TOTAL_POWER_OUTPUT_STATUS		(2)
#define SL_TOTAL_POWER_OUTPUT_CURRENT		(2)

typedef struct {
	uint16_t temperature[SL_TOTAL_SENSOR_TEMPERATURE];				/* *C */
	uint8_t humidity[SL_TOTAL_SENSOR_HUMIDITY];						/* RH% */
	uint8_t general_input[SL_TOTAL_GENERAL_INPUT];					/* HIGH/LOW */
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];				/* HIGH/LOW */
	uint8_t fan_pop[SL_TOTAL_FAN_POP];								/* (0 -> 100)% */
	uint8_t fan_dev;												/* (0 -> 100)% */
	uint8_t power_output_status[SL_TOTAL_POWER_OUTPUT_STATUS];		/* ON/OFF */
	uint32_t power_output_current[SL_TOTAL_POWER_OUTPUT_CURRENT];	/* mA */
} __AK_PACKETED sl_sensors_t;

#define GW_CONNECTION_STATUS_UNKNOWN		0x00
#define GW_CONNECTION_STATUS_CONNECTED		0x01
#define GW_CONNECTION_STATUS_DISCONNECTED	0x02

typedef struct {
	uint8_t sl_conn;
} gw_connection_t;

#define SL_POWER_STATUS_ON					0x01
#define SL_POWER_STATUS_OFF					0x02

#define SL_POP_CTRL_MODE_AUTO				0x01
#define SL_POP_CTRL_MODE_MANUAL				0x02

typedef struct {
	uint8_t mode;
	uint8_t power_status;
	uint8_t fan_status[SL_TOTAL_FAN_POP];
} sl_pop_fan_ctrl_t;

typedef struct {
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];
} sl_io_ctrl_t;

typedef struct {
	uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];
} mqtt_sl_io_control_packet_t;

typedef struct {
	char lora_host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char mqtt_host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	uint32_t mqtt_port;
	char mqtt_user_name[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char mqtt_psk[CONFIGURE_PARAMETER_BUFFER_SIZE];
} lora_gateway_t;

typedef struct {
	char host[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char gateway_id_prefix[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char gateway_id[CONFIGURE_PARAMETER_BUFFER_SIZE];
	uint32_t port;

	char user_name_view[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char user_psk_view[CONFIGURE_PARAMETER_BUFFER_SIZE];

	char user_name_control[CONFIGURE_PARAMETER_BUFFER_SIZE];
	char user_psk_control[CONFIGURE_PARAMETER_BUFFER_SIZE];
} pop_gateway_t;

typedef struct {
	lora_gateway_t	lora_gateway;
	pop_gateway_t	pop_gateway;
} app_config_parameter_t;

/* device connection status */
#define DEVICE_IS_DISCONNECTED				(0x00)
#define DEVICE_IS_CONNECTED					(0x01)

#define LORA_NODE_REGISTER					(0)
#define LORA_NODE_REPORT					(1)
#define LORA_NODE_KEEP_ALIVE				(2)

#define FLOOD_SENSOR_ACTIVE					(0x01)
#define FLOOD_SENSOR_INACTIVE				(0x02)

typedef struct {
	uint32_t scr_addr;
	uint32_t des_addr;
	uint8_t  type;
} __attribute__((__packed__))lora_header_t;

typedef struct {
	lora_header_t header;
	uint32_t data;
} __attribute__((__packed__))lora_message_t;

/**
 * gw_connection will be updated in handshake state.
 */
extern void app_data_set_gw_connection(gw_connection_t*);
extern void app_data_get_gw_connection(gw_connection_t*);

/**
 * configure parameter will be updated when app start.
 */
extern void app_data_set_config_parameter(app_config_parameter_t*);
extern void app_data_get_config_parameter(app_config_parameter_t*);

/**
 * Sensor calib define
 */
#define ADD_OPERATOR						('+')
#define SUB_OPERATOR						('-')

typedef struct {
	uint8_t temp_calib_val[SL_TOTAL_SENSOR_TEMPERATURE];	/* *C */
	uint8_t temp_calib_opt[SL_TOTAL_SENSOR_TEMPERATURE];	/*  + / - */
	uint8_t humi_calib_val[SL_TOTAL_SENSOR_HUMIDITY];		/* RH% */
	uint8_t humi_calib_opt[SL_TOTAL_SENSOR_HUMIDITY];		/*  + / - */
}	sl_sensor_calib_t;

/* Function 05 Modbus define */
#define MODBUS_F05_ON					(0xFF00)
#define MODBUS_F05_OFF					(0x0000)
#define MODBUS_F05_DEV_ADDR_BASE			((unsigned int) 167)

/*******************************************/

/* data struct for force single coil */
typedef struct {
	unsigned int reg;
	unsigned int num_of_reg;
	unsigned int data;
	unsigned int* reg_array;
} modbus_common_data_t;

/*data for fatal function*/

typedef struct {
	uint32_t except_number;
	uint32_t timestamp;
} exception_info_t;

typedef struct {
	uint32_t	start_post;
	uint32_t	start_exe;
	uint32_t	stop_exe;
} __AK_PACKETED sl_dbg_handler_t;

typedef struct ak_msg_sl_t {
	/*******************************
	 * private for kernel.
	 ******************************/
	/* message management */
	struct ak_msg_sl_t*	next;

	/*******************************
	 * kernel debug.
	 ******************************/
	/* task debug */
#if (AK_TASK_DEBUG == AK_ENABLE)
	sl_dbg_handler_t		dbg_handler;
#endif

	uint8_t				src_task_id;
	uint8_t				des_task_id;
	uint8_t				ref_count;
	uint8_t				sig;

	uint8_t				if_src_task_id;
	uint8_t				if_des_task_id;
	uint8_t				if_src_type;
	uint8_t				if_des_type;
	uint8_t				if_sig;
} __AK_PACKETED ak_msg_sl_t;

typedef uint8_t	task_pri_t;
typedef uint8_t	task_id_t;
typedef void	( *pf_sl_task)( ak_msg_sl_t* );

typedef struct {
	task_id_t	id;
	task_pri_t	pri;
	pf_sl_task	task;
} __AK_PACKETED task_t;

typedef struct {
	uint32_t ipsr;
	uint32_t primask;
	uint32_t faultmask;
	uint32_t basepri;
	uint32_t control;
} __AK_PACKETED m3_core_reg_t;

typedef struct {
	int8_t			string[10];
	uint8_t			code;
	task_t			current_task;
	ak_msg_sl_t		current_active_object;
	m3_core_reg_t	m3_core_reg;
	uint32_t		fatal_times;
	uint32_t		restart_times;
} __AK_PACKETED fatal_log_t;

#define SYS_IRQ_EXCEPTION_NUMBER_IRQ0_NUMBER_RESPECTIVE		16	/* exception number 16 ~~ IRQ0 */
/*******************************************/

/*data for modbus rs485 task*/

#define SL_MAX_MOSBUS_REGISTER	    20
#define SL_MAX_MODBUS_COIL			12

typedef struct {
	uint8_t slaveID;
	uint8_t total_reg;
	uint8_t reg_func[SL_MAX_MOSBUS_REGISTER];
	uint16_t reg[SL_MAX_MOSBUS_REGISTER];
	uint16_t reg_value[SL_MAX_MOSBUS_REGISTER];
} sl_generator_register_t;

enum MB_FC {
	MB_FC_NONE                     = 0,   /*!< null operator */
	MB_FC_READ_COILS               = 1,	/*!< FCT=1 -> read coils or digital outputs */
	MB_FC_READ_DISCRETE_INPUT      = 2,	/*!< FCT=2 -> read digital inputs */
	MB_FC_READ_REGISTERS           = 3,	/*!< FCT=3 -> read registers or analog outputs */
	MB_FC_READ_INPUT_REGISTER      = 4,	/*!< FCT=4 -> read analog inputs */
	MB_FC_WRITE_COIL               = 5,	/*!< FCT=5 -> write single coil or output */
	MB_FC_WRITE_REGISTER           = 6,	/*!< FCT=6 -> write single register */
	MB_FC_WRITE_MULTIPLE_COILS     = 15,	/*!< FCT=15 -> write multiple coils or outputs */
	MB_FC_WRITE_MULTIPLE_REGISTERS = 16	/*!< FCT=16 -> write multiple registers */
};

typedef struct {
	uint8_t state;
} __AK_PACKETED register_list_t;

typedef struct {
	register_list_t coil[SL_MAX_MODBUS_COIL];
} __AK_PACKETED power_reset_registers_t;

enum {
	NO_REPLY = 18,
	EXC_FUNC_CODE = 1,
	EXC_ADDR_RANGE = 2,
	EXC_REGS_QUANT = 3,
	EXC_EXECUTE = 4,
	EXC_BUFFER = 5,
	EXC_ID = 6,
	EXC_BAD_CRC = 7,

	ERR_EXCEPTION = 8,
	ERR_NOT_MASTER    = 9,
	ERR_POLLING       = 10,
	ERR_BUFF_OVERFLOW = 11,
	ERR_BAD_CRC       = 12,
	ERR_SLAVE_ID		 = 13,
	ERR_BUFF			 = 14,

	COM_IDLE                     = 15,
	COM_WAITING                  = 16,
	COM_SUCCESSFULL				 = 17
};

typedef struct {
	uint8_t sensor_report_en;
}ui_setting_t;

typedef struct {
	char log[40];
}ui_log_t;

/*******************************************/
#endif //__APP_DATA_H__
