#include <stdio.h>
#include <ogcsys.h>
#include <sys/stat.h>

#include "subsystem.h"
#include "fat.h"
#include "wpad.h"
#include "config.h"
#include "restart.h"
#include "net.h"

void Subsystem_Init(u8 method)
{
	if (method == VERBOSE) printf("[+] Initializing Subsystems\n\n");
		
	/* Initialize Wiimote subsystem */
	if (method == VERBOSE) printf("    Initializing Wiimote: ");
	if (Wpad_Init()) {
		if (method == VERBOSE) {
			printf("FAIL\n\n");
			printf("    Will have to exit, sorry...\n");
		}
		sleep(3);
		Restart();
	} else
		if (method == VERBOSE) printf("OK!\n");

	/* Mount SDHC */
	if (method == VERBOSE) printf("    Initializing SD Card: ");
	Fat_MountSDHC();
	if (!Fat_IsMounted()) {
		if (method == VERBOSE) {
			printf("FAIL\n");
			printf("    * Unable to use game covers.\n");
			printf("    Press any button...\n");
		}
	} else {
		if (method == VERBOSE) printf("OK!\n");

		/* Create data directories if needed */
		if (method == VERBOSE) printf("    Initializing Cover Directories: ");
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
			if (method == VERBOSE) {
				printf("FAIL\n");
				printf("    * Unable to use game  covers.\n");
			}
			Fat_UnmountSDHC();
		} else
			if (method == VERBOSE) printf("OK!\n");
	}
	
	/* Initialize Network */
	if (method == VERBOSE) printf("    Initializing Newtwork: ");
	Net_Init();
	if (!Net_IsRunning()) {
		if (method == VERBOSE) {
			printf("FAIL\n");
			printf("    * Unable to download covers.\n\n");
		}
	} else
		if (method == VERBOSE) printf("OK!\n\n");
	
	if (method == VERBOSE) printf("[+] Subsystems Initialized\n\n");
	                
}

void Subsystem_Close(void)
{
	/* Disconnect Wiimotes */
	Wpad_Disconnect();

	/* Unmount SDHC */
	Fat_UnmountSDHC();
	
}
