#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <sys/stat.h>

#include "disc.h"
#include "subsystem.h"
#include "usbstorage.h"
#include "gui.h"
#include "menu.h"
#include "partition.h"
#include "restart.h"
#include "sys.h"
#include "utils.h"
#include "video.h"
#include "wbfs.h"
#include "wpad.h"
#include "config.h"
#include "cover.h"
#include "update.h"

/* Constants */
#define ENTRIES_PER_PAGE	12
#define MAX_CHARACTERS		28

/* Gamelist buffer */
static struct discHdr *gameList = NULL;

/* Gamelist variables */
static s32 gameCnt = 0, gameSelected = 0, gameStart = 0, selectedCnt = 0;

static bool listRefresh = true;

/* WBFS device */
static s32 wbfsDev = WBFS_MIN_DEVICE;


s32 __Menu_EntryCmp(const void *a, const void *b)
{
	struct discHdr *hdr1 = (struct discHdr *)a;
	struct discHdr *hdr2 = (struct discHdr *)b;

	/* Compare strings */
	return strcmp(hdr1->title, hdr2->title);
}

s32 __Menu_GetEntries(void)
{
	struct discHdr *buffer = NULL;

	u32 cnt, len;
	s32 ret;

	/* Get list length */
	ret = WBFS_GetCount(&cnt);
	if (ret < 0)
		return ret;

	/* Buffer length */
	len = sizeof(struct discHdr) * cnt;

	/* Allocate memory */
	buffer = (struct discHdr *)memalign(32, len);
	if (!buffer)
		return -1;

	/* Clear buffer */
	memset(buffer, 0, len);

	/* Get header list */
	ret = WBFS_GetHeaders(buffer, cnt, sizeof(struct discHdr));
	if (ret < 0)
		goto err;

	/* Sort entries */
	qsort(buffer, cnt, sizeof(struct discHdr), __Menu_EntryCmp);

	/* Free memory */
	if (gameList)
		free(gameList);

	/* Set values */
	gameList = buffer;
	gameCnt  = cnt;

	/* Reset variables */
	gameSelected = gameStart = 0;

	return 0;

err:
	/* Free memory */
	if (buffer)
		free(buffer);

	return ret;
}


void Menu_CoverFetch(void)
{	
	Con_Clear();

	u16 coverCnt = 0, cnt;	
	for (cnt = gameStart; cnt < gameCnt; cnt++) {   
                struct discHdr *header = &gameList[cnt];
                if (Cover_Exists(header->id)) 
			coverCnt++;
	}
	
	printf("[+] Games: %d  Covers: %d\n\n", gameCnt, coverCnt);
	if (gameCnt == coverCnt) {
		printf("    You have all game covers.\n\n");
		printf("    Press any button...\n");
		Wpad_WaitButtons();
	} else {
		printf("    Press A to download %d missing covers.\n", gameCnt - coverCnt);
		printf("    Press B to skip cover downloading.\n\n");

                /* Wait for user answer */
                for (;;) {
                        u32 buttons = Wpad_WaitButtons();

                        /* A button */
                        if (buttons & WPAD_BUTTON_A) {
                                for (cnt = gameStart; cnt < gameCnt; cnt++) {
					struct discHdr *header = &gameList[cnt];
					if (!Cover_Exists(header->id)) {
						__Menu_PrintInfo(header);
						s32 bytes = Cover_Fetch(header->id);
						if (bytes > 0)
							printf(" : OK!\n\n");
						if (bytes ==  0)
							printf(" : FAIL on Net_GetFile()\n\n");
						if (bytes < 0)
							printf(" : FAIL on Fat_WriteFile()\n\n");
						sleep(1);
					}
				}
				printf("    Cover download complete.\n\n");
				printf("    Press any button...\n");
				Wpad_WaitButtons();
                                break;
                        }

                        /* B button */
                        if (buttons & WPAD_BUTTON_B)
                                break;
                }
	}
}		

	
char *__Menu_PrintTitle(char *name)
{
	static char buffer[MAX_CHARACTERS + 4];

	///* Pad buffer with spaces */
	//memset(buffer, 32, sizeof(buffer));

	/* Check string length */
	if (strlen(name) > (MAX_CHARACTERS + 3)) {
		memset(buffer, 0, sizeof(buffer));
		strncpy(buffer, name,  MAX_CHARACTERS);
		strncat(buffer, "...", 3);
		strncat(buffer, " ", 1);
	} else  {
		memset(buffer, 32, sizeof(buffer));
		strncpy(buffer, name, strlen(name));
	}

	return buffer;
}

