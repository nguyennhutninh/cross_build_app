/**
 ******************************************************************************
 * @author: ThanNT
 * @date:   12/01/2017
 * @brief: mailbox
 ******************************************************************************
**/
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdint.h>
#include <pthread.h>

#define AK_MSG_NULL ((ak_msg_t*)0)

#define DYNAMIC_MSG_TYPE				(0x40)
#define PURE_MSG_TYPE					(0x80)
#define COMMON_MSG_TYPE					(0xC0)

#define MAX_MSG_REF_COUNT				10

#define AK_MSG_TYPE_MASK			(0xC0)
#define AK_MSG_REF_COUNT_MASK		(0x3F)

#define get_fatal_msg_ref_count(x)		((((ak_msg_sl_t*)x)->ref_count) & AK_MSG_REF_COUNT_MASK)
#define get_fatal_msg_type(x)			((((ak_msg_sl_t*)x)->ref_count) & AK_MSG_TYPE_MASK)

typedef struct {
	/* time of message handler */
	uint32_t	start_post;
	uint32_t	start_exe;
	uint32_t	stop_exe;
} dbg_handler_t;

typedef struct {
	/* debug */
	dbg_handler_t dbg_handler;

	/* task header */
	uint32_t src_task_id;
	uint32_t des_task_id;
	uint32_t sig;

	/* external task header */
	uint32_t if_src_task_id;
	uint32_t if_des_task_id;
	uint32_t if_src_type;
	uint32_t if_des_type;
	uint32_t if_sig;

	/* message type (pool type)*/
	uint32_t type;

	/* payload */
	uint32_t len;
	void* payload;
} header_t;

typedef struct ak_msg_t{
	ak_msg_t* next;
	ak_msg_t* prev;
	header_t* header;
} ak_msg_t;

typedef struct {
	pthread_mutex_t mt;
	uint32_t len;
	ak_msg_t* head;
	ak_msg_t* tail;
} q_msg_t;

extern void q_msg_init(q_msg_t* q_msg);

extern void q_msg_put(q_msg_t* q_msg, ak_msg_t* msg);
extern ak_msg_t* q_msg_get(q_msg_t* q_msg);
extern void q_msg_free(ak_msg_t* msg);

extern uint32_t q_msg_len(q_msg_t* q_msg);
extern bool q_msg_available(q_msg_t* q_msg);

#endif // __MESSAGE_H__
