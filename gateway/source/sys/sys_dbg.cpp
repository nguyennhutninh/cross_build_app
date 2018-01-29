#include <stdlib.h>
#include "sys_dbg.h"
#include "time.h"

void sys_dbg_fatal(const char* s, uint8_t c) {
	printf("[FATAL] %s %X\n", s, c);
	exit(EXIT_FAILURE);
}

char* sys_dbg_get_time(void) {
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	static char return_val[20];
	strftime(return_val, sizeof(return_val), "%Y-%m-%d %H:%M:%S", timeinfo);

	return (char*)return_val;
}
