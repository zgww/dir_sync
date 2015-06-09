#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "fs.h"
#include "pack/list.h"

struct file_info {
	char *file;
	long long mtime;// ms
	int is_dir;
};

static void file_info_fini(struct file_info *ins) {
	free(ins->file);
}

static struct list src_files;
static struct list des_files;

static void load_files_info(const char *path, struct list *out){
	struct file_info file_info;
	struct list files = fs_list_dir(path, -1, -1);
	for (char *cur = NULL; cur = list_each(&files, cur);) {
		file_info.file = cur;
		file_info.mtime = fs_mtime_(cur);
		file_info.is_dir = fs_is_dir(cur);
		list_add(out, &file_info);
	}
	list_fini(&files, NULL);
}
static struct file_info * get_rel_file_info(
		const char *src,
		const char *des, 
		const char *path, 
		struct list *list) {
	const char *from = path + strlen(src);

	for (struct file_info *cur = NULL; cur = list_each(list, cur);) {
		const char *to = cur->file + strlen(des);
		if (strcmp(from, to) == 0) return cur;
	}
	return NULL;
}

int main(int argc, char **argv){
	assert(argc == 3);
	char *src = argv[1];
	char *des = argv[2];
	char file[1024];

	if (!fs_is_dir(src)) {
		fs_copy_file(src, des);
		return 0;
	}

	list_init(&src_files, sizeof(struct file_info));
	list_init(&des_files, sizeof(struct file_info));

	load_files_info(src, &src_files);
	load_files_info(des, &des_files);

	fs_mkdirs(des);

	for (struct file_info *cur = NULL; cur = list_each(&src_files, cur);){
		struct file_info *f = get_rel_file_info(src, des, cur->file, &des_files);
		sprintf(file, "%s/%s", des, cur->file + strlen(src));
		if (!f) { // copy to des
			if (cur->is_dir) {
				printf("mkdir : %s\n", file);
				fs_mkdirs(file);
			} else {
				printf("copyfile : %s => %s\n", cur->file, file);
				fs_copy_file(cur->file, file);
			}
		} else if (cur->mtime > f->mtime) { // update to des
			if (!cur->is_dir) {
				printf("copyfile : %s => %s\n", cur->file, file);
				fs_copy_file(cur->file, file);
			}
		}
	}
	for (struct file_info *cur = NULL; cur = list_each(&des_files, cur);){
		struct file_info *f = get_rel_file_info(des, src, cur->file, &src_files);
		if (!f) { // will delete des
			if (cur->is_dir) {
				printf("rmdir : %s\n", cur->file);
				fs_rmdir(cur->file);
			} else {
				printf("rmfile : %s\n", cur->file);
				fs_rmfile(cur->file);
			}
		}
	}
	list_fini(&src_files, (void (*)(void *))file_info_fini);
	list_fini(&des_files, (void (*)(void *))file_info_fini);

	return 0;
}
