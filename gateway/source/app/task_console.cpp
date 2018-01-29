#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ak/ak.h"
#include "../ak/timer.h"

#include "../common/cmd_line.h"
#include "../common/base64.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_config.h"
#include "app_cmd.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_console.h"

q_msg_t mt_task_console_mailbox;

static uint8_t cls_cmd_buffer[CMD_BUFFER_SIZE];

void* mt_task_console_entry(void*) {
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_console_entry\n");

	ak_msg_t* msg;

	while (1) {

		/* get messge */
		msg = msg_get(MT_TASK_CONSOLE_ID);

		/* handler message */
		switch (msg->header->sig) {

		case MT_CONSOLE_INTERNAL_LOGIN_CMD: {

			get_data_dynamic_msg(msg, cls_cmd_buffer, msg->header->len);

			switch (cmd_line_parser(lgn_cmd_table, cls_cmd_buffer)) {
			case CMD_SUCCESS:
				break;

			case CMD_NOT_FOUND:
				if (cls_cmd_buffer[0] != '\r' &&
						cls_cmd_buffer[0] != '\n') {
					APP_PRINT("ERROR: cmd unknown\n");
				}
				break;

			case CMD_TOO_LONG:
				APP_PRINT("ERROR: cmd too long\n");
				break;

			case CMD_TBL_NOT_FOUND:
				APP_PRINT("ERROR: cmd table not found\n");
				break;

			default:
				APP_PRINT("ERROR: cmd error\n");
				break;
			}

			/* clear recent data */
			memset(cls_cmd_buffer, 0, CMD_BUFFER_SIZE);
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
