#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Application Constants */
#define CONSOLE_XCOORD 258 // 250 + 354 = 604 + 16 = 620
#define CONSOLE_YCOORD 112 // 104 + 304 = 408 + 16 = 424
#define CONSOLE_WIDTH 354
#define CONSOLE_HEIGHT 304

#define COVER_XCOORD 24  //  20 + 160 = 180 + 8 = 188
#define COVER_YCOORD 104 // 104 + 224 = 328 + 8 = 336
#define COVER_WIDTH 160
#define COVER_HEIGHT 225

#define BACKGROUND_WIDTH 640
#define BACKGROUND_HEIGHT 480

#define USB_DEVICE_NAME "USB Mass Storage Device"
#define SD_DEVICE_NAME "SD/SDHC Card"

#define DEVICE_TIMEOUT 20

#define DATA_PATH "sdhc:/usb-loader"
#define COVERS_PATH DATA_PATH "/covers"

// Allow for different cover file hosts on the internet
//  in the 2D and 3D PATH strings, %s get replaced with header->id
#define COVER_HOST_US "wiitdb.com"
#define COVER_HOST_US_2D_PATH "/wiitdb/artwork/cover/US/%s.png"
#define COVER_HOST_US_3D_PATH "/sites/default/files/cover/3D%%20Cover/%s.png"

#define COVER_HOST_EN "wiitdb.com"
#define COVER_HOST_EN_2D_PATH "/wiitdb/artwork/cover/EN/%s.png"
#define COVER_HOST_EN_3D_PATH "/wii_3D_box_images/%s.png"

#define COVER_HOST_JA "wiitdb.com"
#define COVER_HOST_JA_2D_PATH "/wiitdb/artwork/cover/JA/%s.png"
#define COVER_HOST_JA_3D_PATH "/3dBoxArt176x248/%s.png"

#define COVER_HOST_4 "www.wiiboxart.com"
#define COVER_HOST_4_2D_PATH "/ntsc/download.php?file=%s.png"
#define COVER_HOST_4_3D_PATH "/3d/160/225/%s.png"

#define COVER_HOST_5 "www.wiiboxart.com"
#define COVER_HOST_5_2D_PATH "/pal/download.php?file=%s.png"
#define COVER_HOST_5_3D_PATH "/3d/160/225/%s.png"

#define UPDATE_HOST "saratoga.ath.cx"
#define UPDATE_DOL_PATH "/latest_version/boot.dol"
#define UPDATE_XML_PATH "/latest_version/meta.xml"
#define UPDATE_PNG_PATH "/latest_version/icon.png"
#define UPDATE_VER_PATH "/LATEST_VERSION"

#define DOL_FILE "sdhc:/apps/yaul/boot.dol"
#define XML_FILE "sdhc:/apps/yaul/meta.xml"
#define PNG_FILE "sdhc:/apps/yaul/icon.png"

#endif
