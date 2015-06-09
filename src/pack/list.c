#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "list.h"


static struct list_ele * list_ele_new(size_t size, void *data){
	struct list_ele *ins = (struct list_ele *)calloc(1, sizeof(struct list_ele) + size);
	if (size == 0) {
		ins->data = data;
	} else {
		ins->data = ((char *)ins) + sizeof(struct list_ele);//&ins->data + 1;
		memmove(ins->data, data, size);
	}
	return ins;
}
static void * list_ele_free(struct list_ele *ins, size_t size, void (*free_item)(void *data)){
	void *ret = ins->data;
	if (size == 0 && free_item) {
		free_item(ins->data);	
		ret = 0;
	}

	free(ins);
	return ret;
}
static struct list_ele ** get_ptr_to_next_at(struct list *me, int at){
	struct list_ele **cur = &me->head;
	int i = 0;

	while (*cur && i++ < at) {
		cur = &(*cur)->next;
	}
	return cur;
}
static struct list_ele * get_prev_at(struct list *me, int at){
	if (at == 0) return NULL;

	struct list_ele *cur = me->head;
	int i = 0;

	while (cur && ++i < at) {
		cur = cur->next;
	}
	return cur;
}
struct list * list_new(size_t size){
	struct list *me = (struct list *)malloc(sizeof(struct list));
	list_init(me, size);
	return me;
}
void list_free(struct list *me, void (*free_item)(void *data)) {
	list_fini(me, free_item);	
	free(me);
}
void list_init(struct list *me, size_t size){
	me->size = size;
	me->head = 0;
	me->tail = 0;
	me->len = 0;

	list_reset_each(me);
}
void list_fini(struct list *me, void (*free_item)(void *data)){
	list_del_all(me, free_item);
}
int list_len(struct list *me){
	return me->len;
}
void list_add(struct list *me, void *data){
	list_add_at(me, data, me->len);
}
void list_add_at(struct list *me, void *data, int at){
	struct list_ele *ins = list_ele_new(me->size, data);
	struct list_ele *prev = get_prev_at(me, at);

	if (prev) {
		ins->prev = prev;
		ins->next = prev->next;
		if (prev->next) prev->next->prev = ins;
		prev->next = ins;
	} else {
		if (me->head) me->head->prev = ins;

		ins->prev = NULL;
		ins->next = me->head;
		me->head = ins;
	}
	if (!ins->next) me->tail = ins;

	me->len++;
}

int list_find(struct list *me, void *data) {
	int i = 0;
	for (void *cur = NULL; cur = list_each(me, cur); i++) {
		if (cur == data) return i;
	}
	return -1;
}
void * list_set(struct list *me, void *data, int at, void (*free_item)(void *data)){
	struct list_ele **ptr_to = get_ptr_to_next_at(me, at);
	struct list_ele *found = *ptr_to;
	
	if (!found) return 0;
	void *old = found->data;

	if (me->size == 0 && free_item) {
		free_item(old);
		old = 0;
	}
	if (me->size == 0) {
		found->data = data;
	} else {
		memmove(found->data, data, me->size);
	}

	return old;
}
void * list_get(struct list *me, int at){
	if (at == me->len) {
		return me->tail;
	}

	struct list_ele **ptr_to = get_ptr_to_next_at(me, at);
	struct list_ele *found = *ptr_to;

	return found ? found->data : 0;
}
void * list_del(struct list *me, int at, void (*free_item)(void *data)){
	struct list_ele **ptr_to = get_ptr_to_next_at(me, at);
	struct list_ele *found = *ptr_to;
	
	if (!found) return 0;

	void *old = found->data;

	if (me->size == 0 && free_item) {
		free_item(old);
		old = 0;
	}

	*ptr_to = found->next;
	if (found->next) {
		found->next->prev = found->prev;
	} else {
		me->tail = found->prev;
	}
	
	if (found->prev) {
		found->prev->next = found->next;
	}

	free(found);

	me->len--;

	return old;
}
void list_del_all(struct list *me, void (*free_item)(void *data)){
	struct list_ele *cur = me->head;
	struct list_ele *tmp = 0;

	while (cur) {
		if (me->size == 0 && free_item) {
			free_item(cur->data);
		}
		tmp = cur;
		cur = cur->next;

		free(tmp);
	}
	me->len = 0;
	me->head = 0;
	me->tail = 0;
	list_reset_each(me);
}
void list_reset_each(struct list *me) {
	me->cur = 0;
}
void * list_each(struct list *me, void *cur) {
	if (!cur) list_reset_each(me);

	if (!me->cur) me->cur = me->head;
	else me->cur = me->cur->next;

	void *ret = !me->cur ? NULL : me->cur->data;
	
	if (ret == NULL) {
		list_reset_each(me);
	}

	return ret;
}
void list_del_each(struct list *me, void (*free_item)(void *data)) {
	if (!me->cur) return;

	struct list_ele *will_free = me->cur;
	if (me->size == 0 && free_item) {
		free_item(will_free->data);
	}

	if (will_free->prev) {
		will_free->prev->next = will_free->next;
		//me->cur back
		me->cur = will_free->prev;
	} else {
		me->head = will_free->next;
		//me->cur back
		me->cur = NULL;
	}

	if (will_free->next) {
		will_free->next->prev = will_free->prev;
	} else {
		me->tail = will_free->prev;
	}

	free(will_free);
	me->len--;
}
struct list_ele * list_get_ele(struct list *me, int at) {
	struct list_ele **ptr_to = get_ptr_to_next_at(me, at);
	struct list_ele *found = *ptr_to;

	return found;
}
struct list_ele * list_ele_prev(struct list_ele *cur) {
	if (!cur) return 0;
	return cur->prev;
}
struct list_ele * list_ele_next(struct list_ele *cur) {
	if (!cur) return 0;
	return cur->next;
}
void * list_ele_data(struct list_ele *cur) {
	if (!cur) return 0;
	return cur->data;
}
void list_detach_ele(struct list *me, struct list_ele *cur) {
	if (cur->next) {
		cur->next->prev = cur->prev;
	} else {
		me->tail = cur->prev;
	}
	if (cur->prev) {
		cur->prev->next = cur->next;
	} else  {
		me->head = cur->next;
	}
}
void list_attach_ele(struct list *me, struct list_ele *cur, int at) {
	struct list_ele **ptr_to = get_ptr_to_next_at(me, at);
	struct list_ele *next = *ptr_to;

	cur->prev = 0;
	cur->next = 0;

	cur->next = next;
	if (next) {
		cur->prev = next->prev;
		next->prev = cur;
	} else if (me->tail) {
		cur->prev = me->tail;
		me->tail = cur;
	}
	*ptr_to = cur;
}
