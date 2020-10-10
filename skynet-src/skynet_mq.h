#ifndef SKYNET_MESSAGE_QUEUE_H
#define SKYNET_MESSAGE_QUEUE_H

#include <stdlib.h>
#include <stdint.h>

struct skynet_message {
    //消息发送的原地址
	uint32_t source;
    //skynet采用请求回应的方式。当一个服务向另一个服务发送请求时，会生成一个 session。当响应端处理结束后会将结果返回，此时请求端可以根据
    //session 字段找到处理完毕的结果。
    //当 lua 层调用 call 时，push一个消息，并且生成一个session，然后将本地的协程挂起，挂起时，会以session为key，协程句  
    // 柄为值，放入一个table中，当回应消息送达时，通过session找到对应的协程，并将其唤醒。
	int session;
	void * data;
	size_t sz;
};

// type is encoding in skynet_message.sz high 8bit
#define MESSAGE_TYPE_MASK (SIZE_MAX >> 8)
#define MESSAGE_TYPE_SHIFT ((sizeof(size_t)-1) * 8)

struct message_queue;

void skynet_globalmq_push(struct message_queue * queue);
struct message_queue * skynet_globalmq_pop(void);

struct message_queue * skynet_mq_create(uint32_t handle);
void skynet_mq_mark_release(struct message_queue *q);

typedef void (*message_drop)(struct skynet_message *, void *);

void skynet_mq_release(struct message_queue *q, message_drop drop_func, void *ud);
uint32_t skynet_mq_handle(struct message_queue *);

// 0 for success
int skynet_mq_pop(struct message_queue *q, struct skynet_message *message);
void skynet_mq_push(struct message_queue *q, struct skynet_message *message);

// return the length of message queue, for debug
int skynet_mq_length(struct message_queue *q);
int skynet_mq_overload(struct message_queue *q);

void skynet_mq_init();

#endif
