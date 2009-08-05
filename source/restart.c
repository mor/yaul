#include <stdio.h>
#include <ogcsys.h>
#include <stdlib.h>

#include "sys.h"
#include "wpad.h"
#include "subsystem.h"
#include "usbstorage.h"

void Restart(void)
{
	printf("\n    Restarting Wii...");
	fflush(stdout);

	/* Load system menu */
	Subsystem_Close();
	USBStorage_Deinit();
	exit (0);

}

void Restart_Wait(void)
{
	printf("\n");

	printf("    Press any button to restart...");
	fflush(stdout);

	/* Wait for button */
	Wpad_WaitButtons();

	/* Restart */
	Restart();
}
 
