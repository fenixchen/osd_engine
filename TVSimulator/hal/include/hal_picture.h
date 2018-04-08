#ifndef _HAL_PICTURE_H
#define _HAL_PICTURE_H

#include "hal_types.h"

HAL_RESULT hal_brightness_set(int value);

HAL_RESULT hal_contrast_set(int value);

HAL_RESULT hal_sharpness_set(int value);

HAL_RESULT hal_color_set(int value);

HAL_RESULT hal_tint_set(int value);

HAL_RESULT hal_color_temperature_set(int value);

HAL_RESULT hal_aspect_ratio_set(int value);


#endif