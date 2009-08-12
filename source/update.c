#include "update.h"
#include "fat.h"
#include "config.h"

s32 Update_Fetch() {

	s32 ret = -1;

	char host[64];
	strcpy(host, UPDATE_HOST);
	char path[100];
	strcpy(path, UPDATE_DOL_PATH);
	char target[100];
	strcpy(target, DOL_FILE);

	struct block update_file;
	
	update_file = Net_GetFile(host, path);
	if (update_file.size > 0) {
		ret = Fat_WriteFile(target, update_file);
		if (ret != -1)  
			ret = 0;
	}
	
	strcpy(path, UPDATE_XML_PATH);
	strcpy(target, XML_FILE);
	
	update_file = Net_GetFile(host, path);
	if (update_file.size > 0) {
		ret = Fat_WriteFile(target, update_file);
		if (ret != -1)
			ret = 0;
	}
	
	strcpy(path, UPDATE_PNG_PATH);
	strcpy(target, PNG_FILE);
	update_file = Net_GetFile(host, path);
	if (update_file.size > 0) {
		ret = Fat_WriteFile(target, update_file);
		if (ret != -1)
			ret = 0;
	}
	
	return ret;
}

void Update_CheckVersion(struct block version)
{
	char host[64];
	strcpy(host, UPDATE_HOST);
	char ver_path[100];
	strcpy(ver_path, UPDATE_VER_PATH);
	
	struct block ver_file;
	
	ver_file = Net_GetFile(host, ver_path);
	if (ver_file.size > 0)
		memcpy(version.data, ver_file.data, ver_file.size);
	free(ver_file.data);
}
