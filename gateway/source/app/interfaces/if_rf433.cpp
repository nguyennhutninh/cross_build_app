#if 0

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../driver/rc-switch/RCSwitch.h"

#include "../ak/ak.h"

#include "app.h"
#include "app_dbg.h"

#include "if_rf24.h"
#include "task_list.h"

q_msg_t mt_task_if_rf433_mailbox;

RCSwitch sersor_433 = RCSwitch();

void* mt_task_if_rf433_entry(void*) {
	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_if_rf433_entry\n");

	sersor_433.enableReceive(1);

	while (1) {
		if (sersor_433.available()) {
			/* get code */
			uint32_t recv_code = sersor_433.getReceivedValue();

			/* post code to task rf433 */
			ak_msg_t* s_msg = get_common_msg();
			set_msg_sig(s_msg, MT_RF433_RECV_NOTIFY);
			set_data_common_msg(s_msg, (uint8_t*)&recv_code, sizeof(uint32_t));

			set_msg_src_task_id(s_msg, MT_TASK_IF_RF433_ID);
			task_post(MT_TASK_RF433_ID, s_msg);

			sersor_433.resetAvailable();
		}

		/* sleep */
		usleep(1000);
	}

	return (void*)0;
}
#endif
