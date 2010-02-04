#include "cover.h"
#include "wpad.h"

void Cover_GetFilepath(u8 * discid, char * cover_file) {
	 sprintf(cover_file, COVERS_PATH "/%s.png", discid);
}

s32 __Cover_FetchURL(char * host, char * path, u8 * discid) {

	s32 ret;

	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);

	struct block png_data = Net_GetFile(host, path);
	if (png_data.size > 25000) {
		ret = Fat_WriteFile(cover_file, png_data);
		if (ret == -1)
			ret = 0;
	} else
		ret = -1;
	
        return ret;
}			

bool Cover_Exists(u8 * discid) {
	struct stat filestat;
	char cover_file[100];
	sprintf(cover_file, COVERS_PATH "/%s.png", discid);
	return (!stat(cover_file, &filestat));
}

s32 Cover_Fetch(u8 * discid) {
	s32 ret;

	char path[128];
	memset(path, 0, 128);
	char host[64];
	memset(host, 0, 64);
	char country = (char) discid[3];
	if (country == 'E') {
	        printf("NTSC-US ");
		strcpy(host, COVER_HOST_US);
		sprintf(path,  COVER_HOST_US_2D_PATH, discid);
		//printf("trying: %s %s", host, path);
	}
	else if (country == 'P') {
		printf("PAL ");
		strcpy(host, COVER_HOST_EN);
		sprintf(path,  COVER_HOST_EN_2D_PATH, discid);
	}
	else if (country == 'J') {
		printf("NTSC-JA ");
		strcpy(host, COVER_HOST_JA);
		sprintf(path,  COVER_HOST_JA_2D_PATH, discid);
	}
	ret = __Cover_FetchURL(host, path, discid);
	if (ret <= 0) 
		printf("fail: %d\n", ret);
	else
		printf("success.\n");
	return ret;
}

s32 Cover_Delete(u8 * discid) {
	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);
	return unlink(cover_file);
}
