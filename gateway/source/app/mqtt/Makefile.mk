CXXFLAGS	+= -I./source/app/mqtt

VPATH += source/app/mqtt

OBJ += $(OBJ_DIR)/mqtt_sl_sensor.o
OBJ += $(OBJ_DIR)/mqtt_temperature_cont.o
OBJ += $(OBJ_DIR)/mqtt_io_cont.o
