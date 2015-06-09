#include <stdlib.h>
#include <string.h>
#include "fs.h"


const char * path_ext_(const char *path) {
	const char *name = path_name_(path);

	const char *last_dot = strrchr(name, '.');

	return last_dot;
}

const char * path_name_(const char *path) {
	const char * last_slant = strrchr(path, '/');
	const char * last_back_slant = strrchr(path, '\\');
	//没有斜杠反斜杠
	//代表没有父目录
	//所以直接返回
	if (!last_slant && !last_back_slant) return path;

	//TODO QUESTION 假如其中一个为NULL,还可以比较吗
	if (last_back_slant > last_slant) last_slant = last_back_slant;

	return last_slant + 1;
}
char * path_base_name_new_(const char *path) {
	const char *name = path_name_(path);
	char *dup = strdup(name);
	char *ext = (char *)path_ext_(dup);
	*ext = '\0';
	return dup;
}

