#ifndef __IF_LORA_USB_STICK_H__
#define __IF_LORA_USB_STICK_H__

#include <stdint.h>

#include "../ak/message.h"

extern q_msg_t mt_task_if_lora_usb_stick_mailbox;
extern void* mt_task_if_lora_usb_stick_entry(void*);

#endif //__IF_LORA_USB_STICK_H__
