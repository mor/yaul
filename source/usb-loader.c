#include <stdio.h>
#include <stdlib.h>
#include <ogcsys.h>
#include <string.h>

#include "sys/stat.h"

#include "wpad.h"
#include "disc.h"
#include "gui.h"
#include "menu.h"
#include "restart.h"
#include "subsystem.h"
#include "sys.h"
#include "video.h"
#include "config.h"

int main(int argc, char **argv)
{
	s32 ret;

	/* Load Custom IOS */
	ret = IOS_ReloadIOS(249);

	/* Initialize system */
	Sys_Init();

	/* Initialize subsystems */
	Subsystem_Init();

	/* Set video mode */
	Video_SetMode();

	/* Initialize console */
	Gui_InitConsole();

	/* Show background */
	//Gui_DrawBackground();

	/* Check if Custom IOS is loaded */
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Custom IOS could not be loaded! (ret = %d)\n", ret);

		goto out;
	}

	/* Initialize DIP module */
	ret = Disc_Init();
	if (ret < 0) {
		printf("[+] ERROR:\n");
		printf("    Could not initialize DIP module! (ret = %d)\n", ret);

		goto out;
	}

        struct stat filestat;

        char data_path[100];
        strcpy(data_path, DATA_PATH);
        if (!stat(data_path, &filestat))
        	printf("%s already exists!", data_path);
	else {
		printf("%s does not exist...", data_path);
		printf("creating %s.", data_path);
		mode_t mode = 0777;
		mkdir(data_path, mode);
	}

        char cover_path[100];
        strcpy(cover_path, COVERS_PATH);
        if (!stat(cover_path, &filestat))
        	printf("%s already exists!", cover_path);
	else {
		printf("%s does not exist...", cover_path);
		printf("creating %s.", cover_path);
		mode_t mode = 0777;
		mkdir(cover_path, mode);
	}

	printf("Press any button...\n");
	Wpad_WaitButtons();
		
	/* Menu loop */
	Menu_Loop();

out:
	/* Restart */
	Restart_Wait();

	return 0;
}
