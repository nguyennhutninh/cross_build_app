#include <string.h>

#include "../ak/ak.h"

#include "mqtt_temperature_cont.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"

mqtt_temperature_cont::mqtt_temperature_cont(const char *id, const char *host, int port) : mosquittopp(id) {
	/* init private data */
	m_connect_ok_flag = -1;
	m_mid = 1;

	/* init mqtt */
	mosqpp::lib_init();

	/* connect */
	username_pw_set(gateway_configure_parameter.pop_gateway.user_name_control, gateway_configure_parameter.pop_gateway.user_psk_control);
	connect_async(host, port, 60);
	loop_start();
}

mqtt_temperature_cont::~mqtt_temperature_cont() {
	loop_stop();
	mosqpp::lib_cleanup();
}

void mqtt_temperature_cont::set_topic(const char* topic) {
	m_topic = static_cast<string>(topic);
}

void mqtt_temperature_cont::on_connect(int rc) {
	if (rc == 0) {
		m_connect_ok_flag = 0;
		APP_DBG("[MQTT_CONTROL] on_connect OK\n");
		subscribe(NULL, m_topic.data());
	}
	else {
		APP_DBG("[MQTT_CONTROL] on_connect ERROR\n");
	}
}

void mqtt_temperature_cont::control_public(uint8_t* msg, uint32_t len) {
	APP_DBG("[MQTT_CONTROL][control_public] msg:%s len:%d\n", msg, len);
	publish(&m_mid, m_topic.data(), len, msg, true);
}

void mqtt_temperature_cont::on_publish(int mid) {
	APP_DBG("[MQTT_CONTROL][on_publish] mid: %d\n", mid);
}

void mqtt_temperature_cont::on_subscribe(int mid, int qos_count, const int *granted_qos) {
	(void)granted_qos;
	APP_DBG("[MQTT_CONTROL][on_subscribe] mid:%d\tqos_count:%d\n", mid, qos_count);
}

void mqtt_temperature_cont::on_message(const struct mosquitto_message *message) {
	APP_DBG("[MQTT_CONTROL][on_message] topic:%s\tpayloadlen:%d\n", message->topic, message->payloadlen);
	if (!strcmp(message->topic, m_topic.data())) {
		if (message->payloadlen > 0) {
			int payload_len = message->payloadlen + 1;
			char* payload = (char*)malloc(payload_len);
			memset(payload, 0 , payload_len);

			memcpy(payload, message->payload, message->payloadlen);

			APP_DBG("[MQTT_CONTROL][on_message] message->payload:%s\n", payload);

			/* free temp payload */
			free(payload);
		}
	}
}
