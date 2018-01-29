#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ak/ak.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"

#include "if_console.h"
#include "task_list.h"
#include "task_list_if.h"
#include "task_if.h"


q_msg_t mt_task_if_console_mailbox;

static unsigned char cmd_buf[CMD_BUFFER_SIZE];
static int i_get_command(unsigned char* cmd_buf);

void* mt_task_if_console_entry(void*) {

	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_if_console_entry\n");

	while (1) {
		if (i_get_command(cmd_buf) == 0) {
			ak_msg_t* s_msg = get_dynamic_msg();
			set_msg_sig(s_msg, MT_CONSOLE_INTERNAL_LOGIN_CMD);
			set_data_dynamic_msg(s_msg, cmd_buf, strlen((const char*)cmd_buf));

			set_msg_src_task_id(s_msg, MT_TASK_IF_CONSOLE_ID);
			task_post(MT_TASK_CONSOLE_ID, s_msg);

			/* clean command buffer */
			memset(cmd_buf, 0, CMD_BUFFER_SIZE);
		}
		usleep(100);
	}
	return (void*)0;
}

int i_get_command(unsigned char* cmd_buf) {
	unsigned char c = 0;
	unsigned char index = 0;

	do {
		c = getchar();
		cmd_buf[index++] = c;
		if (index > CMD_BUFFER_SIZE ) {
			index = 0;
			printf("ERROR: buffer overload !\n");
			return (-1);
		}

		/* sleep 100us */
		usleep(100);

	} while (c != '\n');

	cmd_buf[index] = 0;
	return (0);
}
