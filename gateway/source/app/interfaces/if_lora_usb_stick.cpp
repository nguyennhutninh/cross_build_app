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
#include <list>

#include "../ak/ak.h"

#include "../sys/sys_dbg.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"

#include "task_list.h"
#include "task_list_if.h"
#include "if_lora_usb_stick.h"

#define IPCPU_SOP_CHAR		0xEF
#define IFCPU_DEVPATH		"/dev/loraUSBstick"
#define IFCPU_DATA_SIZE		256

typedef struct {
	uint8_t frame_sop;
	lora_message_t lora_message;
	uint8_t frame_fcs;
} __attribute__((__packed__)) usb_stick_msg_frame_t;

typedef struct {
	usb_stick_msg_frame_t usb_stick_msg_frame;
	uint8_t data_index;
} __attribute__((__packed__)) usb_stick_msg_frame_mng_t;

static int lora_usb_fd;

static int lora_usb_opentty(const char* devpath);

static pthread_t lora_usb_rx_thread;
static void* lora_usb_rx_thread_handler(void*);
static uint8_t lora_usb_calcfcs(uint8_t *data_ptr);

static uint8_t flood_system_packaged_serial_msg(ak_msg_t* ak_msg);

#define SOP_STATE		0x00
#define DATA_STATE		0x01
#define FCS_STATE		0x02
static uint8_t rx_frame_state = SOP_STATE;

static usb_stick_msg_frame_mng_t rev_usb_stick_msg_frame_mng;

static void rx_frame_parser(uint8_t data);

q_msg_t mt_task_if_lora_usb_stick_mailbox;

void* mt_task_if_lora_usb_stick_entry(void*) {
	task_mask_started();
	wait_all_tasks_started();

	APP_DBG("[STARTED] mt_task_if_lora_usb_stick_entry\n");

	if (lora_usb_opentty(IFCPU_DEVPATH) < 0) {
		APP_DBG("Cannot open %s !\n", IFCPU_DEVPATH);
	}
	else {
		APP_DBG("Opened %s success !\n", IFCPU_DEVPATH);	\
		pthread_create(&lora_usb_rx_thread, NULL, lora_usb_rx_thread_handler, NULL);
	}

	while (1) {
		while (msg_available(MT_TASK_IF_LORA_USB_STICK_ID)) {
			ak_msg_t* msg = rev_msg(MT_TASK_IF_LORA_USB_STICK_ID);

			switch (msg->header->sig) {
			case MT_IF_LORA_USB_STICK_FLOOD_SYSTEM_MSG_OUT :{
				APP_DBG("MT_IF_LORA_USB_STICK_FLOOD_SYSTEM_MSG_OUT\n");
				flood_system_packaged_serial_msg(msg);
			}

			default:
				break;
			}

			/* free message */
			free_msg(msg);
		}
	}

	return (void*)0;
}

void* lora_usb_rx_thread_handler(void*) {
	APP_DBG("lora_usb_rx_thread_handler entry successed!\n");
	uint32_t rx_read_len;
	uint8_t st_c = 0;

	while(1) {
		rx_read_len = read(lora_usb_fd, &st_c, 1);
		if (rx_read_len > 0) {
			rx_read_len = 0;
			rx_frame_parser(st_c);
		}

		usleep(1000);
	}

	return (void*)0;
}

int lora_usb_opentty(const char* devpath) {
	struct termios options;
	APP_DBG("[IF CPU SERIAL][lora_usb_opentty] devpath: %s\n", devpath);

	lora_usb_fd = open(devpath, O_RDWR | O_NOCTTY | O_NDELAY);
	if (lora_usb_fd < 0) {
		return lora_usb_fd;
	}
	else {
		fcntl(lora_usb_fd, F_SETFL, 0);

		/* get current status */
		tcgetattr(lora_usb_fd, &options);

		cfsetispeed(&options, B9600);
		cfsetospeed(&options, B9600);

		/* No parity (8N1) */
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;

		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag     &=  ~CRTSCTS;

		cfmakeraw(&options);

		tcflush(lora_usb_fd, TCIFLUSH);
		if (tcsetattr (lora_usb_fd, TCSANOW, &options) != 0) {
			SYS_DBG("error in tcsetattr()\n");
		}
	}
	return 0;
}

/* Calculate lora_usb frame FCS */
uint8_t lora_usb_calcfcs(uint8_t *data_ptr) {
	uint8_t xor_result = 0;
	for (uint32_t i = 0; i < sizeof(lora_message_t); i++, data_ptr++) {
		xor_result = xor_result ^ *data_ptr;
	}
	return xor_result;
}

void rx_frame_parser(uint8_t data) {
	switch (rx_frame_state) {
	case SOP_STATE: {
		if (IPCPU_SOP_CHAR == data) {
			rx_frame_state = DATA_STATE;
			rev_usb_stick_msg_frame_mng.data_index = 0;
		}
	}
		break;

	case DATA_STATE: {
		((uint8_t*)&(rev_usb_stick_msg_frame_mng.usb_stick_msg_frame.lora_message))[rev_usb_stick_msg_frame_mng.data_index++] = data;

		if (rev_usb_stick_msg_frame_mng.data_index == sizeof(lora_message_t)) {
			rx_frame_state = FCS_STATE;
		}
	}
		break;

	case FCS_STATE: {
		rx_frame_state = SOP_STATE;

		rev_usb_stick_msg_frame_mng.usb_stick_msg_frame.frame_fcs = data;

		if (rev_usb_stick_msg_frame_mng.usb_stick_msg_frame.frame_fcs \
				== lora_usb_calcfcs((uint8_t*)&rev_usb_stick_msg_frame_mng.usb_stick_msg_frame.lora_message)) {
#if 0
			APP_DBG("src addr: %d\n", rev_lora_msg.header.scr_addr);
			APP_DBG("des addr: %d\n", rev_lora_msg.header.des_addr);
			APP_DBG("msg type: %d\n", rev_lora_msg.header.type);
			APP_DBG("msg data: %d\n", rev_lora_msg.data);
#endif
			ak_msg_t* msg = get_common_msg();
			set_msg_sig(msg, MT_FLOOD_SYSTEM_INCOMMING);
			set_data_common_msg(msg, (uint8_t*)&rev_usb_stick_msg_frame_mng.usb_stick_msg_frame.lora_message, sizeof(lora_message_t));
			//task_post(MT_TASK_FLOOD_SYSTEM_ID, msg);
		}
		else {
			/* TODO: handle checksum incorrect */
			APP_DBG("lora checksum ERR\n");
		}
	}
		break;

	default:
		break;
	}
}

uint8_t flood_system_packaged_serial_msg(ak_msg_t* ak_msg) {
	usb_stick_msg_frame_mng_t send_usb_stick_msg_frame_mng;

	send_usb_stick_msg_frame_mng.usb_stick_msg_frame.frame_sop = IPCPU_SOP_CHAR;
	get_data_common_msg(ak_msg, (uint8_t*)&send_usb_stick_msg_frame_mng.usb_stick_msg_frame.lora_message, sizeof(lora_message_t));
	send_usb_stick_msg_frame_mng.usb_stick_msg_frame.frame_fcs = lora_usb_calcfcs((uint8_t*)&send_usb_stick_msg_frame_mng.usb_stick_msg_frame.lora_message);

	//write(lora_usb_fd,(uint8_t*)&send_usb_stick_msg_frame_mng, sizeof(usb_stick_msg_frame_t));

	return 0;
}