f32 __Menu_GameSize(struct discHdr *header)
{
	f32 size = 0.0;
	WBFS_GameSize(header->id, &size);
	return size;
}

void __Menu_PrintInfo(struct discHdr *header)
{
	/* Print game info */
	printf("    %s\n", header->title);
	printf("    (%s) (%.2fg)", header->id, __Menu_GameSize(header) );
}

void __Menu_MoveList(s8 delta)
{
	s32 index;

	/* No game list */
	if (!gameCnt)
		return;

	/* Select next entry */
	gameSelected += delta;

	/* Out of the list? */
	if (gameSelected <= -1)
		gameSelected = (gameCnt - 1);
	if (gameSelected >= gameCnt)
		gameSelected = 0;

	/* List scrolling */
	index = (gameSelected - gameStart);

	if (index >= ENTRIES_PER_PAGE) {
		gameStart += index - (ENTRIES_PER_PAGE - 1);
		listRefresh = true;
	}

	if (index <= -1) {
		gameStart += index;
		listRefresh = true;
	}

	if (!listRefresh && (delta == 1 || delta == -1) ) {
	        /* Move to previously 'selected' row */
        	printf("\x1b[u");
        	fflush(stdout);

		/* Retrieve title/size of previously 'selected' game */
		struct discHdr *header = &gameList[selectedCnt];
		printf(" %s%.2fg ", __Menu_PrintTitle(header->title), __Menu_GameSize(header));

		/* Change to new row */
		if (delta == 1) {
			printf("\n");
			printf("   ");
		}
		else {
			printf("\x1b[u"); //restore curs again
			fflush(stdout);
			printf("\x1b[1A"); //nudge it to the line above
	                fflush(stdout);
		}
		
		/* Retrieve title/size of new game */
		selectedCnt += delta;
		header = &gameList[selectedCnt];
		
		///printf("   ");
		
		/* Save current cursor posn */
		printf("\x1b[s");
		fflush(stdout);
		/* Print it reverse */
		Con_ReverseVideo();
                printf(" %s%.2fg ", __Menu_PrintTitle(header->title), __Menu_GameSize(header));
                Con_NormalVideo();
	}	
}

void __Menu_ShowList(void)
{
	f32 free, used;

	/* Get free space */
	WBFS_DiskSpace(&used, &free);

	printf("[+] WBFS device contains these %d games:\n\n", gameCnt);

	/* Print out list of game titles and sizes */
	if (gameCnt) {
		u32 cnt;

		/* Print game list */
		for (cnt = gameStart; cnt < gameCnt; cnt++) {
			struct discHdr *header = &gameList[cnt];

			/* Entries per page limit reached */
			if ((cnt - gameStart) >= ENTRIES_PER_PAGE)
				break;

			printf("   ");

			if (gameSelected == cnt) {
				/* Save selected game index */
				selectedCnt = cnt;
				/* Save current cursor posn */
				printf("\x1b[s");
				fflush(stdout);
				/* Turn on reverse video scroll bar */
				Con_ReverseVideo();
			}
                        printf(" %s%.2fg \n", __Menu_PrintTitle(header->title), __Menu_GameSize(header));
                        Con_NormalVideo();
		}
	} else
		/* Unless there aren't any */
		printf("\t>> No games found!!\n");

	/* Print free/used space */
	printf("\n[+] Free: %.2fg  Used: %.2fg\n\n", free, used);
	
	listRefresh = false;
}

void __Menu_ShowCover(void)
{
	struct discHdr *header = &gameList[gameSelected];

	/* No game list*/
	if (!gameCnt)
		return;

	/* Draw cover */
	Gui_DrawCover(header->id);
}

