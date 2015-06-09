#ifndef FS_H
#define FS_H

#include "pack/list.h"


char * fs_read_new(const char *path, int *len);
char * fs_read_str_new(const char *path);
void fs_write(
		const char *path,
	   	const char *data,
	   	unsigned int len);
void *fs_write_str(
		const char *path,
		const char *str);


int fs_exist(const char *path);
int fs_is_dir(const char *path); 
int fs_mkdirs(const char *dir);
int fs_rmdir(const char *dir);
int fs_mkdir(const char *dir);
long long fs_mtime_(const char *file_path);
int fs_rmdir(const char *path);
int fs_rmfile(const char *path);
int fs_copy_file(const char *from, const char *to);

// list files and dirs in dir
// @depth 
// @with_dir 是否将遍历到的目录加到结果列表中 0 不添加， -1先加到列表中，
// 	1 后于子目录子文件加到列表
// @return list of files and dirs. element type is char *
// need to free list element, call : list_fini(&return, free);
struct list fs_list_dir(const char *dir, int depth, int with_dir);

const char * path_name_(const char *path);
const char * path_ext_(const char *path);
char * path_base_name_new_(const char *path);


#endif
