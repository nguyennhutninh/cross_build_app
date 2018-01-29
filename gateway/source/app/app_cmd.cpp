#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>

#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/cmd_line.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_config.h"
#include "app_cmd.h"

#include "task_list.h"
#include "task_list_if.h"

/*****************************************************************************/
/*  command function declare
 */
/*****************************************************************************/
static int32_t i_shell_ver(uint8_t* argv);
static int32_t i_shell_help(uint8_t* argv);
static int32_t i_shell_cfg(uint8_t* argv);
static int32_t i_shell_dbg(uint8_t* argv);
static int32_t i_shell_pop(uint8_t* argv);
static int32_t i_shell_out(uint8_t* argv);
static int32_t i_shell_fw(uint8_t* argv);
static int32_t i_shell_set(uint8_t* argv);
static int32_t i_shell_modbus_rs485(uint8_t* argv);
static int32_t i_shell_sen(uint8_t* argv);
static int32_t i_shell_fatal(uint8_t* argv);

/*****************************************************************************/
/*  local declare
 */
/*****************************************************************************/
#define STR_LIST_MAX_SIZE		10
#define STR_BUFFER_SIZE			128

static char cmd_buffer[STR_BUFFER_SIZE];
static char* str_list[STR_LIST_MAX_SIZE];
static uint8_t str_list_len;
extern uint32_t sensor_sl_sensor_req_failed_counter;
/*****************************************************************************/
/*  parser function declare
 */
/*****************************************************************************/
static uint8_t str_parser(char* str);
static char* str_parser_get_attr(uint8_t);

/*****************************************************************************/
/*  modbus define
 */
/*****************************************************************************/
#define MODBUS_SLAVE_RESET_DATA		(0x0000)
#define MODBUS_RESET_REGISTER		(0x4321)

/*****************************************************************************/
/*  command table declare
 */
/*****************************************************************************/
cmd_line_t lgn_cmd_table[] = {
	{(const int8_t*)"ver",		i_shell_ver,			(const int8_t*)"get kernel version"},
	{(const int8_t*)"sen",		i_shell_sen,			(const int8_t*)"get list sensor"},
	{(const int8_t*)"help",		i_shell_help,			(const int8_t*)"help command info"},
	{(const int8_t*)"cfg",		i_shell_cfg,			(const int8_t*)"config"},
	{(const int8_t*)"dbg",		i_shell_dbg,			(const int8_t*)"debug"},
	{(const int8_t*)"pop",		i_shell_pop,			(const int8_t*)"pop temperature control"},
	{(const int8_t*)"out",		i_shell_out,			(const int8_t*)"general output control"},
	{(const int8_t*)"fw",		i_shell_fw,				(const int8_t*)"up fw"},
	{(const int8_t*)"set",		i_shell_set,			(const int8_t*)"calib sensor"},
	{(const int8_t*)"mbs",		i_shell_modbus_rs485,	(const int8_t*)"devices power control"},
	{(const int8_t*)"fatal",	i_shell_fatal,			(const int8_t*)"fatal test"},
	/* End Of Table */
	{(const int8_t*)0,(pf_cmd_func)0,(const int8_t*)0}
};

int32_t i_shell_ver(uint8_t* argv) {
	(void)argv;
	printf("version: %s\n", AK_VERSION);
	return 0;
}

int32_t i_shell_sen(uint8_t* argv) {
	switch (*(argv + 4)) {
	case '?': {
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_DBG_REPORT_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_SENSOR_ID);
		task_post(MT_TASK_SENSOR_ID, s_msg);
	}
		break;

	default:
		break;
	}
	return 0;
}

int32_t i_shell_help(uint8_t* argv) {
	uint32_t idx = 0;
	switch (*(argv + 4)) {
	default:
		printf("\nCOMMANDS INFORMATION:\n\n");
		while(lgn_cmd_table[idx].cmd != (const int8_t*)0) {
			printf("%s\n\t%s\n\n", lgn_cmd_table[idx].cmd, lgn_cmd_table[idx].info);
			idx++;
		}
		break;
	}
	return 0;
}

