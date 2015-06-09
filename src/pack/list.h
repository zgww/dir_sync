#ifndef LIST_H
#define LIST_H

struct list_ele
{
	struct list_ele *next;
	struct list_ele *prev;

	void *data;
};
struct list
{
	struct list_ele *head;
	struct list_ele *tail;

	struct list_ele *cur;
	int len;
	size_t size;
};

/**

size : 0 代表存的是指针
*/
struct list * list_new(size_t size); 
void list_free(struct list *me, void (*free_item)(void *data));
void list_init(struct list *me, size_t size);
void list_fini(struct list *me, void (*free_item)(void *data));

int list_len(struct list *me);
void list_add(struct list *me, void *data);
void list_add_at(struct list *me, void *data, int at);
/**
@return item_size == 0 && free_item == 0 => 返回旧值
	else 返回0
*/
void * list_set(struct list *me, void *data, int at, void (*free_item)(void *data));
void * list_del(struct list *me, int at, void (*free_item)(void *data));
void list_del_all(struct list *me, void (*free_item)(void *data));
void * list_get(struct list *me, int at);

struct list_ele * list_get_ele(struct list *me, int at);
void list_detach_ele(struct list *me, struct list_ele *cur);
void list_attach_ele(struct list *me, struct list_ele *cur, int at);

struct list_ele * list_ele_prev(struct list_ele *cur);
struct list_ele * list_ele_next(struct list_ele *cur);
void * list_ele_data(struct list_ele *cur);

void list_reset_each(struct list *me);
void * list_each(struct list *me, void *cur);
void list_del_each(struct list *me, void (*free_item)(void *data));


int list_find(struct list *me, void *data);

#endif
