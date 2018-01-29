/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   13/08/2016
 ******************************************************************************
**/

#include <string.h>
#include "cmd_line.h"
#include "stdio.h"
#include "time.h"

uint8_t cmd_line_parser(cmd_line_t* cmd_table, uint8_t* command){
	uint8_t     cmd[MAX_CMD_SIZE];
	uint8_t*    p_command = command;
	uint8_t     cmd_index = 0;
	uint8_t     indexCheck = 0;

	if (cmd_table == (cmd_line_t*)0) {
		return CMD_TBL_NOT_FOUND;
	}

	/* get cmd */
	while(*p_command) {
		if (*p_command == ' ' || *p_command == '\r' || *p_command == '\n') {
			cmd[cmd_index] = 0;
			break;
		}
		else {
			cmd[cmd_index++] = *(p_command++);
			if (cmd_index >= MAX_CMD_SIZE) {
				return CMD_TOO_LONG;
			}
		}
	}

	/* find respective command in command table */
	while(cmd_table[indexCheck].cmd){

		if (strcmp((const char*)cmd_table[indexCheck].cmd, (const char*)cmd) == 0) {

			/* perform respective function */
			cmd_table[indexCheck].func(command);

			/* return success */
			return CMD_SUCCESS;
		}

		indexCheck++;
	}

	return CMD_NOT_FOUND;  /* command not found */
}

char* print_time(char * buf) {
	int size = 0;
	time_t t;

	t=time(NULL); /* get current calendar time */

	char *timestr = asctime( localtime(&t) );
	timestr[strlen(timestr) - 1] = 0;  //Getting rid of \n

	size = strlen(timestr)+ 1 + 2; //Additional +2 for square braces
	memset(buf, 0x0, size);
	snprintf(buf,size,"[%s]", timestr);

	return buf;
}


void Log(char *message) {
	FILE *file;

	file = fopen(LOGFILE, "a");

	if (file != NULL) {
		fprintf(file, "%s\n",message);
		fclose(file);
	}
	else
		printf("Cannot open gl.log file\n");
}

void APP_LOG(const char* signal) {

	char time_array[40];
	const char* signal_log = signal;
	const char* time_log = (const char *) print_time(time_array);
	char log[200];

	strcpy(log, time_log);
	strcat(log, signal_log);
	Log(log);
}