int32_t i_shell_cfg(uint8_t* argv) {
	switch (*(argv + 4)) {
	case '0': {
		app_config_parameter_t config;

		/* lora gateway */
		strcpy(config.lora_gateway.lora_host,			"1.1.1.1");
		strcpy(config.lora_gateway.mqtt_host,			"118.69.135.199");
		config.lora_gateway.mqtt_port =					1883;
		strcpy(config.lora_gateway.mqtt_user_name,		"y55fYL");
		strcpy(config.lora_gateway.mqtt_psk,			"eJwKMNV2BQwC69PC");

		/* mqtt server */
		strcpy(config.pop_gateway.gateway_id_prefix,	"iot-");
		strcpy(config.pop_gateway.gateway_id,			"pop-dev");
		strcpy(config.pop_gateway.host,					"118.69.135.199");
		config.pop_gateway.port =						1883;
		strcpy(config.pop_gateway.user_name_view,		"fiot");
		strcpy(config.pop_gateway.user_psk_view	,		"ZmlvdEA5MTFmaW90");
		strcpy(config.pop_gateway.user_name_control,	"fciot");
		strcpy(config.pop_gateway.user_psk_control,		"ZmNpb3RAOTExOTExZmNpb3Q=");

		gateway_configure.write_config_data(&config);
		gateway_configure.parser_config_file(&config);

		APP_DBG("lora_gateway.lora_host:%s\n"			, config.lora_gateway.lora_host);
		APP_DBG("lora_gateway.mqtt_host:%s\n"			, config.lora_gateway.mqtt_host);
		APP_DBG("lora_gateway.mqtt_port:%d\n"			, config.lora_gateway.mqtt_port);
		APP_DBG("lora_gateway.mqtt_user_name:%s\n"		, config.lora_gateway.mqtt_user_name);
		APP_DBG("lora_gateway.mqtt_psk:%s\n"			, config.lora_gateway.mqtt_psk);

		APP_DBG("mqtt_server.gateway_id_prefix:%s\n"	, config.pop_gateway.gateway_id_prefix);
		APP_DBG("mqtt_server.gateway_id:%s\n"			, config.pop_gateway.gateway_id);
		APP_DBG("mqtt_server.host:%s\n"					, config.pop_gateway.host);
		APP_DBG("mqtt_server.port:%d\n"					, config.pop_gateway.port);
		APP_DBG("mqtt_server.user_name_view:%s\n"		, config.pop_gateway.user_name_view);
		APP_DBG("mqtt_server.user_psk_view:%s\n"		, config.pop_gateway.user_psk_view);
		APP_DBG("mqtt_server.user_name_control:%s\n"	, config.pop_gateway.user_name_control);
		APP_DBG("mqtt_server.user_psk_control:%s\n"		, config.pop_gateway.user_psk_control);
	}
		break;

	default:
		break;
	}

	return 0;
}

int32_t i_shell_dbg(uint8_t* argv) {
	switch (*(argv + 4)) {
	case '1': {
		printf("dbg request func 5!\n");

		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, 11);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	case '2': {
		printf("dbg request func 6!\n");
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, 13);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	case '3': {
		printf("dbg request func 1!\n");
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, 15);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	case 't': {
		printf("dbg stop all modbus test!\n");
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, 17);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	default:
		break;
	}
	return 0;
}

