#include <stdio.h>
#include <ogcsys.h>
#include <sys/stat.h>

#include "subsystem.h"
#include "fat.h"
#include "wpad.h"
#include "config.h"
#include "restart.h"
#include "net.h"
#include "disc.h"
#include "wdvd.h"

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

        if (method == VERBOSE) printf("    Initializing Wii DVD: ");
        s32 ret;
	ret = Disc_Init();
	if (ret < 0) {
		if (method == VERBOSE) printf("FAIL\n");
		if (method == VERBOSE) printf("    * Unable to install from DVD.\n\n");
	} else
		if (method == VERBOSE) printf("OK!\n\n");

	
	///* Initialize Network */
	//if (method == VERBOSE) printf("    Initializing Newtwork: ");
	//Net_Init();
	///* Network Init never returns if it fails, so we don't check anything.
	//   if we got this far, it must have initialized... */	
	//if (method == VERBOSE) printf("OK!\n\n");

	///* Pre-cache IPs of needed hostnames */
	//Net_GetIP(COVER_HOST_1);
	//Net_GetIP(COVER_HOST_2);
	//Net_GetIP(COVER_HOST_3);
	//Net_GetIP(UPDATE_HOST);

	if (method == VERBOSE) printf("[+] Subsystems Initialized\n\n");
}

void Subsystem_Close(void)
{
	/* Disconnect Wiimotes */
	Wpad_Disconnect();

	/* Unmount SDHC */
	Fat_UnmountSDHC();

	/* Shutdown DVD */
	WDVD_Close();	
}
