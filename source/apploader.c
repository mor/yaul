#include <stdio.h>
#include <ogcsys.h>
#include <string.h>

#include "apploader.h"
#include "wdvd.h"
#include "wpad.h"

/* Apploader function pointers */
typedef int   (*app_main)(void **dst, int *size, int *offset);
typedef void  (*app_init)(void (*report)(const char *fmt, ...));
typedef void *(*app_final)();
typedef void  (*app_entry)(void (**init)(void (*report)(const char *fmt, ...)), int (**main)(), void *(**final)());

/* Apploader pointers */
static u8 *appldr = (u8 *)0x81200000;


/* Constants */
#define APPLDR_OFFSET	0x2440

/* Variables */
static u32 buffer[0x20] ATTRIBUTE_ALIGN(32);


static void __noprint(const char *fmt, ...)
{
}

/* Anti 002 fix for IOS 249 */
void Apply_Anti_002_Fix(void *Address, int Size)
{
        u8 SearchPattern[12] =  { 0x2C, 0x00, 0x00, 0x00, 0x48, 0x00, 0x02, 0x14, 0x3C, 0x60, 0x80, 0x00 };
        u8 PatchData[12] =      { 0x2C, 0x00, 0x00, 0x00, 0x40, 0x82, 0x02, 0x14, 0x3C, 0x60, 0x80, 0x00 };

        void *Addr = Address;
        void *Addr_end = Address+Size;

        while(Addr <= Addr_end-sizeof(SearchPattern))
        {
                if(memcmp(Addr, SearchPattern, sizeof(SearchPattern))==0)
                {
                        memcpy(Addr,PatchData,sizeof(PatchData));
                }
                Addr += 4;
        }
}


/** New Super Mario Bros. Wii copy protection fix **/
/** Thanks to WiiPower **/
bool Apply_NSMB_Wii_Fix(void *Address, int Size)
{
	if (memcmp("SMNE", (char *)0x80000000, 4) == 0)
	{
		u8 SearchPattern[32] = 	{ 0x94, 0x21, 0xFF, 0xD0, 0x7C, 0x08, 0x02, 0xA6, 0x90, 0x01, 0x00, 0x34, 0x39, 0x61, 0x00, 0x30, 0x48, 0x12, 0xD7, 0x89, 0x7C, 0x7B, 0x1B, 0x78, 0x7C, 0x9C, 0x23, 0x78, 0x7C, 0xBD, 0x2B, 0x78 };
		u8 PatchData[32] = 		{ 0x4E, 0x80, 0x00, 0x20, 0x7C, 0x08, 0x02, 0xA6, 0x90, 0x01, 0x00, 0x34, 0x39, 0x61, 0x00, 0x30, 0x48, 0x12, 0xD7, 0x89, 0x7C, 0x7B, 0x1B, 0x78, 0x7C, 0x9C, 0x23, 0x78, 0x7C, 0xBD, 0x2B, 0x78 };

		void *Addr = Address;
		void *Addr_end = Address+Size;

		while(Addr <= Addr_end-sizeof(SearchPattern))
		{
			if(memcmp(Addr, SearchPattern, sizeof(SearchPattern))==0)
			{
				memcpy(Addr,PatchData,sizeof(PatchData));
				return true;
			}
			Addr += 4;
		}
	}
	else if (memcmp("SMN", (char *)0x80000000, 3) == 0)
	{
		u8 SearchPattern[32] = 	{ 0x94, 0x21, 0xFF, 0xD0, 0x7C, 0x08, 0x02, 0xA6, 0x90, 0x01, 0x00, 0x34, 0x39, 0x61, 0x00, 0x30, 0x48, 0x12, 0xD9, 0x39, 0x7C, 0x7B, 0x1B, 0x78, 0x7C, 0x9C, 0x23, 0x78, 0x7C, 0xBD, 0x2B, 0x78 };
		u8 PatchData[32] = 		{ 0x4E, 0x80, 0x00, 0x20, 0x7C, 0x08, 0x02, 0xA6, 0x90, 0x01, 0x00, 0x34, 0x39, 0x61, 0x00, 0x30, 0x48, 0x12, 0xD9, 0x39, 0x7C, 0x7B, 0x1B, 0x78, 0x7C, 0x9C, 0x23, 0x78, 0x7C, 0xBD, 0x2B, 0x78 };

		void *Addr = Address;
		void *Addr_end = Address+Size;

		while(Addr <= Addr_end-sizeof(SearchPattern))
		{
			if(memcmp(Addr, SearchPattern, sizeof(SearchPattern))==0)
			{
				memcpy(Addr,PatchData,sizeof(PatchData));
				return true;
			}
			Addr += 4;
		}
	}
	return false;
}