int32_t i_shell_pop(uint8_t* argv) {
	switch (*(argv + 4)) {
	case 'i': {
		printf("POP INFO request !\n");
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_CLOUD_POP_CTRL_GET_INFO_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_CLOUD_ID, s_msg);
	}
		break;

	case 'a': {
		printf("POP AUTO mode request !\n"); //
		sl_pop_fan_ctrl_t st_sl_pop_fan_ctrl;
		memset(&st_sl_pop_fan_ctrl, 0, sizeof(sl_pop_fan_ctrl_t));
		st_sl_pop_fan_ctrl.mode = SL_POP_CTRL_MODE_AUTO;
		ak_msg_t* s_msg = get_common_msg();

		set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
		set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
		set_if_des_task_id(s_msg, SL_TASK_POP_CTRL_ID);
		set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_if_sig(s_msg, SL_POP_CTRL_MODE_SWITCH_REQ);
		set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_pop_fan_ctrl, sizeof(sl_pop_fan_ctrl_t));

		set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_IF_ID, s_msg);
	}
		break;

	case 'm': {
		printf("POP MANUAL mode request !\n");
		sl_pop_fan_ctrl_t st_sl_pop_fan_ctrl;
		memset(&st_sl_pop_fan_ctrl, 0, sizeof(sl_pop_fan_ctrl_t));
		st_sl_pop_fan_ctrl.mode = SL_POP_CTRL_MODE_MANUAL;
		ak_msg_t* s_msg = get_common_msg();

		set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
		set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
		set_if_des_task_id(s_msg, SL_TASK_POP_CTRL_ID);
		set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_if_sig(s_msg, SL_POP_CTRL_MODE_SWITCH_REQ);
		set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_pop_fan_ctrl, sizeof(sl_pop_fan_ctrl_t));

		set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_IF_ID, s_msg);
	}
		break;

	case 'F': {
		printf("POP FANS run request !\n");
		sl_pop_fan_ctrl_t st_sl_pop_fan_ctrl;
		memset(&st_sl_pop_fan_ctrl, 0, sizeof(sl_pop_fan_ctrl_t));
		st_sl_pop_fan_ctrl.mode = SL_POP_CTRL_MODE_MANUAL;
		st_sl_pop_fan_ctrl.power_status = SL_POWER_STATUS_OFF;
		st_sl_pop_fan_ctrl.fan_status[0] = 100;
		st_sl_pop_fan_ctrl.fan_status[1] = 100;
		st_sl_pop_fan_ctrl.fan_status[2] = 100;
		st_sl_pop_fan_ctrl.fan_status[3] = 100;

		ak_msg_t* s_msg = get_common_msg();

		set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
		set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
		set_if_des_task_id(s_msg, SL_TASK_POP_CTRL_ID);
		set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_if_sig(s_msg, SL_POP_CTRL_CONTROL_REQ);
		set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_pop_fan_ctrl, sizeof(sl_pop_fan_ctrl_t));

		set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_IF_ID, s_msg);
	}
		break;

	case 'A': {
		printf("POP AIRCOND run request !\n");
		sl_pop_fan_ctrl_t st_sl_pop_fan_ctrl;
		memset(&st_sl_pop_fan_ctrl, 0, sizeof(sl_pop_fan_ctrl_t));
		st_sl_pop_fan_ctrl.mode = SL_POP_CTRL_MODE_MANUAL;
		st_sl_pop_fan_ctrl.power_status = SL_POWER_STATUS_ON;
		st_sl_pop_fan_ctrl.fan_status[0] = 0;
		st_sl_pop_fan_ctrl.fan_status[1] = 0;
		st_sl_pop_fan_ctrl.fan_status[2] = 0;
		st_sl_pop_fan_ctrl.fan_status[3] = 0;

		ak_msg_t* s_msg = get_common_msg();

		set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
		set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
		set_if_des_task_id(s_msg, SL_TASK_POP_CTRL_ID);
		set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_if_sig(s_msg, SL_POP_CTRL_CONTROL_REQ);
		set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_pop_fan_ctrl, sizeof(sl_pop_fan_ctrl_t));

		set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_IF_ID, s_msg);
	}
		break;

	default: {
		printf("\"pop a\" -> switch to AUTO mode\n");
		printf("\"pop m\" -> switch to MANUAL mode\n");
		printf("\"pop F\" -> on FANS and off AIRCOND\n");
		printf("\"pop A\" -> off FANS and on AIRCOND\n");
	}
		break;
	}

	return 0;
}

int32_t i_shell_fw(uint8_t* argv) {
	ak_msg_t* s_msg = get_pure_msg();
	set_msg_sig(s_msg, MT_FIRMWARE_SL_FW_UPDATE_REQ);
	set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
	task_post(MT_TASK_FIRMWARE_ID, s_msg);
	return 0;
}

