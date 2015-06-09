#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include "arr.h"

#define MIN_CAPACITY 16

static int expand(struct arr *me) {
	int next_cap = me->capacity * 2;

	size_t size = next_cap * (me->size == 0 ? sizeof(void *) : me->size);
	char *new_buf = realloc(me->buf, size);

	if (!new_buf) {
		perror("realloc failed \n");
		return -1;
	}

	me->buf = new_buf;
	me->capacity = next_cap;
	return 0;
}

struct arr * arr_new(int cap, size_t size) {
	struct arr *ins = (struct arr *)malloc(sizeof(struct arr));

	arr_init(ins, cap, size);	

	return ins;
}
void arr_free(struct arr *me, void (*free_item)(void *data)){
	arr_fini(me, free_item);	
	free(me);
}
void arr_init(struct arr *me, int cap, size_t size) {
	me->capacity = cap < MIN_CAPACITY ? MIN_CAPACITY : cap;
	me->size = size;
	me->len = 0;

	size_t actual_size = size == 0 ? sizeof(void *) : size;

	me->buf = calloc(me->capacity, actual_size);

	arr_reset_each(me);
}
void arr_fini(struct arr *me, void (*free_item)(void *data)) {
	arr_del_all(me, free_item);
	free(me->buf);
}

int arr_len(struct arr *me) {
	return me->len;
}
void arr_add(struct arr *me, void *data) {
	arr_add_at(me, data, me->len);
}
void arr_add_at(struct arr *me, void *data, int at) {
	if (me->len >= me->capacity && expand(me) == -1) {
		printf("aray_expand failed\n");
		return;
	}
	size_t size = me->size ? me->size : sizeof(void *);

	if (at > me->len) at = me->len;

	void *from = (char *)me->buf + at * size;
	void *to = (char *)me->buf + (at + 1) * size;
	if (at < me->len) {
		memmove(to, from, (me->len - at) * size);
	}
	memcpy(from, me->size ? data : &data, size);

	me->len++;
}
/**
@return item_size == 0 && free_item == 0 => 返回旧值
	else 返回0
*/
void * arr_set(struct arr *me, void *data, int at, void (*free_item)(void *data)) {
	if (at >= me->len) return 0;

	void *old = 0;
	if (me->size == 0) {
		old = ((void **)me->buf)[at];
		if (free_item) {
			free_item(old);
			old = 0;
		}
		((void **)me->buf)[at] = data;
	} else {
		memcpy(me->buf + at * me->size, data, me->size);
	}

	return old;
}
void * arr_del(struct arr *me, int at, void (*free_item)(void *data)) {
	if (at >= me->len) return 0;

	void *old = 0;
	if (me->size == 0) {
		old = ((void **)me->buf)[at];
		if (free_item) {
			free_item(old);
			old = 0;
		}
	}
	size_t actual_size = me->size == 0 ? sizeof(void *) : 0;
	memcpy(me->buf + at * actual_size, me->buf + (at + 1) * actual_size, actual_size);
	me->len--;

	return old;
}
void arr_del_all(struct arr *me, void (*free_item)(void *data)) {
	if (me->size == 0) {
		int i;
		for (i = 0; i < me->len; i++) {
			void *old = ((void **)me->buf)[i];
			if (free_item) 
				free_item(old);
		}

	} else {
		memset(me->buf, 0, (me->size == 0 ? sizeof(void *) : me->size) * me->len);
	}
	me->len = 0;
}
void * arr_get(struct arr *me, int at) {
	if (at > me->len) return 0;

	if (me->size == 0) {
		return ((void **)me->buf)[at];
	}
	return me->buf + at * me->size;
}

void arr_reset_each(struct arr *me) {
	me->cur_at = 0;
}
void * arr_each(struct arr *me, void *cur) {
	if (!cur) arr_reset_each(me);

	if (me->cur_at >= me->len) return 0;

	void *data = arr_get(me, me->cur_at);

	me->cur_at++;

	if (data == NULL) {
		arr_reset_each(me);
	}
	return data;
}
void arr_del_each(struct arr *me, void (*free_item)(void *data)) {
	arr_del(me, --me->cur_at, free_item);
//	me->cur_at--;
}

int arr_is_full(struct arr *me){
	return me->len == me->capacity;
}
