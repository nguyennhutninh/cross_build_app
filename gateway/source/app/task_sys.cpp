#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_sys.h"

q_msg_t mt_task_sys_mailbox;

void* mt_task_sys_entry(void*) {
	wait_all_tasks_started();

	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_SYS_ID);

		switch (msg->header->sig) {

		case MT_SYS_WATCH_DOG_REPORT_REQ: {
			APP_DBG("MT_SYS_WATCH_DOG_REPORT_REQ\n");
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