/**/
/* CMD format
 * set i +06
 * i [1-6]:
 * i [1-4] -> temp_1 : temp_4
 * i [5-6] -> humi_1 : humi_2
*/
int32_t i_shell_set(uint8_t* argv) {
	uint8_t index = (*(argv + 5)) - 48;
	uint8_t opt   = (*(argv + 7));
	uint8_t value   = ((*(argv + 8) - 48)) * 10 + ((*(argv + 9) - 48));

	sl_sensor_calib_t sl_sensor_calib;
	memset(&sl_sensor_calib, 0 , sizeof(sl_sensor_calib_t));

	switch (*(argv + 4)) {
	case '?': {
		{
			ak_msg_t* s_msg = get_pure_msg();

			set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_CALIB_INFO_REQ);
			task_post(MT_TASK_SENSOR_ID, s_msg);
		}
		break;
	}

	case 't': {
		if(index > 0 && index < 4) {
			if(opt == ADD_OPERATOR || opt == SUB_OPERATOR) {
				if (value >= 0 && value < 99) {
					/* Do something here*/

					APP_DBG("[APP_CMD] calib temp\n");
					sl_sensor_calib.temp_calib_opt[index - 1] = opt;
					sl_sensor_calib.temp_calib_val[index - 1] = (uint8_t)value;

					{
						ak_msg_t* s_msg = get_common_msg();

						set_msg_sig(s_msg,				MT_SENSOR_SL_SENSOR_CALIB_TRANS_REQ);
						set_data_common_msg(s_msg, (uint8_t*)&sl_sensor_calib, sizeof(sl_sensor_calib_t));
						set_msg_src_task_id(s_msg,		MT_TASK_CONSOLE_ID);
						task_post(MT_TASK_SENSOR_ID,	s_msg);
					}

					break;
				}
			}
		}
	}

	case 'h': {
		if(index > 0 && index < 3) {
			if(opt == ADD_OPERATOR || opt == SUB_OPERATOR) {
				if (value >= 0 && value < 99) {
					/* Do something here*/
					APP_DBG("[APP_CMD] calib humi\n");
					sl_sensor_calib.humi_calib_opt[index - 1] = opt;
					sl_sensor_calib.humi_calib_val[index - 1] = value;

					ak_msg_t* s_msg = get_common_msg();

					set_msg_sig(s_msg, MT_SENSOR_SL_SENSOR_CALIB_TRANS_REQ);
					set_data_common_msg(s_msg, (uint8_t*)&sl_sensor_calib, sizeof(sl_sensor_calib_t));
					set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
					task_post(MT_TASK_SENSOR_ID, s_msg);

					break;
				}
			}
		}
	}

	default:{
		APP_DBG("\"set ti +/-j\" -> set temperature calib i:[1-4]  j[00-99]\n");
		APP_DBG("\"set hi +/-j\" -> set humidity calib    i:[1-2]  j[00-99]\n");
		APP_DBG(" Ex: \"set t1 +06\" -> set calib temp 1 = +06\n");
		APP_DBG(" Ex: \"set h1 +06\" -> set calib humidity 1 = +06\n");
	}
		break;
	}
	return 0;
}

int32_t i_shell_out(uint8_t* argv) {

	// declare temp variable
	uint8_t	num_of_string = str_parser((char*)argv);

	switch(num_of_string) {

	case 2: {
		char*	cmd1 = str_parser_get_attr(1);

		switch ((char)*cmd1) {
		case 'o': {
			sl_io_ctrl_t st_sl_io_ctrl;
			memset(&st_sl_io_ctrl, 1, sizeof(sl_io_ctrl_t));

			st_sl_io_ctrl.general_output[0]=1;
			st_sl_io_ctrl.general_output[1]=1;
			st_sl_io_ctrl.general_output[2]=1;
			st_sl_io_ctrl.general_output[3]=1;

			ak_msg_t* s_msg = get_common_msg();

			set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
			set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
			set_if_des_task_id(s_msg, SL_TASK_IO_CTRL_ID);
			set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
			set_if_sig(s_msg, SL_IO_CTRL_CONTROL_REQ);
			set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_io_ctrl, sizeof(sl_io_ctrl_t));

			set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
			task_post(MT_TASK_IF_ID, s_msg);
		}
			break;

		default:
			break;
		}
	}
		break;

	case 3: {
		char*	cmd1 = str_parser_get_attr(1);
		char*	stt = str_parser_get_attr(2);
		uint8_t pin_number = atoi(cmd1);

		if( pin_number < 1 || pin_number > 4 || ((char)*stt != '0' && (char)*stt != '1')) {
			goto cmd_error;
			return 0;
		}

		uint8_t value = 0;
		sl_io_ctrl_t st_sl_io_ctrl;
		memset(st_sl_io_ctrl.general_output,0,4);
		st_sl_io_ctrl.general_output[0] = 255;
		st_sl_io_ctrl.general_output[1]	= 255;
		st_sl_io_ctrl.general_output[2]	= 255;
		st_sl_io_ctrl.general_output[3]	= 255;

		if ((char)*stt == '1')
			value = 1;
		else
			value = 0;

		switch(pin_number) {
		case 1:
			st_sl_io_ctrl.general_output[pin_number - 1] = value;
			break;
		case 2:
			st_sl_io_ctrl.general_output[pin_number - 1] = value;
			break;
		case 3:
			st_sl_io_ctrl.general_output[pin_number - 1] = value;
			break;
		case 4:
			st_sl_io_ctrl.general_output[pin_number - 1] = value;
			break;
		default:
			break;
		}

		ak_msg_t* s_msg = get_common_msg();

		set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
		set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
		set_if_des_task_id(s_msg, SL_TASK_IO_CTRL_ID);
		set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_if_sig(s_msg, SL_IO_CTRL_CONTROL_REQ);
		set_if_data_common_msg(s_msg, (uint8_t*)&st_sl_io_ctrl, sizeof(sl_io_ctrl_t));

		set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_IF_ID, s_msg);

	}
		break;

