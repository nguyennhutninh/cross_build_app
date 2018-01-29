#include "mqtt_sl_sensor.h"

#include "app_dbg.h"
#include "app.h"

mqtt_sl_sensor::mqtt_sl_sensor(const char *id, const char *host, int port) : mosquittopp(id) {
	/* init private data */
	m_connect_ok_flag = -1;
	m_mid = 1;

	/* init mqtt */
	mosqpp::lib_init();

	/* connect */
	username_pw_set(gateway_configure_parameter.pop_gateway.user_name_view, gateway_configure_parameter.pop_gateway.user_psk_view);
	connect_async(host, port, 60);
	loop_start();
}

mqtt_sl_sensor::~mqtt_sl_sensor() {
	loop_stop();
	mosqpp::lib_cleanup();
}

void mqtt_sl_sensor::set_topic(const char* topic) {
	m_topic = static_cast<string>(topic);
}

void mqtt_sl_sensor::on_connect(int rc) {
	if (rc == 0) {
		m_connect_ok_flag = 0;
		APP_DBG("[mqtt_sl_sensor] on_connect OK\n");
	}
	else {
		APP_DBG("[mqtt_sl_sensor] on_connect ERROR\n");
	}
}

void mqtt_sl_sensor::sensor_public(uint8_t* msg, uint32_t len) {
	APP_DBG("[mqtt_sl_sensor][sensor_public] msg:%s len:%d\n", msg, len);
	publish(&m_mid, m_topic.data(), len, msg);
}

void mqtt_sl_sensor::on_publish(int mid) {
	APP_DBG("[mqtt_sl_sensor][on_publish] mid: %d\n", mid);
}