void __Menu_Controls(void)
{
	for (;;) 
	{
		u32 buttons = Wpad_GetButtons();

		/* UP/DOWN buttons */
		if (buttons & WPAD_BUTTON_UP) {
			__Menu_MoveList(-1);
			break;
		}

		if (buttons & WPAD_BUTTON_DOWN) {
			__Menu_MoveList(1);
			break;
		}

		/* LEFT/RIGHT buttons */
		if (buttons & WPAD_BUTTON_LEFT) {
			__Menu_MoveList(-ENTRIES_PER_PAGE);
			break;
		}

		if (buttons & WPAD_BUTTON_RIGHT) {
			__Menu_MoveList(ENTRIES_PER_PAGE);
			break;
		}

		/* HOME button */
		if (buttons & WPAD_BUTTON_HOME) {
			Restart();
			break;
		}

		/* PLUS (+) button */
		if (buttons & WPAD_BUTTON_PLUS) {
			Gui_DrawTitle();
			Menu_Install();
			listRefresh = true;
			break;
		}

		/* MINUS (-) button */
		if (buttons & WPAD_BUTTON_MINUS) {
			Gui_DrawTitle();
			Menu_Remove();
			listRefresh = true;
			break;
		}

		/* ONE (1) button */
		if (buttons & WPAD_BUTTON_1) {
			Gui_DrawTitle();
			USBStorage_Deinit();
			Menu_Device();
			listRefresh = true;
			break;
		}

		/* A button */
		if (buttons & WPAD_BUTTON_A) {
			Menu_Boot();
			break;
		}
		if (buttons & WPAD_BUTTON_B) {
			Menu_Config();
			listRefresh = true;
			break;
		}
		
		if (buttons & WPAD_BUTTON_2) {
			Gui_DrawTitle();
			Menu_CoverFetch();
			Menu_Update();
			listRefresh = true;
			break;
		}
	}
}

void Menu_Update(void)
{
	bool cancel = false;
	
	Con_Clear();

	printf("[+] Checking for yaul upgrades...\n\n");
	
	printf("    Your currrent version: %s\n", _YAUL_VERSION_);

	struct block latest_version;
	latest_version = Update_CheckVersion();
	if (latest_version.data == NULL) {
		printf("    Error, unable to connect to yaul\n    update server.  Try again later.\n\n");
		goto err;
	}
		
	//printf("    downloaded VERSION size: %u\n\n", latest_version.size);
	char version[9];
	memset(version, 0, 9);

	int i;
	for (i = 1; i < latest_version.size - 1 ; i++)
		version[i - 1] = latest_version.data[i];

	printf("      Latest yaul release: %s\n\n", version);
	
	if (strcmp(_YAUL_VERSION_, version) == 0) {
		printf("    You have the latest version.\n\n");
		printf("    Press A to Re-install it.\n");
	}
	else {
		printf("    An upgrade is available.\n\n");
		printf("    Press A to Upgrade yaul\n");
	}
	
	printf("    Press B to Cancel.\n\n");
	
	/* Wait for user answer */
	for (;;) {
		u32 buttons = Wpad_WaitButtons();

		/* A button */
		if (buttons & WPAD_BUTTON_A) {
			s32 ret;
			ret = Update_Fetch();
			if (ret == -1)
				printf("    UPDATE FAILED!\n\n");
			else {
				printf("    Update Succeeded! You need to restart.\n");
				Restart_Wait();
			}
			break;
		}

		/* B button */
		if (buttons & WPAD_BUTTON_B) {
			cancel = true;
			break;
		}
	}

err:
	if (!cancel) {
		printf("    Press any button...");
		Wpad_WaitButtons();
	}
}
	