cmd_error:
	default:
		APP_DBG("\"out o\" <=> write all pin out\n");
		APP_DBG("\"out i 0/1\" i[1 -> 4] <=> write pin i with high/low \n");
		break;
	}
	return 0;
}

int32_t	i_shell_modbus_rs485(uint8_t* argv) {

	uint8_t	num_str = str_parser((char*)argv);
	char*	cmd;
	int8_t	addr = 0;
	uint16_t	value = 0;
	uint8_t slaveid = 0;
	switch (num_str) {

	case 5: {
		//APP_DBG("MODBUS_DBG_WRITE_SINGLE_REGISTER_REQ\n");

		cmd		= str_parser_get_attr(2);
		addr	= atoi(str_parser_get_attr(3)); // check addrs
		slaveid = atoi(str_parser_get_attr(1));

		if ((addr < 0) || (addr > 12)) {
			goto exception;
		}

		if (strcmp((const char *)cmd, (const char *)"w") == 0) {
			value	= atoi(str_parser_get_attr(4));

			if ((value < 0) || (value > 1) ) {
				goto exception;
			}

			uint16_t reg_data;

			if(value == 0)
				reg_data = 0xFF00;
			else if(value == 1)
				reg_data = 0x0000;
			else
				goto exception;

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));
			generator.slaveID		  = slaveid;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_WRITE_COIL;
			generator.reg[0]          = addr;
			generator.reg_value[0]    = reg_data;
			APP_DBG("generator.slaveID      :  %d\n", generator.slaveID	    );
			APP_DBG("generator.total_reg    :  %d\n", generator.total_reg   );
			APP_DBG("generator.reg_func[0]  :  %d\n", generator.reg_func[0] );
			APP_DBG("generator.reg[0]       :  %d\n", generator.reg[0]      );
			APP_DBG("generator.reg_value[0] :  %d\n", generator.reg_value[0]);

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				set_if_sig(s_msg, SL_DBG_MODBUS_WRITE_SINGLE_REGISTER_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
		else if (strcmp((const char *)cmd, (const char *)"t") == 0) {
			//APP_DBG("modbus reset single coil\n");
			value	= atoi(str_parser_get_attr(4));
			uint16_t reg_data;

			if(value < 50 || value > 100000) {
				goto exception;
			}
			else {
				reg_data = value;
			}

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));

			generator.slaveID		  = slaveid;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_WRITE_REGISTER;
			generator.reg[0]          = addr;
			generator.reg_value[0]    = reg_data;

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				set_if_sig(s_msg, SL_SENSOR_MODBUS_REGISTER_INIT_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
		else
			goto exception;
	}
		break;

	case 3: {
		//APP_DBG("modbus reboot slave\n");

		cmd  = str_parser_get_attr(2);
		slaveid = atoi(str_parser_get_attr(1));

		APP_DBG("addr: %d\n",addr);

		if (strcmp((const char *)cmd, (const char *)"s") == 0) {

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));
			generator.slaveID		  = slaveid;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_WRITE_REGISTER;
			generator.reg[0]          = MODBUS_RESET_REGISTER;
			generator.reg_value[0]    = MODBUS_SLAVE_RESET_DATA;

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				set_if_sig(s_msg, SL_DBG_MODBUS_RESET_SLAVE_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
		else if (strcmp((const char *)cmd, (const char *)"r") == 0) {

			sl_generator_register_t generator;

			memset(&generator, 0, sizeof(sl_generator_register_t));
			generator.slaveID		  = slaveid;
			generator.total_reg       = 0x01;
			generator.reg_func[0]     = MB_FC_READ_COILS;
			generator.reg[0]          = 0x0000;
			generator.reg_value[0]    = 0x000C;
			APP_DBG("generator.slaveID      :  %d\n", generator.slaveID	    );
			APP_DBG("generator.total_reg    :  %d\n", generator.total_reg   );
			APP_DBG("generator.reg_func[0]  :  %d\n", generator.reg_func[0] );
			APP_DBG("generator.reg[0]       :  %d\n", generator.reg[0]      );
			APP_DBG("generator.reg_value[0] :  %d\n", generator.reg_value[0]);

			{
				ak_msg_t* s_msg = get_common_msg();

				set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
				set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
				set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
				set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				set_if_sig(s_msg, SL_MODBUS_READ_COIL_REQ);
				set_if_data_common_msg(s_msg, (uint8_t*)&generator, sizeof(sl_generator_register_t));

				set_msg_sig(s_msg, MT_IF_COMMON_MSG_OUT);
				set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
				task_post(MT_TASK_IF_ID, s_msg);
			}
		}
		else
			goto exception;
	}
		break;

	case 2: {
		cmd = str_parser_get_attr(1);
		if (strcmp((const char *)cmd, (const char *)"i") == 0) {

			ak_msg_t* s_msg = get_pure_msg();
			set_if_des_type(s_msg, IF_TYPE_CPU_SERIAL_SL);
			set_if_src_type(s_msg, IF_TYPE_CPU_SERIAL_MT);
			set_if_des_task_id(s_msg, SL_TASK_RS485_MODBUS_ID);
			set_if_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
			set_if_sig(s_msg, SL_DBG_MODBUS_SLAVE_LIST_INFO_REQ);

			set_msg_sig(s_msg, MT_IF_PURE_MSG_OUT);
			set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
			task_post(MT_TASK_IF_ID, s_msg);

		}
		else
			goto exception;
	}
		break;

exception:
	default:
		APP_DBG("\n");
		APP_DBG("mbs i to get slave list info\n");
		APP_DBG("mbs r to read all status coil\n");
		APP_DBG("mbs <slave addr>  w  < reg >  < value >  1:on 0:off\n");
		APP_DBG("mbs <slave addr>  t  < reg >  < (50ms < (time_toggle)  < 500000ms) > \n");
		APP_DBG("mbs <slave addr>  s  restart modbus slave\n");
		APP_DBG("\n");
		break;
	}
	return 0;
}

