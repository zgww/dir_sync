#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "dict.h"

#define MIN_CAPACITY 16
#define FACTOR 0.75

/**

0 => keg1 => keg2 => keg3 => keg4
1 => keg1 => keg2 => keg3 => keg4
2 => keg1 => keg2 => keg3 => keg4
3 => keg1 => keg2 => keg3 => keg4
4 => keg1 => keg2 => keg3 => keg4
5 => keg1 => keg2 => keg3 => keg4
6 => keg1 => keg2 => keg3 => keg4
7 => keg1 => keg2 => keg3 => keg4

keg => {
	data : ptr | data
}

*/

static unsigned int hash(const char *str) {
	unsigned int h = 0;
	char c;
	while ((c = *str++)) h = h * 131 + c;
	return h;
}
static void expand(struct dict *me) {
	int cap = me->capacity * 2;
	struct dict_keg **buf = (struct dict_keg **)calloc(cap, sizeof(struct dict_keg *));
	struct dict_keg *cur;
	struct dict_keg **to;
	int i;
	int h;
	for (i = 0; i < me->capacity; i++) {
		cur = me->buf[i];
		while (cur) {
			h = hash(cur->name) % cap;
			to = buf + h;
			while (*to) 
				to = &((*to)->next);
			*to = cur;
			cur = cur->next;	
			(*to)->next = 0;
		}
	}
	free(me->buf);
	me->buf = buf;
	me->capacity = cap;
}
// 得到指向NAME对应KEG的指针的指针
static struct dict_keg ** at(struct dict *me, const char *name){
	int pos = hash(name) % me->capacity;
	struct dict_keg **cur = me->buf + pos;
	
	while (*cur) {
		if (strcmp((*cur)->name, name) == 0) break;	
		cur = &(*cur)->next;
	}

	return cur;
}
struct dict * dict_new(int capacity, size_t size){
	struct dict *me = (struct dict *)calloc(1, sizeof(struct dict));

	dict_init(me, capacity, size);

	return me;
}
void dict_free(struct dict *me, void (*free_item)(void *data)) {
	dict_fini(me, free_item);

	free(me);
}
void dict_init(struct dict *me, int capacity, size_t size) {
	me->size = size;
	me->capacity = capacity < MIN_CAPACITY ? MIN_CAPACITY : capacity;
	me->len = 0;

	me->buf = (struct dict_keg **)calloc(me->capacity, sizeof(struct dict_keg *));
}
void dict_fini(struct dict *me, void (*free_item)(void *data)){
	dict_del_all(me, free_item);
}

void * dict_put(struct dict *me, const char *name, void *data, void (*free_item)(void *data)){
	struct dict_keg **ptr_next = at(me, name); 

	if (strlen(name) >= DICT_MAX_NAME_LEN) {
		printf("dict name must <= %d", DICT_MAX_NAME_LEN);
		return 0;
	}

	void *old = NULL;

	if (*ptr_next == NULL) {
		*ptr_next = (struct dict_keg *)calloc(1, sizeof(struct dict_keg) + me->size);
		if (!*ptr_next)  {
			perror("dict_put ptr_ext is nil");
			exit(-1);
		}
		strcpy((*ptr_next)->name, name);
		me->len++;
	} else {
		old =  (*ptr_next)->data;

		if (free_item) {
			free_item(old);
			old = NULL;
		}
	}

	if (me->size) {
		struct dict_keg *keg = *ptr_next;
		keg->data = ((char *)keg) + sizeof(struct dict_keg);
		memmove(keg->data, data, me->size);
	} else {
		(*ptr_next)->data = data;
	}

	if (me->capacity * FACTOR < me->len){
	   	expand(me);
	}

	return old;
}
void * dict_get(struct dict *me, const char * name) {
	struct dict_keg **ptr_next = at(me, name);
	void *ret = *ptr_next ? (*ptr_next)->data : NULL;
	return ret;
}
void * dict_del(struct dict *me, const char *name, void (*free_item)(void *data)){
	struct dict_keg **ptr_next = at(me, name);

	if ((*ptr_next) == 0) return 0;

	void *old = (*ptr_next)->data;

	if (me->size && free_item) {
		free_item(old);
		old = 0;
	}
	*ptr_next = (*ptr_next)->next;

	me->len--;

	return old;
}
void dict_del_all(struct dict *me, void (*free_item)(void *data)) {
	int i;

	for (i = 0; i < me->capacity; i++) {
		struct dict_keg *cur = me->buf[i];

		while (cur) {
			struct dict_keg *tmp = cur;

			if (me->size && free_item) {
				free_item(tmp->data);
			}
			free(tmp);

			cur = cur->next;
		}
	}
}
void dict_reset_each(struct dict *me) {
	me->cur_index = 0;
	me->cur = me->buf + me->cur_index;
	me->cur_name = 0;
}
void * dict_each(struct dict *me, char **out_name, void *prev_ret) {
	if (!prev_ret) {
		dict_reset_each(me);
	}

	void *ret = NULL;
	while (1) {
		if (!(*me->cur)) {
			me->cur_index++;
			me->cur = me->buf + me->cur_index;
		} else break;
		if (me->cur_index >= me->capacity) goto ret;
	}
	struct dict_keg *tmp = *me->cur;
	me->cur = &tmp->next;

	me->cur_name = tmp->name;

	*out_name = tmp->name;
	ret = tmp->data;
ret :
	return ret;
}
void dict_del_each(struct dict *me, void (*free_item)(void *data)){
	dict_del(me, me->cur_name, free_item);
}

int dict_len(struct dict *me){
	return me->len;
}