void Menu_Config(void)
{
	struct discHdr *header = NULL;

	/* No game list */
	if (!gameCnt)
		return;

	/* Selected game */
	header = &gameList[gameSelected];

	/* Clear console */
	Con_Clear();

	/* Show game info */
	printf("[+] Info/Status for:\n\n");
	__Menu_PrintInfo(header);
	printf("\n\n");
	
	printf("    Have DVD box cover? ... ");

	if (!Cover_Exists(header->id)) {
		printf("NO\n\n");
		printf("    Press A button to FETCH cover.\n");
		printf("    Press B button to skip.\n\n");

		/* Wait for user answer */
		for (;;) {
			u32 buttons = Wpad_WaitButtons();

			/* A button */
			if (buttons & WPAD_BUTTON_A) {
				Cover_Fetch(header->id);
				break;
			}

			/* B button */
			if (buttons & WPAD_BUTTON_B)
				return;
		}
	} else {
		printf("YES\n\n");
		printf("    Press A button to DELETE cover.\n");
		printf("    Press B button to skip.\n\n");

		/* Wait for user answer */
		for (;;) {
			u32 buttons = Wpad_WaitButtons();

			/* A button */
			if (buttons & WPAD_BUTTON_A)
				break;

			/* B button */
			if (buttons & WPAD_BUTTON_B)
				return;
		}

		if ( Cover_Delete(header->id) == 0 )
			printf("    Cover deleted!\n\n");
		else
			printf("    Error, unable to delete cover...\n\n");

	}

	printf("    Press any button...\n");
	Wpad_WaitButtons();
	return;	
}

void Menu_Format(void)
{
	partitionEntry partitions[MAX_PARTITIONS];

	u32 cnt, sector_size;
	s32 ret, selected = 0;

	/* Clear console */
	Con_Clear();

	/* Get partition entries */
	ret = Partition_GetEntries(wbfsDev, partitions, &sector_size);
	if (ret < 0) {
		printf("[+] ERROR: No partitions found! (ret = %d)\n", ret);

		/* Restart */
		//Restart_Wait();
		Menu_Device();
	}

loop:
	/* Clear console */
	Con_Clear();

	printf("[+] Selected the partition you want to \n");
	printf("    format:\n\n");

	/* Print partition list */
	for (cnt = 0; cnt < MAX_PARTITIONS; cnt++) {
		partitionEntry *entry = &partitions[cnt];

		/* Calculate size in gigabytes */
		f32 size = entry->size * (sector_size / GB_SIZE);

		/* Selected entry */
		(selected == cnt) ? printf(">> ") : printf("   ");
		fflush(stdout);

		/* Valid partition */
		if (size)
			printf("Partition #%d: (size = %.2fg)\n",       cnt + 1, size);
		else 
			printf("Partition #%d: (cannot be formatted)\n", cnt + 1);
	}

	partitionEntry *entry = &partitions[selected];
	u32           buttons = Wpad_WaitButtons();

	/* UP/DOWN buttons */
	if (buttons & WPAD_BUTTON_UP) {
		if ((--selected) <= -1)
			selected = MAX_PARTITIONS - 1;
	}

	if (buttons & WPAD_BUTTON_DOWN) {
		if ((++selected) >= MAX_PARTITIONS)
			selected = 0;
	}

	/* B button */
	if (buttons & WPAD_BUTTON_B)
		return;

	/* Valid partition */
	if (entry->size) {
		/* A button */
		if (buttons & WPAD_BUTTON_A)
			goto format;
	}

	goto loop;

format:
	/* Clear console */
	Con_Clear();

	printf("[+] Are you sure you want to format\n");
	printf("    this partition?\n\n");

	printf("    Partition #%d\n",                  selected + 1);
	printf("    (size = %.2fg - type: %02X)\n\n", entry->size * (sector_size / GB_SIZE), entry->type);

	printf("    Press A button to FORMAT partition.\n");
	printf("    Press B button to go back.\n\n\n");

	/* Wait for user answer */
	for (;;) {
		u32 buttons = Wpad_WaitButtons();

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;

		/* B button */
		if (buttons & WPAD_BUTTON_B)
			goto loop;
	}

	printf("[+] Formatting, please wait...");
	fflush(stdout);

	/* Format partition */
	ret = WBFS_Format(entry->sector, entry->size);
	if (ret < 0) {
		printf("\n    ERROR! (ret = %d)\n", ret);
		goto out;
	} else
		printf(" OK!\n");

out:
	printf("\n");
	printf("    Press any button to continue...\n");

	/* Wait for any button */
	Wpad_WaitButtons();
}

