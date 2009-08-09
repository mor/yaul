#ifndef _CONFIG_H_
#define _CONFIG_H_

/* Application Constants */
#define CONSOLE_XCOORD 258
#define CONSOLE_YCOORD 112
#define CONSOLE_WIDTH 354
#define CONSOLE_HEIGHT 304

#define COVER_XCOORD 24
#define COVER_YCOORD 104
#define COVER_WIDTH 160
#define COVER_HEIGHT 224

#define BACKGROUND_WIDTH 640
#define BACKGROUND_HEIGHT 480

#define USB_DEVICE_NAME "USB Mass Storage Device"
#define SD_DEVICE_NAME "SD/SDHC Card"

#define DEVICE_TIMEOUT 20

#define DATA_PATH "sdhc:/usb-loader"
#define COVERS_PATH DATA_PATH "/covers"

// Allow for two different cover file hosts on the internet
//  in the 2D and 3D PATH strings, %s get replaced with header->id
#define COVER_HOST_1 "wiicover.gateflorida.com"
#define COVER_HOST_1_2D_PATH "/sites/default/files/cover/2D%%20Cover/%s.png"
#define COVER_HOST_1_3D_PATH "/sites/default/files/cover/3D%%20Cover/%s.png"

#define COVER_HOST_2 "saratoga.ath.cx"
#define COVER_HOST_2_2D_PATH "/wii_2D_cover_images/%s.png"
#define COVER_HOST_2_3D_PATH "/wii_3D_box_images/%s.png"

#define COVER_HOST_3 "awiibit.com"
#define COVER_HOST_3_2D_PATH "/BoxArt160x224/%s.png"
#define COVER_HOST_3_3D_PATH "/3dBoxArt176x248/%s.png"
 
#define UPDATE_HOST "saratoga.ath.cx"
#define UPDATE_PATH "/downloads/latest_version/sd-usb-loader.dol"

#define APP_FILE "sdhc:/apps/usb_loader/boot.dol"

#endif
