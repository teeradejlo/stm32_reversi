#include "stm32f10x.h"
#include "global.H"

//Implementation of Queue
Queue* createQueue(u8 cap) {
	Queue* temp = (Queue*)malloc(sizeof(Queue));
	temp->capacity = cap;
	temp->front = temp->size = 0;
	temp->rear = cap - 1;
	temp->array = (u8*)malloc(temp->capacity * sizeof(u8));
	return temp;
}

u8 queue_isFull (Queue* q) {
	return (q->size == q->capacity);
}

u8 queue_isEmpty (Queue* q) {
	return (q->size == 0);
}

void enqueue(Queue* q, u8 item) {
	if (queue_isFull(q)) return;

	q->rear = (q->rear+1) % q->capacity;
	q->array[q->rear] = item;
	q->size = q->size + 1;
}

u8 dequeue(Queue* q) {
	u8 item;

	if (queue_isEmpty(q)) return GBOARD_SIZE;
	item = q->array[q->front];
	q->front = (q->front+1) % q->capacity;
	q->size = q->size - 1;
	return item;
}

u8 queue_front(Queue* q) {
	if (queue_isEmpty(q)) return GBOARD_SIZE;
	return q->array[q->front];
}

u8 queue_rear(Queue* q) {
	if (queue_isEmpty(q)) return GBOARD_SIZE;
	return q->array[q->rear];
}

u8 queue_getItem(Queue* q, u8 item_no) {
	if (queue_isEmpty(q)) return GBOARD_SIZE;
	if (item_no >= q->size) return GBOARD_SIZE;

	return q->array[(q->front + item_no) % q->capacity];
}

void queue_free(Queue** q) {
	if (q == NULL) return;
	if (*q == NULL) return;

	free((*q)->array);
	free((*q));
	*q = NULL;
}
//end Implementation of Queue
