#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

#include "app.h"
#include "app_data.h"
#include "app_dbg.h"

#include "task_list.h"
#include "task_if.h"
#include "task_snmp.h"

using namespace std;

q_msg_t mt_task_snmp_mailbox;

static string snmp_folder_path;

static string sl_sensors_file_path;
static string connection_file_path;

static FILE* sl_sensors_file_obj;
static FILE* connection_file_obj;

static void update_sl_sensors_file(sl_sensors_t* sl_sensors);
static void update_connection_file(gw_connection_t* gw_connection);

void* mt_task_snmp_entry(void*) {
	wait_all_tasks_started();
	APP_DBG("[STARTED] mt_task_snmp_entry\n");

	/* create file paths */
	snmp_folder_path		= string(APP_ROOT_PATH_RAM) + string("/snmp");
	sl_sensors_file_path	= snmp_folder_path + string("/sl_sensors.txt");
	connection_file_path	= snmp_folder_path + string("/gw_connection.txt");

	/* create snmp folder path */
	struct stat st = {0};
	if (stat(snmp_folder_path.data(), &st) == -1) {
		mkdir(snmp_folder_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_SNMP_ID);

		/* handler message */
		switch (msg->header->sig) {
		case MT_SNMP_SL_SYNC_OK: {
			APP_DBG("MT_SNMP_SL_SYNC_OK\n");
			gw_connection_t gw_connection;
			app_data_get_gw_connection(&gw_connection);
			gw_connection.sl_conn = GW_CONNECTION_STATUS_CONNECTED;
			update_connection_file(&gw_connection);
		}
			break;

		case MT_SNMP_SL_SYNC_ERR: {
			APP_DBG("MT_SNMP_SL_SYNC_ERR\n");
			gw_connection_t gw_connection;
			app_data_get_gw_connection(&gw_connection);
			gw_connection.sl_conn = GW_CONNECTION_STATUS_DISCONNECTED;
			update_connection_file(&gw_connection);
		}
			break;

		case MT_SNMP_SL_SENSOR_REPORT_REP: {
			// APP_DBG("MT_SNMP_SL_SENSOR_REPORT_REP\n");
			sl_sensors_t st_sensors;

			memset(&st_sensors,0,sizeof(sl_sensors_t));

			get_data_common_msg(msg, (uint8_t*)&st_sensors, sizeof(sl_sensors_t));

			//				APP_PRINT("[-----------------------------]\n");
			//				APP_DBG("temperature      : [%2d : %2d :%2d :%2d]\n",
			//						st_sensors.temperature[0],
			//						st_sensors.temperature[1],
			//						st_sensors.temperature[2],
			//						st_sensors.temperature[3]);
			//				APP_DBG("himidity         : [%2d : %2d]\n",
			//						st_sensors.humidity[0],
			//						st_sensors.humidity[1]);
			//				APP_DBG("general_output   : [%2d : %2d :%2d :%2d]\n",
			//						st_sensors.general_output[0],
			//						st_sensors.general_output[1],
			//						st_sensors.general_output[2],
			//						st_sensors.general_output[3])
			//						;
			//				APP_DBG("general_input    : [%2d : %2d :%2d :%2d :%2d : %2d :%2d :%2d]\n",
			//						st_sensors.general_input[0],
			//						st_sensors.general_input[1],
			//						st_sensors.general_input[2],
			//						st_sensors.general_input[3],
			//						st_sensors.general_input[4],
			//						st_sensors.general_input[5],
			//						st_sensors.general_input[6],
			//						st_sensors.general_input[7]);
			//				APP_DBG("Fan speed        : [%2d : %2d :%2d :%2d]\n",
			//						st_sensors.fan_pop[0],
			//						st_sensors.fan_pop[1],
			//						st_sensors.fan_pop[2],
			//						st_sensors.fan_pop[3]);
			//				APP_DBG("power_output_status_1:  %d\n",	 st_sensors.power_output_status[0]);
			//				APP_DBG("power_output_current_1: %d\n",	st_sensors.power_output_current[0]);
			//				APP_DBG("power_output_status_2:	 %d\n",	 st_sensors.power_output_status[1]);
			//				APP_DBG("power_output_current_2: %d\n",	st_sensors.power_output_current[1]);
			//				APP_PRINT("[-----------------------------]\n");
			//				APP_DBG("						\n");

			update_sl_sensors_file(&st_sensors);

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

void update_sl_sensors_file(sl_sensors_t* sl_sensors) {
	sl_sensors_file_obj = fopen(sl_sensors_file_path.data(), "w+r");

	if (sl_sensors_file_obj == NULL) {
		APP_DBG("can not open %s file\n", sl_sensors_file_path.data());
	}
	else {

		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->temperature[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->temperature[1]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->temperature[2]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->temperature[3]);

		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->humidity[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->humidity[1]);

		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[1]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[2]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[3]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[4]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[5]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[6]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_input[7]);

		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_output[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_output[1]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_output[2]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->general_output[3]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->fan_pop[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->fan_pop[1]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->fan_pop[2]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->fan_pop[3]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->fan_dev);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->power_output_status[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->power_output_status[1]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->power_output_current[0]);
		fprintf(sl_sensors_file_obj, "%d\n",	sl_sensors->power_output_current[1]);

		fclose(sl_sensors_file_obj);
	}
}

void update_connection_file(gw_connection_t* gw_connection) {
	connection_file_obj = fopen(connection_file_path.data(), "w+r");

	if (connection_file_obj == NULL) {
		APP_DBG("can not open %s file\n", connection_file_path.data());
	}
	else {
		APP_DBG("opened %s file\n", connection_file_path.data());

		APP_DBG("sl_conn: %d\n"		, gw_connection->sl_conn);

		fprintf(connection_file_obj, "%d\n", gw_connection->sl_conn);

		fclose(connection_file_obj);
	}
}
