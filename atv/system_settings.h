#ifndef _SCENE_SYSTEM_SETTINGS_H_
#define _SCENE_SYSTEM_SETTINGS_H_

#define OSD_SCENE_SYSTEM_SETTINGS_TITLE "SYS_SET"
#define OSD_SCENE_SYSTEM_SETTINGS_BINARY "system_settings.osd"

/* define the macro to open the macros */
#ifdef OSD_ENABLE_MACROS_SYSTEM_SETTINGS 
#define OSD_WINDOW_PICTURE_MODE     0
#define OSD_INGREDIENT_PICTURE_MODE_LEFT             4
#define OSD_INGREDIENT_PICTURE_MODE_RIGHT            5
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_PICTURE_MODE 29
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_BRIGHTNESS  31
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_CONTRAST    33
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_SHARPNESS   35
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_COLOR       37
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_TINT        39
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_COLOR_TEMP  43
#define OSD_INGREDIENT_PICTURE_MODE_TEXT_ASPECT_RATIO 46

#define OSD_BLOCK_PICTURE_MODE_LEFT_ARROW 56
#define OSD_BLOCK_PICTURE_MODE_RIGHT_ARROW 57
#define OSD_WINDOW_SOUND_MODE       1
#define OSD_INGREDIENT_SOUND_MODE_LEFT             4
#define OSD_INGREDIENT_SOUND_MODE_RIGHT            5
#define OSD_INGREDIENT_SOUND_MODE_TEXT_SOUND_MODE  30
#define OSD_INGREDIENT_SOUND_MODE_TEXT_BASS        32
#define OSD_INGREDIENT_SOUND_MODE_TEXT_TREBLE      34
#define OSD_INGREDIENT_SOUND_MODE_TEXT_BALANCE     36
#define OSD_INGREDIENT_SOUND_MODE_TEXT_SURROUND    39
#define OSD_INGREDIENT_SOUND_MODE_TEXT_AVL         42
#define OSD_INGREDIENT_SOUND_MODE_TEXT_SPDIF       47
#define OSD_INGREDIENT_SOUND_MODE_TEXT_AUDIO_DESC  49

#define OSD_BLOCK_SOUND_MODE_LEFT_ARROW 59
#define OSD_BLOCK_SOUND_MODE_RIGHT_ARROW 60
#define OSD_WINDOW_CHANNEL          2
#define OSD_INGREDIENT_CHANNEL_LEFT             4
#define OSD_INGREDIENT_CHANNEL_RIGHT            5
#define OSD_INGREDIENT_CHANNEL_TEXT_CHANNEL_MGMT 24
#define OSD_INGREDIENT_CHANNEL_TEXT_FAVORITE_MGMT 26
#define OSD_INGREDIENT_CHANNEL_TEXT_COUNTRY     59
#define OSD_INGREDIENT_CHANNEL_TEXT_TV_CONNECTION 63
#define OSD_INGREDIENT_CHANNEL_TEXT_AUTO_SEARCH 65
#define OSD_INGREDIENT_CHANNEL_TEXT_DTV_MANNUAL_SEARCH 67
#define OSD_INGREDIENT_CHANNEL_TEXT_ATV_MANNUAL_SEARCH 69

#define OSD_BLOCK_CHANNEL_LEFT_ARROW 78
#define OSD_BLOCK_CHANNEL_RIGHT_ARROW 79
#define OSD_WINDOW_FEATURE          3
#define OSD_INGREDIENT_FEATURE_LEFT             4
#define OSD_INGREDIENT_FEATURE_RIGHT            5
#define OSD_INGREDIENT_FEATURE_TEXT_LOCK        26
#define OSD_INGREDIENT_FEATURE_TEXT_LANGUAGE    28
#define OSD_INGREDIENT_FEATURE_TEXT_CLOCK       30
#define OSD_INGREDIENT_FEATURE_TEXT_SLEEP_TIMER 40
#define OSD_INGREDIENT_FEATURE_TEXT_AUTO_POWER_OFF 45
#define OSD_INGREDIENT_FEATURE_TEXT_HEARING_IMPAIRED 48
#define OSD_INGREDIENT_FEATURE_TEXT_CI_INFO     50
#define OSD_INGREDIENT_FEATURE_TEXT_PVR         52

#define OSD_BLOCK_FEATURE_LEFT_ARROW 62
#define OSD_BLOCK_FEATURE_RIGHT_ARROW 63
#define OSD_WINDOW_SETUP            4
#define OSD_INGREDIENT_SETUP_LEFT             4
#define OSD_INGREDIENT_SETUP_RIGHT            5
#define OSD_INGREDIENT_SETUP_TEXT_OSD_TIMER   32
#define OSD_INGREDIENT_SETUP_TEXT_AUTO_SCART  35
#define OSD_INGREDIENT_SETUP_TEXT_PC_SETUP    37
#define OSD_INGREDIENT_SETUP_TEXT_BLUE_SCREEN 40
#define OSD_INGREDIENT_SETUP_TEXT_RESET_TV    42
#define OSD_INGREDIENT_SETUP_TEXT_SOFTWARE_UPGRADE 44
#define OSD_INGREDIENT_SETUP_TEXT_OTA_UPGRADE 46
#define OSD_INGREDIENT_SETUP_TEXT_VERSION     48

#define OSD_BLOCK_SETUP_LEFT_ARROW 58
#define OSD_BLOCK_SETUP_RIGHT_ARROW 59
#define OSD_WINDOW_LEFT             5
#define OSD_INGREDIENT_LEFT_ICON_LOGO        3
#define OSD_INGREDIENT_LEFT_LABEL_MENU_PICTURE 8
#define OSD_INGREDIENT_LEFT_LABEL_MENU_SOUND 10
#define OSD_INGREDIENT_LEFT_LABEL_MENU_CHANNEL 12
#define OSD_INGREDIENT_LEFT_LABEL_MENU_FEATURE 14
#define OSD_INGREDIENT_LEFT_LABEL_MENU_SETUP 16

#define OSD_WINDOW_TOP              6
#define OSD_INGREDIENT_TOP_LABEL_TITLE      8

#define OSD_WINDOW_BOTTOM           7


#endif


#endif