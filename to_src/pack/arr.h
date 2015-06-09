#ifndef ARR_H
#define ARR_H

#include <stdlib.h>

struct arr {
	int capacity;
	int len;

	void *buf;

	size_t size;
	// for each
	int cur_at;
};



/**

size : 0 代表存的是指针
*/
struct arr * arr_new(int cap, size_t size); 
void arr_free(struct arr *me, void (*free_item)(void *data));
void arr_init(struct arr *me, int cap, size_t size);
void arr_fini(struct arr *me, void (*free_item)(void *data));

int arr_is_full(struct arr *me);
int arr_len(struct arr *me);
void arr_add(struct arr *me, void *data);
void arr_add_at(struct arr *me, void *data, int at);
/**
@return item_size == 0 && free_item == 0 => 返回旧值
	else 返回0
*/
void * arr_set(struct arr *me, void *data, int at, void (*free_item)(void *data));
void * arr_del(struct arr *me, int at, void (*free_item)(void *data));
void arr_del_all(struct arr *me, void (*free_item)(void *data));
void * arr_get(struct arr *me, int at);

void arr_reset_each(struct arr *me);
void * arr_each(struct arr *me, void *cur);
void arr_del_each(struct arr *me, void (*free_item)(void *data));



#endif
