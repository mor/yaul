#include "cover.h"
#include "wpad.h"

void Cover_GetFilepath(u8 * discid, char * cover_file) {
	 sprintf(cover_file, COVERS_PATH "/%s.png", discid);
}

s32 __Cover_FetchURL(char * host, char * path, u8 * discid) {

	s32 ret = -1;

	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);
	
	struct block png_data = Net_GetFile(host, path);
	if (png_data.size > 0) {
		ret = Fat_WriteFile(cover_file, png_data);
		if (ret != -1)
			ret = 0;
	}		
        return ret;
}			

bool Cover_Exists(u8 * discid) {
	struct stat filestat;
	char cover_file[100];
	sprintf(cover_file, COVERS_PATH "/%s.png", discid);
	return (!stat(cover_file, &filestat));
}

s32 Cover_Fetch(u8 * discid) {

	/* Make sure networking is on */
	if (!Net_IsRunning())
		Net_Init();

	s32 ret;

	char path[128];
	char host[64];

	strcpy(host, COVER_HOST_1);
	sprintf(path,  COVER_HOST_1_2D_PATH, discid);

	ret = __Cover_FetchURL(host, path, discid);
	if (ret == -1) {
		strcpy(host, COVER_HOST_2);
		sprintf(path,  COVER_HOST_2_2D_PATH, discid);
		ret = __Cover_FetchURL(host, path, discid);
	}
	
	return ret;
}

s32 Cover_Delete(u8 * discid) {
	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);
	return unlink(cover_file);
}
