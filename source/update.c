#include "update.h"
#include "fat.h"
#include "config.h"

s32 Update_Fetch() {

	s32 ret = -1;

	char host[64];
	strcpy(host, UPDATE_HOST);
	char path[100];
	strcpy(path, UPDATE_PATH);
	char target[100];
	strcpy(target, APP_FILE);

	//printf("   Update host: %s\n", host);
	//printf("   Update path: %s\n", path);
	//printf("   Target file: %s\n\n", target);

	struct block update_file = Net_GetFile(host, path);
	if (update_file.size > 0) {
		//printf("    Recvd: %d bytes\n", update_file.size);
		ret = Fat_WriteFile(target, update_file);
		if (ret != -1)  
			ret = 0;
	}	       
	return ret;
}
