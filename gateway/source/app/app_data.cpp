#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "app_data.h"
#include "app_dbg.h"
#include "app_if.h"

uint8_t rf24_encrypt_decrypt_key[] = {0x4C, 0xD4, 0x00, 0x08, 0x37, 0xB5, 0x18, 0x4C, 0x01, 0x21, 0x20, 0x46, 0x04, 0xF0, 0xC7, 0xF9};

if_app_t if_app_list[] = {
	/*	[id]				[socket_path]							[location_path]									[enable_laucher]*/
	{	IF_TYPE_APP_GMNG,	(char*)"/tmp/outdoor_app_manager",		(char*)"/usr/local/bin/outdoor_app_manager",	ENABLE_LAUNCHER		},
	{	IF_TYPE_APP_MT,		(char*)"/tmp/outdoor_master",			(char*)"/usr/local/bin/outdoor_master",			ENABLE_LAUNCHER		},
	{	IF_TYPE_APP_GU,		(char*)"/tmp/outdoor_master_ui",		(char*)"/usr/local/bin/outdoor_master_ui",		ENABLE_LAUNCHER		},
};

uint32_t if_app_list_size = sizeof(if_app_list)/sizeof(if_app_list[0]);

void show_app_table() {
	APP_DBG("\n[APPS TABLE]\n");
	APP_DBG("%-10s%-40s%-40s%-40s%-40s\n", "[id]", "[socket_path]", "[location_path]", "[enable_laucher]", "[pid]");
	for (uint32_t i = 0; i < if_app_list_size; i++) {
		APP_DBG("%-10d%-30s%-30s%-30s%-30d\n", if_app_list[i].id	\
				, if_app_list[i].socket_path	\
				, if_app_list[i].location_path	\
				, (const char*)((if_app_list[i].enable_launcher == ENABLE_LAUNCHER) ? "ENABLE_LAUNCHER" : "DISABLE_LAUNCHER")	\
				, if_app_list[i].pid);
	}
}

static gw_connection_t gw_connection_data;

/**
 * configure parameter will be updated when app start.
 */
static app_config_parameter_t config_parameter_data;

void app_data_set_gw_connection(gw_connection_t* gw_connect) {
	APP_DBG("[APP_DATA] app_data_set_sl_settings\n");
	if (gw_connect != NULL) {
		memcpy(&gw_connection_data, gw_connect, sizeof(gw_connection_t));
	}
}

void app_data_get_gw_connection(gw_connection_t* gw_connect) {
	APP_DBG("[APP_DATA] app_data_get_sl_settings\n");
	if (gw_connect != NULL) {
		memcpy(gw_connect, &gw_connection_data, sizeof(gw_connection_t));
	}
}

void app_data_set_config_parameter(app_config_parameter_t* config) {
	APP_DBG("[APP_DATA] app_data_set_config_parameter\n");
	if (config != NULL) {
		memcpy(&config_parameter_data, config, sizeof(app_config_parameter_t));
	}
}

void app_data_get_config_parameter(app_config_parameter_t* config) {
	APP_DBG("[APP_DATA] app_data_get_config_parameter\n");
	if (config != NULL) {
		memcpy(&config, &config_parameter_data, sizeof(app_config_parameter_t));
	}
}

uint8_t temperature[SL_TOTAL_SENSOR_TEMPERATURE];	/* *C */
uint8_t humidity[SL_TOTAL_SENSOR_HUMIDITY];			/* RH% */
uint8_t general_input[SL_TOTAL_GENERAL_INPUT];		/* HIGH/LOW */
uint8_t general_output[SL_TOTAL_GENERAL_OUTPUT];	/* HIGH/LOW */
uint8_t fan_pop[SL_TOTAL_FAN_POP];					/* (0 -> 100)% */
uint8_t fan_dev;									/* (0 -> 100)% */
uint8_t power_output_status;						/* ON/OFF */
uint32_t power_output_current;						/* mA */
