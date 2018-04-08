#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

#include "tv_api.h"

typedef int HAL_RESULT;

#define HAL_OK 0

#define HAL_FAIL -1

#define HAL_SUCCEEDED(result) (result == HAL_OK)

#define HAL_FAILED(result) (result != HAL_OK)

#endif