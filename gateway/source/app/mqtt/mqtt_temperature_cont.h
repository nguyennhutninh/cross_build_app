#ifndef __MQTT_TEMPERATURE_CONT_H__
#define __MQTT_TEMPERATURE_CONT_H__

#include <stdint.h>
#include <string>

#include <mosquittopp.h>
#include "task_cloud.h"

using namespace std;

class mqtt_temperature_cont : public mosqpp::mosquittopp {
public:
	mqtt_temperature_cont(const char *id, const char *host, int port);
	~mqtt_temperature_cont();

	void set_topic(const char* topic);

	void control_public(uint8_t* msg, uint32_t len);

	/* call back functions */
	void on_connect(int rc);
	void on_publish(int mid);
	void on_subscribe(int mid, int qos_count, const int *granted_qos);
	void on_message(const struct mosquitto_message *message);

private:
	string m_topic;
	char m_connect_ok_flag;
	int m_mid;
};

#endif //__MQTT_TEMPERATURE_CONT_H__
