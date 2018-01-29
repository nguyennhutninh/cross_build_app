#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>

#include "../ak/ak.h"

#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "if_cpu_serial.h"

#define IPCPU_SOP_CHAR		0xEF
#define IFCPU_DEVPATH		"/dev/ttyS3"
//#define IFCPU_DEVPATH		"/dev/ttyUSB0"
#define IFCPU_DATA_SIZE		256

#define RX_BUFFER_SIZE		256

typedef struct {
	uint8_t frame_sop;
	uint32_t len;
	uint8_t data_index;
	uint8_t data[IFCPU_DATA_SIZE];
	uint8_t frame_fcs;
} if_cpu_serial_frame_t;

q_msg_t mt_task_if_cpu_serial_mailbox;

static int if_cpu_serial_fd;

static int if_cpu_serial_opentty(const char* devpath);
static uint8_t if_cpu_serial_calcfcs(uint8_t len, uint8_t *data_ptr);

static pthread_t if_cpu_serial_rx_thread;
static void* if_cpu_serial_rx_thread_handler(void*);

#define SOP_STATE		0x00
#define LEN_STATE		0x01
#define DATA_STATE		0x02
#define FCS_STATE		0x03
static uint8_t rx_frame_state = SOP_STATE;

#define RX_FRAME_PARSER_FAILED		(-1)
#define RX_FRAME_PARSER_SUCCESS		(0)
#define RX_FRAME_PARSER_rx_remain		(1)

static if_cpu_serial_frame_t if_cpu_serial_frame;
static void rx_frame_parser(uint8_t* data, uint8_t len);
int tx_frame_post(uint8_t* data, uint8_t len);

static uint8_t tx_buffer[1024];

