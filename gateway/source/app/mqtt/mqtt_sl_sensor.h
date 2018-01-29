#ifndef __MQTT_LS_SENSOR_H__
#define __MQTT_LS_SENSOR_H__
#include <stdint.h>
#include <string>

#include <mosquittopp.h>
#include "task_cloud.h"

using namespace std;

class mqtt_sl_sensor : public mosqpp::mosquittopp {
public:
	mqtt_sl_sensor(const char *id, const char *host, int port);
	~mqtt_sl_sensor();

	void set_topic(const char* topic);

	void sensor_public(uint8_t* msg, uint32_t len);

	/* call back functions */
	void on_connect(int rc);
	void on_publish(int mid);

private:
	string m_topic;
	char m_connect_ok_flag;
	int m_mid;
};

#endif //__MQTT_LS_SENSOR_H__
