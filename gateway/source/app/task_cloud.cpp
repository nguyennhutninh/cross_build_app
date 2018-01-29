#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <ctime>
#include <string>
//#include <curl/curl.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/json.hpp"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "mqtt_sl_sensor.h"
#include "mqtt_temperature_cont.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_cloud.h"

using namespace std;

q_msg_t mt_task_cloud_mailbox;

void* mt_task_cloud_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_cloud_entry\n");

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_CLOUD_ID);

		switch (msg->header->sig) {
		case MT_CLOUD_SL_SYNC_OK: {
			APP_DBG("MT_CLOUD_SL_SYNC_OK\n");
		}
			break;

		case MT_CLOUD_SL_SYNC_ERR: {
			APP_DBG("MT_CLOUD_SL_SYNC_ERR\n");
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