void* mt_task_if_cpu_serial_entry(void*) {

	wait_all_tasks_started();
	
	APP_DBG("[STARTED] mt_task_if_cpu_serial_entry\n");
	
	if (if_cpu_serial_opentty(IFCPU_DEVPATH) < 0) {
		APP_DBG("Cannot open %s !\n", IFCPU_DEVPATH);
	}
	else {
		APP_DBG("Opened %s success !\n", IFCPU_DEVPATH);\
		pthread_create(&if_cpu_serial_rx_thread, NULL, if_cpu_serial_rx_thread_handler, NULL);
	}
	
	ak_msg_t* msg;

	while (1) {
		/* get messge */
		msg = msg_get(MT_TASK_IF_CPU_SERIAL_ID);

		switch (get_msg_type(msg)) {
		case PURE_MSG_TYPE: {
			//APP_DBG("[IF CPU SERIAL][SEND] PURE_MSG_TYPE\n");
			ak_msg_pure_if_t app_if_msg;
			memset(&app_if_msg, 0, sizeof(ak_msg_pure_if_t));

			/* assign if message */
			app_if_msg.header.type = PURE_MSG_TYPE;
			app_if_msg.header.if_src_type = IF_TYPE_CPU_SERIAL_MT;
			app_if_msg.header.if_des_type = msg->header->if_des_type;
			app_if_msg.header.sig = msg->header->if_sig;
			app_if_msg.header.src_task_id = msg->header->if_src_task_id;
			app_if_msg.header.des_task_id = msg->header->if_des_task_id;

			tx_frame_post((uint8_t*)&app_if_msg, sizeof(ak_msg_pure_if_t));
		}
			break;

		case COMMON_MSG_TYPE: {
			//APP_DBG("[IF CPU SERIAL][SEND] COMMON_MSG_TYPE\n");
			ak_msg_common_if_t app_if_msg;
			memset(&app_if_msg, 0, sizeof(ak_msg_common_if_t));

			/* assign if message */
			app_if_msg.header.type = COMMON_MSG_TYPE;
			app_if_msg.header.if_src_type = IF_TYPE_CPU_SERIAL_MT;
			app_if_msg.header.if_des_type = msg->header->if_des_type;
			app_if_msg.header.sig = msg->header->if_sig;
			app_if_msg.header.src_task_id = msg->header->if_src_task_id;
			app_if_msg.header.des_task_id = msg->header->if_des_task_id;

			app_if_msg.len = msg->header->len;
			get_data_common_msg(msg, app_if_msg.data, msg->header->len);

			tx_frame_post((uint8_t*)&app_if_msg, sizeof(ak_msg_common_if_t));
		}
			break;

		case DYNAMIC_MSG_TYPE: {
			//APP_DBG("[IF CPU SERIAL][SEND] COMMON_MSG_TYPE\n");
			ak_msg_dynamic_if_t app_if_msg;
			uint32_t app_if_msg_len;

			/* assign if message */
			app_if_msg.header.type			= DYNAMIC_MSG_TYPE;
			app_if_msg.header.if_src_type	= IF_TYPE_CPU_SERIAL_MT;
			app_if_msg.header.if_des_type	= msg->header->if_des_type;
			app_if_msg.header.sig			= msg->header->if_sig;
			app_if_msg.header.src_task_id	= msg->header->if_src_task_id;
			app_if_msg.header.des_task_id	= msg->header->if_des_task_id;

			app_if_msg.len = msg->header->len;
			app_if_msg.data = (uint8_t*)malloc(app_if_msg.len);
			get_data_dynamic_msg(msg, app_if_msg.data, app_if_msg.len);

			app_if_msg_len = sizeof(ak_msg_if_header_t) + sizeof(uint32_t) + app_if_msg.len;

			tx_frame_post((uint8_t*)&app_if_msg, app_if_msg_len);
			free(app_if_msg.data);
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

void* if_cpu_serial_rx_thread_handler(void*) {
	//APP_DBG("if_cpu_serial_rx_thread_handler entry successed!\n");
	uint8_t rx_buffer[RX_BUFFER_SIZE];
	uint32_t rx_read_len;
	
	while(1) {
		rx_read_len = read(if_cpu_serial_fd, rx_buffer, RX_BUFFER_SIZE);
		if (rx_read_len > 0) {
			rx_frame_parser(rx_buffer, rx_read_len);
		}
		usleep(100);
	}
	
	return (void*)0;
}

int if_cpu_serial_opentty(const char* devpath) {
	struct termios options;
	//APP_DBG("[IF CPU SERIAL][if_cpu_serial_opentty] devpath: %s\n", devpath);
	
	if_cpu_serial_fd = open(devpath, O_RDWR | O_NOCTTY | O_NDELAY);
	if (if_cpu_serial_fd < 0) {
		return if_cpu_serial_fd;
	}
	else {
		fcntl(if_cpu_serial_fd, F_SETFL, 0);
		
		/* get current status */
		tcgetattr(if_cpu_serial_fd, &options);
		
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
		
		/* No parity (8N1) */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
		
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag     &=  ~CRTSCTS;
		
		cfmakeraw(&options);
		
		tcflush(if_cpu_serial_fd, TCIFLUSH);
		if (tcsetattr (if_cpu_serial_fd, TCSANOW, &options) != 0) {
			SYS_DBG("error in tcsetattr()\n");
		}
	}
	return 0;
}


/* Calculate IF_CPU_SERIAL frame FCS */
uint8_t if_cpu_serial_calcfcs(uint8_t len, uint8_t *data_ptr) {
	uint8_t xor_result;
	xor_result = len;
	
	for (int i = 0; i < len; i++, data_ptr++) {
		xor_result = xor_result ^ *data_ptr;
	}
	
	return xor_result;
}

void rx_frame_parser(uint8_t* data, uint8_t len) {
	uint8_t ch;
	int rx_remain;
	
	while(len) {
		
		ch = *data++;
		len--;
		
		switch (rx_frame_state) {
		case SOP_STATE: {
			if (IPCPU_SOP_CHAR == ch) {
				rx_frame_state = LEN_STATE;
			}
		}
			break;
			
		case LEN_STATE: {

			if (ch > IFCPU_DATA_SIZE) {
				rx_frame_state = SOP_STATE;
				return;
			}
			else {
				if_cpu_serial_frame.len = ch;
				if_cpu_serial_frame.data_index = 0;
				rx_frame_state = DATA_STATE;
			}
		}
			break;
			
		case DATA_STATE: {
			if_cpu_serial_frame.data[if_cpu_serial_frame.data_index++] = ch;
			
			rx_remain = if_cpu_serial_frame.len - if_cpu_serial_frame.data_index;
			
			if (len >= rx_remain) {
				memcpy((uint8_t*)(if_cpu_serial_frame.data + if_cpu_serial_frame.data_index), data, rx_remain);
				if_cpu_serial_frame.data_index += rx_remain;
				len -= rx_remain;
				data += rx_remain;
			}
			else {
				memcpy((uint8_t*)(if_cpu_serial_frame.data + if_cpu_serial_frame.data_index), data, len);
				if_cpu_serial_frame.data_index += len;
				len = 0;
			}
			
			if (if_cpu_serial_frame.data_index == if_cpu_serial_frame.len) {
				rx_frame_state = FCS_STATE;
			}
		}
			break;
			
		case FCS_STATE: {
			rx_frame_state = SOP_STATE;
			
			if_cpu_serial_frame.frame_fcs = ch;
			
			if (if_cpu_serial_frame.frame_fcs \
					== if_cpu_serial_calcfcs(if_cpu_serial_frame.len, if_cpu_serial_frame.data)) {
				
				ak_msg_if_header_t* if_msg_header = (ak_msg_if_header_t*)if_cpu_serial_frame.data;
				
				switch (if_msg_header->type) {
				case PURE_MSG_TYPE: {
					//APP_DBG("[IF CPU SERIAL][REV] PURE_MSG_TYPE");
					ak_msg_t* s_msg = get_pure_msg();
					
					set_if_src_task_id(s_msg, if_msg_header->src_task_id);
					set_if_des_task_id(s_msg, if_msg_header->des_task_id);
					set_if_src_type(s_msg, if_msg_header->if_src_type);
					set_if_des_type(s_msg, if_msg_header->if_des_type);
					set_if_sig(s_msg, if_msg_header->sig);
					
					set_msg_sig(s_msg, MT_IF_PURE_MSG_IN);
					set_msg_src_task_id(s_msg, MT_TASK_IF_CPU_SERIAL_ID);
					task_post(MT_TASK_IF_ID, s_msg);
				}
					break;
					
				case COMMON_MSG_TYPE: {
					//APP_DBG("[IF CPU SERIAL][REV] COMMON_MSG_TYPE");
					ak_msg_t* s_msg = get_common_msg();
					
					set_if_src_task_id(s_msg, if_msg_header->src_task_id);
					set_if_des_task_id(s_msg, if_msg_header->des_task_id);
					set_if_src_type(s_msg, if_msg_header->if_src_type);
					set_if_des_type(s_msg, if_msg_header->if_des_type);
					set_if_sig(s_msg, if_msg_header->sig);
					set_if_data_common_msg(s_msg, ((ak_msg_common_if_t*)if_msg_header)->data, ((ak_msg_common_if_t*)if_msg_header)->len);
					
					set_msg_sig(s_msg, MT_IF_COMMON_MSG_IN);
					set_msg_src_task_id(s_msg, MT_TASK_IF_CPU_SERIAL_ID);
					task_post(MT_TASK_IF_ID, s_msg);
				}
					break;
					
				case DYNAMIC_MSG_TYPE: {
					//APP_DBG("[IF CPU SERIAL][REV] DYNAMIC_MSG_TYPE\n");
					ak_msg_t* s_msg = get_dynamic_msg();
					
					set_if_src_task_id(s_msg, if_msg_header->src_task_id);
					set_if_des_task_id(s_msg, if_msg_header->des_task_id);
					set_if_src_type(s_msg, if_msg_header->if_src_type);
					set_if_des_type(s_msg, if_msg_header->if_des_type);
					set_if_sig(s_msg, if_msg_header->sig);

					((ak_msg_dynamic_if_t*)if_msg_header)->data = ((uint8_t*)if_cpu_serial_frame.data + sizeof(ak_msg_if_header_t) + sizeof(uint32_t));

					set_if_data_dynamic_msg(s_msg,((ak_msg_dynamic_if_t*)if_msg_header)->data, ((ak_msg_dynamic_if_t*)if_msg_header)->len);
					
					set_msg_sig(s_msg, MT_IF_DYNAMIC_MSG_IN);
					set_msg_src_task_id(s_msg, MT_TASK_IF_CPU_SERIAL_ID);
					task_post(MT_TASK_IF_ID, s_msg);
				}
					break;
					
				default:
					break;
				}
			}
			else {
				/* TODO: handle checksum incorrect */
			}
		}
			break;
			
		default:
			break;
		}
	}
}

int tx_frame_post(uint8_t* data, uint8_t len) {
	tx_buffer[0] = IPCPU_SOP_CHAR;
	tx_buffer[1] = len;
	memcpy(&tx_buffer[2], data, len);
	tx_buffer[2 + len] = if_cpu_serial_calcfcs(len, data);
	return write(if_cpu_serial_fd, tx_buffer, (len + 3));
}
