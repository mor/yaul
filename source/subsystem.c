#include <stdio.h>
#include <ogcsys.h>
#include <sys/stat.h>

#include "fat.h"
#include "wpad.h"
#include "config.h"

void Subsystem_Init(void)
{
	/* Initialize Wiimote subsystem */
	Wpad_Init();

	/* Mount SDHC */
	Fat_MountSDHC();

	/* Create data directories if needed */
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

}

void Subsystem_Close(void)
{
	/* Disconnect Wiimotes */
	Wpad_Disconnect();

	/* Unmount SDHC */
	Fat_UnmountSDHC();
}
