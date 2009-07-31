#include <stdio.h>
#include <ogcsys.h>
#include <string.h>

#include "apploader.h"
#include "wdvd.h"

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
void Apply_Anti_002_fix(void *Address, int Size)
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

s32 Apploader_Run(entry_point *entry)
{
	app_entry appldr_entry;
	app_init  appldr_init;
	app_main  appldr_main;
	app_final appldr_final;

	u32 appldr_len;
	s32 ret;

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
                if (IOS_GetRevision() < 12 || IOS_GetRevision() > 13)
			Apply_Anti_002_fix(dst, len);
	}

	/* Set entry point from apploader */
	*entry = appldr_final();

	return 0;
}
