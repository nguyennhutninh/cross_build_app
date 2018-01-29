#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/base64.h"

#include "app.h"
#include "app_dbg.h"
#include "app_config.h"

string config_folder;
string config_file_path;

app_config::app_config() {

}

void app_config::initializer(char * file_name) {
	struct stat st = {0};

	string config_folder = static_cast<string>(APP_ROOT_PATH_DISK) + static_cast<string>("/config");

	/* create app root path */
	if (stat(config_folder.data(), &st) == -1) {
		mkdir(config_folder.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	string config_file_path = config_folder + static_cast<string>((const char*)file_name);

	strcpy(m_config_path, config_file_path.data());
}

void app_config::set_config_path_file(char* path) {
	strcpy(m_config_path, (const char*)path);
}

void app_config::get_config_path_file(char* path) {
	strcpy(path, (const char*)m_config_path);
}

int app_config::parser_config_file(app_config_parameter_t* cfg) {
	struct stat file_info;
	int configure_file_obj = -1;
	int buffer_len;
	char * buffer;
	char temp_string[CONFIGURE_PARAMETER_BUFFER_SIZE];

	configure_file_obj = open(m_config_path, O_RDONLY);

	if (configure_file_obj < 0) {
		return -1;
	}

	fstat(configure_file_obj, &file_info);

	buffer_len = file_info.st_size + 1;
	buffer = (char*)malloc(buffer_len);

	if (buffer == NULL) {
		return -1;
	}

	memset(buffer, 0, buffer_len);

	/*get data*/
	int8_t status = pread(configure_file_obj, buffer, file_info.st_size, 0);

	if(status == -1) {printf("cannot pread file\n"); }

	close(configure_file_obj);

	/*parse data*/
	json j = json::parse(buffer);

	/* add mqtt server infomation */
	memset(cfg->pop_gateway.host				, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.gateway_id_prefix	, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.gateway_id			, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.user_name_view		, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.user_psk_view		, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.user_name_control	, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->pop_gateway.user_psk_control	, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);

	strcpy(cfg->pop_gateway.host,				j["pop_gateway"]["host"].get<string>().data());
	strcpy(cfg->pop_gateway.gateway_id_prefix,	j["pop_gateway"]["gateway_id_prefix"].get<string>().data());
	strcpy(cfg->pop_gateway.gateway_id,			j["pop_gateway"]["gateway_id"].get<string>().data());
	cfg->pop_gateway.port =						j["pop_gateway"]["port"];
	strcpy(cfg->pop_gateway.user_name_view,		j["pop_gateway"]["user_name_view"].get<string>().data());

	memset(temp_string, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	Base64decode(temp_string,					j["pop_gateway"]["user_psk_view"].get<string>().data());
	temp_string[strlen(temp_string) - strlen(cfg->pop_gateway.user_name_view)] = 0;
	strcpy(cfg->pop_gateway.user_psk_view, temp_string);
	strcpy(cfg->pop_gateway.user_name_control,	j["pop_gateway"]["user_name_control"].get<string>().data());

	memset(temp_string, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	Base64decode(temp_string,					j["pop_gateway"]["user_psk_control"].get<string>().data());
	temp_string[strlen(temp_string) - strlen(cfg->pop_gateway.user_name_control)] = 0;
	strcpy(cfg->pop_gateway.user_psk_control, temp_string);

	/* add lora gateway info */
	memset(cfg->lora_gateway.lora_host			, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->lora_gateway.mqtt_host			, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->lora_gateway.mqtt_user_name		, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);
	memset(cfg->lora_gateway.mqtt_psk			, 0, CONFIGURE_PARAMETER_BUFFER_SIZE);

	strcpy(cfg->lora_gateway.lora_host,			j["lora_gateway"]["lora_host"].get<string>().data());
	strcpy(cfg->lora_gateway.mqtt_host,			j["lora_gateway"]["mqtt_host"].get<string>().data());
	cfg->lora_gateway.mqtt_port =				j["lora_gateway"]["mqtt_port"];

	strcpy(cfg->lora_gateway.mqtt_user_name,	j["lora_gateway"]["mqtt_user_name"].get<string>().data());
	strcpy(cfg->lora_gateway.mqtt_psk,			j["lora_gateway"]["mqtt_psk"].get<string>().data());

	free(buffer);

	return 0;
}

int app_config::write_config_data(app_config_parameter_t * cfg) {
	json js_config = {
		{
			"lora_gateway", {
				{"lora_host", cfg->lora_gateway.lora_host},
				{"mqtt_host", cfg->lora_gateway.mqtt_host},
				{"mqtt_port", cfg->lora_gateway.mqtt_port},
				{"mqtt_user_name", cfg->lora_gateway.mqtt_user_name},
				{"mqtt_psk", cfg->lora_gateway.mqtt_psk},
			}
		},
		{
			"pop_gateway", {
				{"host", cfg->pop_gateway.host},
				{"gateway_id_prefix", cfg->pop_gateway.gateway_id_prefix},
				{"gateway_id", cfg->pop_gateway.gateway_id},
				{"port", cfg->pop_gateway.port},

				{"user_name_view", cfg->pop_gateway.user_name_view},
				{"user_psk_view", cfg->pop_gateway.user_psk_view},
				{"user_name_control", cfg->pop_gateway.user_name_control},
				{"user_psk_control", cfg->pop_gateway.user_psk_control},
			}
		}
	};

	string config = js_config.dump();
	const char* buffer = config.data();

	FILE* js_config_obj = fopen(m_config_path, "w");

	if (js_config_obj == NULL) {
		return -1;
	}

	fwrite(buffer, 1, config.length(), js_config_obj);

	fclose(js_config_obj);

	return 0;
}