s32 Apploader_Run(entry_point *entry)
{
	app_entry appldr_entry;
	app_init  appldr_init;
	app_main  appldr_main;
	app_final appldr_final;

	u32 appldr_len;
	s32 ret;

	//u8 ios_revision;
	//ios_revision = IOS_GetRevision();
	
	/* Read apploader header */
	ret = WDVD_Read(buffer, 0x20, APPLDR_OFFSET);
	if (ret < 0)
		return ret;

	/* Calculate apploader length */
	appldr_len = buffer[5] + buffer[6];

	/* Read apploader code */
	ret = WDVD_Read(appldr, appldr_len, APPLDR_OFFSET + 0x20);
	if (ret < 0)
		return ret;

	/* Set apploader entry function */
	appldr_entry = (app_entry)buffer[4];

	/* Call apploader entry */
	appldr_entry(&appldr_init, &appldr_main, &appldr_final);

	/* Initialize apploader */
	appldr_init(__noprint);

 	/* Remove 002 */
	*(u32 *)0x80003140 = *(u32 *)0x80003188;
	//printf("    Error 002 fix applied...\n");

	// *UPDATE 3* Using VKI PPF patch on the main.dol you should now be able to launch
	// NSMB FROM DVD, cIOSCORP, USB LAUNCHER, MODCHIP, ETC. Use PPF-O-Matic to patch
	// your Dols. What this patch does is the standard fix that I?ve posted in previous
	// updates and changes the offset locations 0x001CED53 AND 0x001CED6B in the 
	// main.dol from DA to 71. See Links for Wiikey 2 v1.3 firmware.

	// *Update* Some USB loaders have now included the patch into their code and don?t
	// require a patched main.dol for NSMB to run.  Such as CFG-Loader, OpenWiiFlow,
	// etc? Also Wiikey is going to release an update in order to resolve the copy
	// protection issue with Wiikey modchip owners, see comment. Also see my Comment
	// about DVD launching below. DriveKey has also posted an update on there website
	// see comments. Check the comments from Insider about the Wiikey 2 v1.3 update
	// video, which will fix NSMB.

	// *Update 2* The offset in the main.dol for hex editing the NTSC version is: 
	// 0x001AB610 - 0x001AB613 . Change that section from 9421ffd0 to  4e800020 with
	// your hex editor.

	// So he simply patched the Value from 0x1ab750-0x1ab753 -> 4e 80 00 20 and got
	// NSMBW booting via USB Loaders.

	// Okay, then...
	// PAL ?
	//if ( *(u32 *)0x801ab750 == 0x94 ) {
	//  printf("    NSMB_Wii patch applied:\n    0x801ab750 0x%x -> 0x4e", *(u32 *)0x801ab750 );
	//  *(u32 *)0x801ab750 = 0x4e;
	//}
	//if ( *(u32 *)0x801ab751 == 0x21 ) *(u32 *)0x801ab751 = 0x80;
	//if ( *(u32 *)0x801ab752 == 0xff ) *(u32 *)0x801ab752 = 0x00;
	//if ( *(u32 *)0x801ab753 == 0xd0 ) *(u32 *)0x801ab753 = 0x20;

	// NTSC = SMNE01 ?
	//if ( *(u32 *)0x801ab610 == 0x94 ) {
	//  printf("    NSMB_Wii patch applied:\n    0x801ab61 0x%x -> 0x4e", *(u32 *)0x801ab610 );
	//  *(u32 *)0x801ab610 = 0x4e;
	//}
	//if ( *(u32 *)0x801ab611 == 0x21 ) *(u32 *)0x801ab611 = 0x80;
	//if ( *(u32 *)0x801ab612 == 0xff ) *(u32 *)0x801ab612 = 0x00;
	//if ( *(u32 *)0x801ab613 == 0xd0 ) *(u32 *)0x801ab613 = 0x20;

	//*(u32 *)0x1ab610 = 0x4e;
	//*(u32 *)0x1ab611 = 0x80;
	//*(u32 *)0x1ab612 = 0x00;
	//*(u32 *)0x1ab613 = 0x20;
	
	
	for (;;) {
		void *dst = NULL;
		s32   len = 0, offset = 0;

		/* Run apploader main function */
		ret = appldr_main(&dst, &len, &offset);
		if (!ret)
			break;

		/* Read data from DVD */
		WDVD_Read(dst, len, (u64)(offset << 2));

		/* Apply Anti_002 fix as needed */
		//if (ios_revision < 12 || ios_revision > 13)
		Apply_Anti_002_Fix(dst, len);
		//printf("    Anti_002 fix applied...\n");
		
		/* Apply NSMB_Wii copy-protection fix */
		Apply_NSMB_Wii_Fix(dst, len);
	}

	/* Set entry point from apploader */
	*entry = appldr_final();

	return 0;
}
