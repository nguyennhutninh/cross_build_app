#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <stdint.h>
#include <string>

#include "app.h"
#include "app_data.h"

#include "ak.h"
#include "ak_dbg.h"

#include "../sys/sys_dbg.h"

#include "../common/json.hpp"

using namespace std;
using json = nlohmann::json;

class app_config {
public:
	app_config();
	void initializer(char*);
	void set_config_path_file(char*);
	void get_config_path_file(char*);
	int parser_config_file(app_config_parameter_t*);
	int write_config_data(app_config_parameter_t*);

private:
	char m_config_path[256];
};

#endif //__APP_CONFIG_H__
