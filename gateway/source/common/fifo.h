#ifndef __FIFO_H__
#define __FIFO_H__
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#define RET_FIFO_OK				(0x01)
#define RET_FIFO_NG				(0x00)

typedef struct {
	uint8_t tail_index;
	uint8_t head_index;
	uint8_t fill_size;
	uint8_t buffer_size;
	uint8_t element_size;
	uint8_t* buffer;
} fifo_t;

extern void		fifo_init(fifo_t* fifo, void* buffer, uint8_t buffer_size, uint8_t element_size);
extern uint8_t	fifo_availble(fifo_t* fifo);
extern bool		fifo_is_empty(fifo_t* fifo);
extern uint8_t	fifo_put(fifo_t* fifo, void* data);
extern uint8_t	fifo_get(fifo_t* fifo, void* data);

#ifdef __cplusplus
}
#endif

#endif //__FIFO_H__
