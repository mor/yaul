#include "cover.h"
#include "wpad.h"

void Cover_GetFilepath(u8 * discid, char * cover_file) {
	 sprintf(cover_file, COVERS_PATH "/%s.png", discid);
}

s32 __Cover_FetchURL(char * host, char * path, u8 * discid) {

	s32 ret = -1;
	//struct stat filestat;

	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);
	
	Con_Clear();
	printf("[+] Trying %s...\n\n", host);
	printf("    requesting %s.png: ", discid);
	//char path[128];
	//sprintf(path, , discid);

	struct block png_data = Net_GetFile(host, path);
	if (png_data.size > 0) {
		printf("OK!\n");
		printf("    recived %d bytes...\n", png_data.size);
		printf("    writing %d bytes: ", png_data.size);
		ret = Fat_WriteFile(cover_file, png_data);
		if (ret != -1) {
			printf("OK!\n\n");
			printf("    Cover download successful!\n\n");
                } else
			printf("FAIL\n\n");
			
	} else
		printf("FAIL\n\n");
		
        return ret;
}			

bool Cover_Exists(u8 * discid) {
	struct stat filestat;
	char cover_file[100];
	sprintf(cover_file, COVERS_PATH "/%s.png", discid);
	return (!stat(cover_file, &filestat));
}

s32 Cover_Fetch(u8 * discid) {

        //Con_Clear();

	/* Make sure networking is on */
	if (!Net_Is_Running()) {
		printf("[+] Waiting for network to initialise...\n\n");
		Net_Init();
	}

	s32 ret;

	char path[128];
	char host[64];
	u8 choice = 0;

	//printf("\n[+} Select cover host:\n\n");
	//printf("    Press A for %s\n", COVER_HOST_1);
	//printf("    Press B for %s\n", COVER_HOST_2);

	///* Wait for user answer */
	//for (;;) {
	//	u32 buttons = Wpad_WaitButtons();

	//	/* A button */
	//	if (buttons & WPAD_BUTTON_A) {
	//		strcpy(host, COVER_HOST_1);
	//		choice = 1;
	//		break;
	//	}

	//	/* B button */
	//	if (buttons & WPAD_BUTTON_B) {
	//		strcpy(host, COVER_HOST_2);
	//		break;
	//	}
	//}

	printf("\n[+] Select cover art style:\n\n");
	printf("    Press A for 2D cover images\n");
	printf("    Press B for 3D box images\n");

	/* Wait for user answer */
	for (;;) {
		u32 buttons = Wpad_WaitButtons();

		/* A button */
		if (buttons & WPAD_BUTTON_A) {
			if (choice == 1)
				sprintf(path,  COVER_HOST_1_2D_PATH, discid);
			else
				sprintf(path,  COVER_HOST_2_2D_PATH, discid);
			break;
		}

		/* B button */
		if (buttons & WPAD_BUTTON_B) {
			if (choice == 1)
				sprintf(path, COVER_HOST_1_3D_PATH, discid);
			else
				sprintf(path, COVER_HOST_2_3D_PATH, discid);
			break;
		}
	}

	//Con_Clear();
	ret = __Cover_FetchURL(host, path, discid);
	return ret;
}

s32 Cover_Delete(u8 * discid) {
	char cover_file[100];
	Cover_GetFilepath(discid, cover_file);
	return unlink(cover_file);
}
