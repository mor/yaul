#include "update.h"
#include "fat.h"
#include "config.h"
#include "md4.h"

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

	char resbuf[17];
	memset(resbuf, 0, 17);
	md4_buffer(update_file.data, update_file.size, resbuf);
	//char * hexed = my_sum(update_file);
	//printf("    recvd file md4: %s\n", hexed);

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

struct block Update_CheckVersion(void)
{
	char host[64];
	strcpy(host, UPDATE_HOST);
	char ver_path[100];
	strcpy(ver_path, UPDATE_VER_PATH);
	
	struct block version;
	version = Net_GetFile(host, ver_path);
	return version;
}

void hexify(char * in_str, char * out_str) {
	const char * hex = "0123456789ABCDEF\0";
	
	int i;
	for (i = 0 ; i < strlen(in_str) ; i++) {
		char char1 = hex[in_str[i] >> 4];
		char char2 = hex[in_str[i] & 0x0000FFFF];
		strncat(out_str, &char1, 1);
		strncat(out_str, &char2, 1);
	}
}

char * my_sum(struct block buffer) {
	const char * hex = "0123456789ABCDEF";
	u32 i;
	u64 sum = 0, slice = 0;
	for (i = 0 ; i < buffer.size ; i++)
		sum += buffer.data[i];
	slice = sum;
	char * output = 0;
	for (i = 0 ; i < 16 ; i++) {
		u64 temp = 0;
		temp = slice & 0xF;
		//temp = temp >> 60;
		char nybble = hex[temp];
		slice = slice >> 4;
		strncat(output, &nybble, 1);
	}
	return output;
}