void Menu_Auto(void)
{
	u32 timeout = DEVICE_TIMEOUT;
	s32 ret;

	//Con_Clear();
	printf("[+] Scanning for USB/WBFS device...\n\n");	
	/* Initialize WBFS */
	ret = WBFS_Init(WBFS_DEVICE_USB, timeout);
	if (ret < 0) {
		printf("    USB/WBFS Not Found...\n\n");
		printf("    Press any button for device menu...\n");
		Wpad_WaitButtons();
		Menu_Device();
	}

	//printf("    Found!");
	
	/* Try to open device */
	if (WBFS_Open() < 0) {
		printf("    USB/WBFS Found, but failed to open...\n\n");
		printf("    Press any button for device menu...\n");
		Wpad_WaitButtons();
		Menu_Device();
	}

	/* Get game list */
	__Menu_GetEntries();
}


void Menu_Device(void)
{
	u32 timeout = DEVICE_TIMEOUT;
	s32 ret;

	/* Ask user for device */
	for (;;) {


		char *devname = "Unknown!";

top:
		/* Set device name */
		switch (wbfsDev) {
			case WBFS_DEVICE_USB:
				devname = USB_DEVICE_NAME;
				break;

			case WBFS_DEVICE_SDHC:
				devname = SD_DEVICE_NAME;
				break;
		}

		/* Clear console */
		Con_Clear();

		printf("[+] Select WBFS device:\n");
		printf("    < %s >\n\n", devname);

		printf("    Press LEFT/RIGHT to select device.\n");
		printf("    Press A button to mount selected device.\n");
		printf("    Press B button to quit to loader.\n\n");
		u32 buttons = Wpad_WaitButtons();

		/* LEFT/RIGHT buttons */
		if (buttons & WPAD_BUTTON_LEFT) {
			if ((--wbfsDev) < WBFS_MIN_DEVICE)
				wbfsDev = WBFS_MAX_DEVICE;
		}
		if (buttons & WPAD_BUTTON_RIGHT) {
			if ((++wbfsDev) > WBFS_MAX_DEVICE)
				wbfsDev = WBFS_MIN_DEVICE;
		}

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;
		
		if (buttons & WPAD_BUTTON_B)
			Restart();		
	}

	printf("[+] Mounting device, please wait...\n");
	printf("    (%d seconds timeout)\n\n", timeout);
	fflush(stdout);

	/* Initialize WBFS */
	ret = WBFS_Init(wbfsDev, timeout);
	if (ret < 0) {
		printf("    ERROR! (ret = %d)\n\n", ret);
		printf("    Press any button...\n");
		Wpad_WaitButtons();
		goto top;
	}
	
	/* Try to open device */
	while (WBFS_Open() < 0) {
		/* Clear console */
		Con_Clear();

		printf("[+] WARNING:\n\n");

		printf("    No WBFS partition found!\n");
		printf("    You need to format a partition.\n\n");

		printf("    Press A button to format a partition.\n");
		printf("    Press B button to cancel.\n\n");

		/* Wait for user answer */
		for (;;) {
			u32 buttons = Wpad_WaitButtons();

			/* A button */
			if (buttons & WPAD_BUTTON_A) {
				break;
			}

			/* B button */
			if (buttons & WPAD_BUTTON_B) {
				goto top;
			}
		}
		Menu_Format();
	}

	/* Get game list */
	__Menu_GetEntries();
	
}

void Menu_Install(void)
{
	static struct discHdr header ATTRIBUTE_ALIGN(32);

	s32 ret;

	/* Clear console */
	Con_Clear();

	printf("[+] Are you sure you want to install a\n");
	printf("    new Wii game?\n\n");

	printf("    Press A button to continue.\n");
	printf("    Press B button to go back.\n\n\n");

	/* Wait for user answer */
	for (;;) {
		u32 buttons = Wpad_WaitButtons();

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;

		/* B button */
		if (buttons & WPAD_BUTTON_B)
			return;
	}

	/* Disable WBFS mode */
	Disc_SetWBFS(0, NULL);

	printf("[+] Insert the game DVD disc...");
	fflush(stdout);

	/* Wait for disc */
	ret = Disc_Wait();
	if (ret < 0) {
		printf("\n    ERROR! (ret = %d)\n", ret);
		goto out;
	} else
		printf(" OK!\n");

	printf("[+] Opening DVD disc...");
	fflush(stdout);

	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		printf("\n    ERROR! (ret = %d)\n", ret);
		goto out;
	} else
		printf(" OK!\n\n");

	/* Check disc */
	ret = Disc_IsWii();
	if (ret < 0) {
		printf("[+] ERROR: Not a Wii disc!!\n");
		goto out;
	}

	/* Read header */
	Disc_ReadHeader(&header);

	/* Check if game is already installed */
	ret = WBFS_CheckGame(header.id);
	if (ret) {
		printf("[+] ERROR: Game already installed!!\n");
		goto out;
	}

	printf("[+] Installing game, please wait...\n\n");

	printf("    %s\n",           header.title);
	printf("    (%s)\n\n", header.id);

	/* Install game */
	ret = WBFS_AddGame();
	if (ret < 0) {
		printf("[+] Installation ERROR! (ret = %d)\n", ret);
		goto out;
	}

	/* Reload entries */
	__Menu_GetEntries();

