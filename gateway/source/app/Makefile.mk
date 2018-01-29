-include source/app/mqtt/Makefile.mk
-include source/app/interfaces/Makefile.mk

CXXFLAGS	+= -I./source/app

VPATH += source/app

OBJ += $(OBJ_DIR)/app.o
OBJ += $(OBJ_DIR)/app_data.o
OBJ += $(OBJ_DIR)/app_config.o
OBJ += $(OBJ_DIR)/app_cmd.o

OBJ += $(OBJ_DIR)/task_list.o
OBJ += $(OBJ_DIR)/task_console.o
OBJ += $(OBJ_DIR)/task_snmp.o
OBJ += $(OBJ_DIR)/task_cloud.o
OBJ += $(OBJ_DIR)/task_if.o
OBJ += $(OBJ_DIR)/task_debug_msg.o
OBJ += $(OBJ_DIR)/task_sm.o
OBJ += $(OBJ_DIR)/task_sensor.o
OBJ += $(OBJ_DIR)/task_sys.o
OBJ += $(OBJ_DIR)/task_firmware.o
OBJ += $(OBJ_DIR)/task_log.o
OBJ += $(OBJ_DIR)/task_modbus.o
