#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#define DICT_MAX_NAME_LEN 128

#include <stdlib.h>

struct dict_keg {
	struct dict_keg *next;
	char name[DICT_MAX_NAME_LEN];
	void *data;
};
typedef struct dict {
	int capacity;
	int len;
	struct dict_keg **buf;

	size_t size;

	// for dict_next
	int cur_index;
	struct dict_keg **cur;
	char *cur_name;
} dict;
struct dict * dict_new(int capacity, size_t size);
void dict_free(struct dict *me, void (*free_item)(void *data));
void dict_init(struct dict *me, int capacity, size_t size);
void dict_fini(struct dict *me, void (*free_item)(void *data));

int dict_len(struct dict *me);

void * dict_put(struct dict *me, const char *name, void *data, void (*free_item)(void *data));
void * dict_get(struct dict *me, const char *name);
void * dict_del(struct dict *me, const char *name, void (*free_item)(void *data));

void dict_del_all(struct dict *me, void (*free_item)(void *data));

void dict_reset_each(struct dict *me);
void * dict_each(struct dict *me, char **out_name, void *ret);
void dict_del_each(struct dict *me, void (*free_item)(void *data));

#endif
