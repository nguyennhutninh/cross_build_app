###############################################################################
# Author: ThanNT
# Date: 21/05/2017
###############################################################################
RPI_SYS_DIR	=
TOOLS_DIR	=
NAME_MODULE	= outdoor_master
OPTIMIZE	= -g -Os
CXX		= arm-linux-gnueabihf-g++-4.9
CC		= arm-linux-gnueabihf-gcc-4.9
OBJ_DIR		= $(NAME_MODULE)_build

-include source/ak/Makefile.mk
-include source/sys/Makefile.mk
-include source/app/Makefile.mk
-include source/common/Makefile.mk
-include source/driver/Makefile.mk

CXXFLAGS	+= -I/usr/local/include
CXXFLAGS	+= -I/usr/include

# CXX compiler option
CXXFLAGS	+=	$(OPTIMIZE)	\
		-std=c++11		\
		-Wall			\
		-Winline		\
		-pipe			\
		-g			\

# Library paths
LDFLAGS	+= -L./external_library
LDFLAGS	+= -L/usr/local/lib
LDFLAGS	+= -L/usr/include

#Library libs
LDLIBS	+=	-lpthread				\
		-lmosquitto				\
		-lmosquittopp				\
		-lrt					\
		-lm					\
		-lcrypto				\
		-lcurl					\
		-Xlinker -rpath=./external_library	\

all: create $(OBJ_DIR)/$(NAME_MODULE)

create:
	@echo mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp
	@echo CXX $<
	@$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	@echo CXX $<
	@$(CC) -c -o $@ $< $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/$(NAME_MODULE): $(OBJ)
	@echo ---------- START LINK PROJECT ----------
	@echo $(CXX) -o $@ $^ $(CXXFLAGS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

copy:
	@echo rm -rf $(OBJ_DIR)/*.o
	@rm -rf $(OBJ_DIR)/*.o
	scp -r ./* root@192.168.2.57:/root/workspace/pop_outdoor_manager/outdoor_master_gateway

flash:
	@echo rm -rf $(OBJ_DIR)/*.o
	@rm -rf $(OBJ_DIR)/*.o
	scp -r ./* root@192.168.2.57:/root/workspace/pop_outdoor_manager/outdoor_master_gateway

debug:
	sudo gdb $(OBJ_DIR)/$(NAME_MODULE)

install:
	cp $(OBJ_DIR)/$(NAME_MODULE) /usr/local/bin

clean:
	@echo rm -rf $(OBJ_DIR)
	@rm -rf $(OBJ_DIR)
