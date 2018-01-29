#include <sys/socket.h>
#include <sys/stat.h>

#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <iostream>

#include "../ak/ak.h"

#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_flood_system.h"

#include "task_list.h"
#include "task_list_if.h"

#define FLAG_SET_X_LOCATION		2
#define FLAG_SET_Y_LOCATION		1
#define ACCEPT					1
#define DENY					0

/* vector contain flood sensor devices */
static string flood_system_folder_path;
static string flood_system_devs_file_path;
static int flood_system_file_obj;

/* list flood sensor devices */
static vector<string> list_flood_sensor;
static bool is_dev_exist_in_dev_list(uint32_t);
static void dev_list_get(vector<string>&);
static void dev_list_set(vector<string>&);
static void dev_list_show(vector<string>&);

/* flood sensor log */
static string flood_system_sensor_log_file_path;
static FILE* flood_system_sensor_file_obj;
static void flood_system_log_write(lora_message_t*);

q_msg_t mt_task_flood_system_mailbox;

void *mt_task_flood_system_entry(void *) {
	flood_system_folder_path			= string(APP_ROOT_PATH_DISK) + string("/flood_system");
	flood_system_devs_file_path			= flood_system_folder_path + string("/flood_sensor_device_list.json");
	flood_system_sensor_log_file_path	= flood_system_folder_path + string("/flood_sensor.log");

	/* create flood_system folder path */
	struct stat st = {0};
	if (stat(flood_system_folder_path.data(), &st) == -1) {
		mkdir(flood_system_folder_path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	task_mask_started();
	wait_all_tasks_started();
	APP_DBG("[STARTED] mt_task_flood_system_entry\n");
	APP_DBG("lora host: %d\n", (uint32_t)inet_addr(gateway_configure_parameter.lora_gateway.lora_host));

	/* update sensor list */
	dev_list_get(list_flood_sensor);
	dev_list_show(list_flood_sensor);

	while(1) {
		while (msg_available(MT_TASK_FLOOD_SYSTEM_ID)) {
			ak_msg_t* msg = rev_msg(MT_TASK_FLOOD_SYSTEM_ID);

			switch (msg->header->sig) {
			case MT_FLOOD_SYSTEM_INCOMMING: {
				APP_DBG("MT_FLOOD_SYSTEM_INCOMMING\n");
				lora_message_t rcv_lora_msg;
				get_data_common_msg(msg,(uint8_t*)&rcv_lora_msg,sizeof(lora_message_t));

				APP_DBG("[header.scr_addr] 0x%X\n", rcv_lora_msg.header.scr_addr);
				APP_DBG("[header.des_addr] 0x%X\n", rcv_lora_msg.header.des_addr);
				APP_DBG("[header.type] %d\n", rcv_lora_msg.header.type);
				APP_DBG("[data] %d\n", rcv_lora_msg.data);

				flood_system_log_write(&rcv_lora_msg);

				if (rcv_lora_msg.header.des_addr == (uint32_t)inet_addr(gateway_configure_parameter.lora_gateway.lora_host)) {
					switch (rcv_lora_msg.header.type) {
					case LORA_NODE_REGISTER: {
						APP_DBG("LORA_NODE_REGISTER\n");
						lora_message_t join_response;
						join_response.header.scr_addr	=	(uint32_t)inet_addr(gateway_configure_parameter.lora_gateway.lora_host);
						join_response.header.des_addr	=	rcv_lora_msg.header.scr_addr;
						join_response.header.type		=	LORA_NODE_REGISTER;
						join_response.data				=	ACCEPT;

						/* response to sensor */
						ak_msg_t* s_msg = get_common_msg();
						set_msg_sig(s_msg, MT_IF_LORA_USB_STICK_FLOOD_SYSTEM_MSG_OUT);
						set_data_common_msg(s_msg, (uint8_t*)&join_response, sizeof(lora_message_t));
						set_msg_src_task_id(s_msg, MT_TASK_FLOOD_SYSTEM_ID);
						task_post(MT_TASK_IF_LORA_USB_STICK_ID, s_msg);

						/* add to sensor list */
						if (is_dev_exist_in_dev_list(rcv_lora_msg.header.scr_addr)) {
							APP_DBG("sensor had been registered\n");
						}
						else {
							struct in_addr lora_in_addr;
							lora_in_addr.s_addr = rcv_lora_msg.header.scr_addr;
							string join_dev_ip((const char*)inet_ntoa(lora_in_addr));

							list_flood_sensor.push_back(join_dev_ip);

							dev_list_set(list_flood_sensor);
						}
					}
						break;

					case LORA_NODE_REPORT: {
						APP_DBG("LORA_NODE_REPORT\n");
						if (is_dev_exist_in_dev_list(rcv_lora_msg.header.scr_addr)) {
							ak_msg_t* s_msg = get_dymanic_msg();

							set_msg_sig(s_msg, MT_CLOUD_MQTT_FLOOD_SENSOR_REP);
							set_data_dynamic_msg(s_msg, (uint8_t*)&rcv_lora_msg, sizeof(lora_message_t));

							set_msg_src_task_id(s_msg, MT_TASK_FLOOD_SYSTEM_ID);
							task_post(MT_TASK_CLOUD_ID, s_msg);
						}
						else {
							/* TODO: strange device */
						}
					}
						break;

					case LORA_NODE_KEEP_ALIVE: {
						APP_DBG("LORA_NODE_KEEP_ALIVE\n");
						if (is_dev_exist_in_dev_list(rcv_lora_msg.header.scr_addr)) {
							ak_msg_t* s_msg = get_dymanic_msg();

							set_msg_sig(s_msg, MT_CLOUD_MQTT_FLOOD_BAT_REP);
							set_data_dynamic_msg(s_msg, (uint8_t*)&rcv_lora_msg, sizeof(lora_message_t));

							set_msg_src_task_id(s_msg, MT_TASK_FLOOD_SYSTEM_ID);
							task_post(MT_TASK_CLOUD_ID, s_msg);
						}
						else {
							/* TODO: strange device */
						}
					}
						break;

					default:
						break;
					}
				}
			}
				break;

			default:
				break;
			}

			/* free message */
			free_msg(msg);
		}

		usleep(1000);
	}

	return (void*)0;
}

bool is_dev_exist_in_dev_list(uint32_t lora_addr) {
	int dev_list_len = list_flood_sensor.size();
	uint32_t ip_addr;
	for (int i = 0; i < dev_list_len; i++) {
		ip_addr = (uint32_t)inet_addr(static_cast<string>(list_flood_sensor[i]).c_str());
		if (ip_addr == lora_addr) {
			return true;
		}
	}
	return false;
}

void dev_list_set(vector<string>& list_flood_sensor) {
	flood_system_file_obj = open(flood_system_devs_file_path.c_str(), O_WRONLY | O_CREAT, 0666);

	if (flood_system_file_obj < 0) {
		APP_DBG("can't open %s file\n", flood_system_devs_file_path.c_str());
	}
	else {
		APP_DBG("opened %s file\n", flood_system_devs_file_path.c_str());
		json j_vector(list_flood_sensor);
		string str_j_vector = j_vector.dump();
	int8_t status =	pwrite(flood_system_file_obj, str_j_vector.c_str(), str_j_vector.length() + 1, 0);

	if (status == errno) {
		return;
	}
		close(flood_system_file_obj);
	}
}

void dev_list_get(vector<string>& list_flood_sensor) {
	struct stat file_info;
	flood_system_file_obj =  open(flood_system_devs_file_path.c_str(), O_RDONLY | O_CREAT, 0666);

	if (flood_system_file_obj < 0) {
		APP_DBG("can not open %s file\n", flood_system_devs_file_path.c_str());
	}
	else {
		APP_DBG("opened %s file\n", flood_system_devs_file_path.c_str());
		fstat(flood_system_file_obj, &file_info);

		uint32_t file_buffer_len = file_info.st_size + 1;
		char* file_buffer = (char*)malloc(file_buffer_len);

		if (file_buffer == NULL) {
			return;
		}

		memset(file_buffer, 0, file_buffer_len);

		int8_t status = pread(flood_system_file_obj, file_buffer, file_info.st_size, 0);

		if( status == errno) {
			return;
		}

		close(flood_system_file_obj);

		if (file_info.st_size) {
			json j = json::parse(file_buffer);

			if (j.size()) {
				/* clear list */
				list_flood_sensor.clear();

				/* update new element */
				for (json::iterator it = j.begin(); it != j.end(); ++it) {
					list_flood_sensor.push_back(*it);
				}
			}
		}

		free(file_buffer);
	}
}

void dev_list_show(vector<string>& list_flood_sensor) {
	int dev_list_len = list_flood_sensor.size();
	//cout << "DEV LIST LEN : " << dev_list_len << endl;
	for (int i = 0; i < dev_list_len; i++) {
		cout << "[" << i << "] " <<  list_flood_sensor[i] << endl;
	}
}

void flood_system_log_write(lora_message_t* lora_message) {
	struct in_addr des_addr;
	struct in_addr scr_addr;
	des_addr.s_addr = lora_message->header.des_addr;
	scr_addr.s_addr = lora_message->header.scr_addr;

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	static char log_time_buffer[20];
	strftime(log_time_buffer, sizeof(log_time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

	flood_system_sensor_file_obj = fopen(flood_system_sensor_log_file_path.c_str(), "a");
	if (flood_system_sensor_file_obj != NULL) {
		fprintf(flood_system_sensor_file_obj, "[ %s ]\tdes_addr: %-15s scr_addr: %-15s type: %-5d data: %d\n", \
				log_time_buffer,						\
				(const char*)inet_ntoa(des_addr),	\
				(const char*)inet_ntoa(scr_addr),	\
				lora_message->header.type,			\
				lora_message->data);

		fclose(flood_system_sensor_file_obj);
	}
}
