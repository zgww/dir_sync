#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <direct.h>
#include <io.h>
#include "../fs.h"

char * fs_read_new(const char *path, int *olen) {
	char *data = NULL;
	int len;
	FILE *f = fopen(path, "rb+");

	if (!f) return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = malloc(len * sizeof(char));
	fread(data, sizeof(char), len, f);

	fclose(f);

	if (olen) *olen = len;
	return data;
}

char * fs_read_str_new(const char *path) {
	char *data = NULL;
	int len;
	FILE *f = fopen(path, "rb+");

	if (!f) return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = malloc((len + 1) * sizeof(char));
	fread(data, sizeof(char), len, f);
	data[len] = '\0';

	fclose(f);

	return data;
}

void fs_write(
		const char *path,
	   	const char *data,
	   	unsigned int len) {

	FILE *f = fopen(path, "w+");
	assert(f);

	fwrite(data, sizeof(char), len, f);
	fclose(f);
}

void *fs_write_str(
		const char *path,
		const char *str) {

	FILE *f = fopen(path, "w+");
	assert(f);

	fwrite(str, sizeof(char), strlen(str), f);
	fclose(f);
}
int fs_exist(const char *path) {
	return access(path, 0) != -1;
}
int fs_is_dir(const char *path) {
	return (GetFileAttributes(path) 
		& FILE_ATTRIBUTE_DIRECTORY) != 0;
}

int fs_rmfile(const char *path) {
	if (!fs_exist(path)) return 0;

	return unlink(path);
}
int fs_rmdir(const char *path) {
	if (!fs_exist(path)) return 0;


	char dir[MAX_PATH];
	char file[MAX_PATH];
	WIN32_FIND_DATA file_data;
	sprintf(dir, "%s//*.*", path);
	HANDLE find = FindFirstFile(dir, &file_data);
	
	if (INVALID_HANDLE_VALUE == find) return -1;

	while (1) {
		sprintf(file, "%s/%s", path, file_data.cFileName);
		if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if(*file_data.cFileName != '.')  {  
				if (fs_rmdir(file) == -1) return -1;
			}
		} else {
			if (unlink(file) == -1) return -1;
		}

		if(!FindNextFile(find, &file_data))  break;  
	}
	FindClose(find);   

	const char *name = path_name_(path);
	if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0) {
		int ret = _rmdir(path);
		//int ret = fs_exist(path) ? -1 : 0;

		if (ret == -1) perror("remove dir failed\n");

		return ret;
	}
	return 0;
}
/**
 * @depth != 0 then recu depth list dir , -1 recu list all
 */
void _list_dir(const char *path, int depth, int with_dir, struct list *list)  {  
	char dir[MAX_PATH];  
	char file[MAX_PATH];  

	WIN32_FIND_DATA file_data;  

	sprintf(dir, "%s//*.*", path);

	HANDLE find = FindFirstFile(dir, &file_data);  

	if(INVALID_HANDLE_VALUE == find) return;  

	while(TRUE)  {  
		sprintf(file, "%s/%s", path, file_data.cFileName);
		if(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  {  
			if(depth != 0 && *file_data.cFileName != '.')  {  

				if (with_dir < 0) 
					list_add(list, strdup(file));

				_list_dir(file, depth > 0 ? depth - 1 : depth, 
						with_dir, list);  

				if (with_dir > 0) 
					list_add(list, strdup(file));
			}  
		} else {        
			list_add(list, strdup(file));
		}  

		if(!FindNextFile(find, &file_data))  
			break;  
	}  
	FindClose(find);   
} 
struct list fs_list_dir(const char *dir, int depth, int with_dir) {
	struct list list;
	list_init(&list, 0);
	_list_dir(dir, depth, with_dir, &list);
	return list;
}

static long long filetime_to_ms(FILETIME ft) {
	long long ms;

	ULARGE_INTEGER ui;  
	ui.LowPart = ft.dwLowDateTime;  
	ui.HighPart = ft.dwHighDateTime;  

	ms = ((long long)(ui.QuadPart - 116444736000000000) / 10000L);  
	return ms;
}
long long fs_mtime_(const char *file_path){
	WIN32_FIND_DATA ffd ;
	HANDLE find = FindFirstFile(file_path, &ffd);

	if (INVALID_HANDLE_VALUE == find) return -1L;

	// ft 单位ns
	long long ms = filetime_to_ms(ffd.ftLastWriteTime);

	FindClose(find);

	return ms;
}
int fs_mkdir(const char *dir){
	if (fs_exist(dir)) return 0;
	return _mkdir(dir);
}
int fs_mkdirs(const char *dir){
	char path[MAX_PATH];
	strcpy(path, dir);
	const char *colon = strchr(path, ':');
	int start = 0;
	// e:///
	if (colon) {
		start = colon - path;
		while (path[++start] == '/' || path[start] == '\\')
			;
	}

	//到达下一个/, \, \0 即到达下一个目录
	while (1) {
		while (path[++start] != '/' && path[start] != '\\' && path[start] != '\0') 
			;
		char tmp = path[start];
		path[start] = '\0';

		// 0 代表创建成功的。非代表二元的函数名，返回0都代表成功
		if (!fs_exist(path) && fs_mkdir(path)) return -1;

		path[start] = tmp;

		if (tmp == '\0') break;
	}
	return 0;
}
int fs_copy_file(const char *from, const char *to){
	int ret = 0;
	assert(strcmp(from, to));

	FILE *f = fopen(from, "rb");
	FILE *t = fopen(to, "wb+");
	if (!f || !t) {
		ret = -1;
		goto RETURN;
	}

	char buf[1024];
	int w_cnt;
	int cnt;
	while ((cnt = fread(buf, 1, 1024, f)) > 0) {
		if ((w_cnt = fwrite(buf, 1, cnt, t)) != cnt) {
			ret = -1;
			goto RETURN;
		}
	}

RETURN : 
	if (f) fclose(f);
	if (t) fclose(t);
	return ret;
}
