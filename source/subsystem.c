#include <stdio.h>
#include <ogcsys.h>
#include <sys/stat.h>

#include "fat.h"
#include "wpad.h"
#include "config.h"
#include "restart.h"
#include "net.h"

void Subsystem_Init(void)
{
	printf("[+] Initializing Subsystems\n\n");
		
	/* Initialize Wiimote subsystem */
	printf("    Initializing Wiimote: ");
	if (Wpad_Init()) {
		printf("FAIL\n\n");
		printf("    Will have to exit, sorry...\n");
		sleep(3);
		Restart();
	} else
		printf("OK!\n");

	/* Mount SDHC */
	printf("    Initializing SD Card: ");
	Fat_MountSDHC();
	if (!Fat_IsMounted()) {
		printf("FAIL\n");
		printf("    * Unable to use game covers.\n");
		printf("    Press any button...\n");
	} else {
		printf("OK!\n");

		/* Create data directories if needed */
		printf("    Initializing Cover Directories: ");
	        struct stat filestat;
	        char data_path[100];
	        strcpy(data_path, DATA_PATH);
	        if (stat(data_path, &filestat)) {
			mode_t mode = 0777;
			mkdir(data_path, mode);
		}	

		char cover_path[100];
		strcpy(cover_path, COVERS_PATH);
		if (stat(cover_path, &filestat)) {
			mode_t mode = 0777;
			mkdir(cover_path, mode);
		}
	
		if (stat(data_path, &filestat) || stat(cover_path, &filestat)) {
			printf("FAIL\n");
			printf("    * Unable to use game  covers.\n");
			Fat_UnmountSDHC();
		} else
			printf("OK!\n");
	}
	
	/* Initialize Network */
	printf("    Initializing Newtwork: ");
	Net_Init();
	if (!Net_IsRunning()) {
		printf("FAIL\n");
		printf("    * Unable to download covers.\n\n");
	} else
		printf("OK!\n\n");
	
	printf("[+] Subsystems Initialized\n\n");
	                
}

void Subsystem_Close(void)
{
	/* Disconnect Wiimotes */
	Wpad_Disconnect();

	/* Unmount SDHC */
	Fat_UnmountSDHC();
	
}
