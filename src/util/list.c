#include <stdlib.h>
#include <string.h>

typedef struct WgvkListNode {
	struct WgvkListNode *prev;
	struct WgvkListNode *next;
	void *data;
} WgvkListNode;

typedef struct {
	WgvkListNode head;
	WgvkListNode tail;
	size_t size;
} WgvkList;

void wgvk_list_init(WgvkList *list) {
	list->head.prev = NULL;
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
	list->tail.next = NULL;
	list->size = 0;
}

void wgvk_list_push_back(WgvkList *list, void *data) {
	WgvkListNode *node = malloc(sizeof(WgvkListNode));
	if (!node)
		return;

	node->data = data;
	node->next = &list->tail;
	node->prev = list->tail.prev;
	list->tail.prev->next = node;
	list->tail.prev = node;
	list->size++;
}

void *wgvk_list_pop_front(WgvkList *list) {
	if (list->size == 0)
		return NULL;

	WgvkListNode *node = list->head.next;
	void *data = node->data;

	list->head.next = node->next;
	node->next->prev = &list->head;
	free(node);
	list->size--;

	return data;
}

size_t wgvk_list_size(WgvkList *list) {
	return list->size;
}
