#include <sys/types.h>
#include <sys/stat.h>

#include "../ak/ak.h"

#include "app.h"
#include "app_dbg.h"

app_config gateway_configure;
app_config_parameter_t gateway_configure_parameter;

void task_init() {
	struct stat st = {0};

	/* create app root path on DISK */
	if (stat(APP_ROOT_PATH_DISK, &st) == -1) {
		mkdir(APP_ROOT_PATH_DISK, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	/* create app root path on RAM */
	if (stat(APP_ROOT_PATH_RAM, &st) == -1) {
		mkdir(APP_ROOT_PATH_RAM, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

	gateway_configure.initializer((char*)"/mqtt_config.json");

	gateway_configure.parser_config_file(&gateway_configure_parameter);
}