int32_t i_shell_fatal(uint8_t* argv) {

	switch (*(argv + 6)) {
	case 't': {
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_DBG_SL_FATAL_TEST_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	case 'l': {
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_DBG_SL_FATAL_LOG_READ_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;

	case 'r': {
		ak_msg_t* s_msg = get_pure_msg();
		set_msg_sig(s_msg, MT_DBG_SL_FATAL_LOG_RESET_REQ);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		task_post(MT_TASK_DEBUG_MSG_ID, s_msg);
	}
		break;
	case '8': {

		power_reset_registers_t coils;
		memset(&coils, 0, sizeof(power_reset_registers_t));
		coils.coil[0].state = 1;
		coils.coil[1].state = 1;
		coils.coil[2].state = 1;

		ak_msg_t* s_msg = get_common_msg ();

		set_msg_sig(s_msg, MT_SM_DBG_MODBUS_COILS_STATUS_INFO_RES);
		set_msg_src_task_id(s_msg, MT_TASK_CONSOLE_ID);
		set_data_common_msg (s_msg, (uint8_t *)&coils, sizeof(power_reset_registers_t));
		task_post(MT_TASK_SM_ID, s_msg);
	}
		break;

	default: {
		APP_DBG("\"fatal t\" <=> fatal outdoor slave\n");
		APP_DBG("\"fatal l\" <=> read outdoor slave fatal log \n");
		APP_DBG("\"fatal r\" <=> reset outdoor slave fatal log \n");
	}
		break;
	}

	return 0;
}

/*****************************************************************************/
/*  parser function definaion
 */
/*****************************************************************************/
/**
  * @method		str_parser(uint8_t* argv)
  * @param		char*	str		Input String
  * @return		uint8_t			Number of substring
  */
uint8_t str_parser(char* str) {
	strcpy(cmd_buffer, str);
	str_list_len = 0;

	uint8_t i = 0;
	uint8_t str_list_index = 0;
	uint8_t flag_insert_str = 1;

	while (cmd_buffer[i] != 0 && cmd_buffer[i] != '\n' && cmd_buffer[i] != '\r') {
		if (cmd_buffer[i] == ' ') {
			cmd_buffer[i] = 0;
			flag_insert_str = 1;
		}
		else if (flag_insert_str) {
			str_list[str_list_index++] = &cmd_buffer[i];
			flag_insert_str = 0;
		}
		i++;
	}

	cmd_buffer[i] = 0;

	str_list_len = str_list_index;
	return str_list_len;
}

/**
  * @method		str_parser_get_attr(uint8_t index)
  * @param		uint8_t index	index of substring
  * @return		char*			pointer to substring
  */
char* str_parser_get_attr(uint8_t index) {
	if (index < str_list_len) {
		return str_list[index];
	}
	return NULL;
}