out:
	printf("\n");
	printf("    Press any button to continue...\n");

	/* Wait for any button */
	Wpad_WaitButtons();
}

void Menu_Remove(void)
{
	struct discHdr *header = NULL;

	s32 ret;

	/* No game list */
	if (!gameCnt)
		return;

	/* Selected game */
	header = &gameList[gameSelected];

	/* Clear console */
	Con_Clear();

	printf("[+] Are you sure you want to remove this\n");
	printf("    game?\n\n");

	/* Show game info */
	__Menu_PrintInfo(header);
	printf("\n\n");

	printf("    Press A button to continue.\n");
	printf("    Press B button to go back.\n\n\n");

	/* Wait for user answer */
	for (;;) {
		u32 buttons = Wpad_WaitButtons();

		/* A button */
		if (buttons & WPAD_BUTTON_A)
			break;

		/* B button */
		if (buttons & WPAD_BUTTON_B)
			return;
	}

	printf("[+] Removing game, please wait...");
	fflush(stdout);

	/* Remove game */
	ret = WBFS_RemoveGame(header->id);
	if (ret < 0) {
		printf("\n  ERROR! (ret = %d)\n", ret);
		goto out;
	} else
		printf(" OK!\n");

	/* Reload entries */
	__Menu_GetEntries();

out:
	printf("\n");
	printf("    Press any button to continue...\n");

	/* Wait for any button */
	Wpad_WaitButtons();
}

void Menu_Boot(void)
{
	struct discHdr *header = NULL;

	s32 ret;

	/* No game list */
	if (!gameCnt)
		return;

	/* Selected game */
	header = &gameList[gameSelected];

//	/* Clear console */
//	Con_Clear();

//	printf("[+] Are you sure you want to boot this\n");
//	printf("    game?\n\n");

//	/* Show game info */
//	__Menu_PrintInfo(header);

//	printf("    Press A button to continue.\n");
//	printf("    Press B button to go back.\n\n");

//	/* Wait for user answer */
//	for (;;) {
//		u32 buttons = Wpad_WaitButtons();

//		/* A button */
//		if (buttons & WPAD_BUTTON_A)
//			break;

//		/* B button */
//		if (buttons & WPAD_BUTTON_B)
//			return;
//	}

	//printf("\n");
	Con_NextToLastLine();
	printf("[+] Run: %s", header->title);

	/* Set WBFS mode */
	Disc_SetWBFS(wbfsDev, header->id);

	/* Open disc */
	ret = Disc_Open();
	if (ret < 0) {
		printf("    ERROR: Could not open game! (ret = %d)\n", ret);
		goto out;
	}

	/* Boot Wii disc */
	Disc_WiiBoot();

	printf("    Returned! (ret = %d)\n", ret);

out:
	printf("\n");
	printf("    Press any button to continue...\n");

	/* Wait for button */
	Wpad_WaitButtons();
}


void Menu_Loop(void)
{
	/* Device menu */
	//Menu_Device();
	Gui_DrawTitle();
	Menu_Auto();
	
	/* Menu loop */
	for (;;) {
		/* Clear console */
		//Con_Clear();

		/* Show gamelist */
		if (listRefresh) {
			Con_Clear();
			__Menu_ShowList();
		}

		/* Show cover */
		__Menu_ShowCover();

		/* Controls */
		__Menu_Controls();
	}
}
