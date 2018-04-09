#include "osd_scene.h"
#include "osd_block.h"
#include "osd_label.h"
#include "osd_proc.h"
#include "osd_proc_sysset.h"

#define OSD_ENABLE_MACROS_SYSTEM_SETTINGS
#include "../../../atv/system_settings.h"

#include "hal.h"

static u32 picture_menu_item[] = {
    OSD_INGREDIENT_TEXT_PICTURE_MODE,
    OSD_INGREDIENT_TEXT_BRIGHTNESS,
    OSD_INGREDIENT_TEXT_CONTRAST,
    OSD_INGREDIENT_TEXT_SHARPNESS,
    OSD_INGREDIENT_TEXT_COLOR,
    OSD_INGREDIENT_TEXT_TINT,
    OSD_INGREDIENT_TEXT_COLOR_TEMP,
    OSD_INGREDIENT_TEXT_ASPECT_RATIO,
};

enum picture_param {
    ITEM_PICTURE_MODE = 0,
    ITEM_BRIGHTNESS,
    ITEM_CONTRAST,
    ITEM_SHARPNESS,
    ITEM_COLOR,
    ITEM_TINT,
    ITEM_COLOR_TEMP,
    ITEM_ASPECT_RATIO,
};

enum picture_menu_state {
    STATE_NORMAL = 0,
    STATE_READONLY = 1,
    STATE_FOCUSED = 2,
};

#define picture_menu_item_count (sizeof(picture_menu_item) / sizeof(picture_menu_item[0]))

struct _window_picture_mode_data {
    osd_block *block_left, *block_right;
    int usermode;
    int focused_item;
    int picture_mode;
    int brightness;
    int contrast;
    int sharpness;
    int color;
    int tint;
    int color_temp;
    int aspect_ratio;
} picture_mode_data;

static int window_picture_mode_update_ui(osd_proc *self,
        osd_window *window_picture_mode,
        window_picture_mode_data *picture_mode_data) {
    int i, focused;
    osd_label *label;
    osd_point point_arrow;

    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused = priv->scene->focused_window(priv->scene) == window_picture_mode;
    for (i = 0; i < picture_menu_item_count; i ++) {
        int state;
        if (i == ITEM_PICTURE_MODE || i == ITEM_COLOR_TEMP) {
            if (i == picture_mode_data->focused_item) {
                state = focused ? STATE_FOCUSED : STATE_NORMAL;
            } else {
                state = STATE_NORMAL;
            }
        } else {
            if (picture_mode_data->usermode) {
                if (i == picture_mode_data->focused_item) {
                    state = focused ? STATE_FOCUSED : STATE_NORMAL;
                } else {
                    state = STATE_NORMAL;
                }
            } else {
                state = STATE_READONLY;
            }
        }
        label = priv->scene->label(priv->scene, picture_menu_item[i]);
        label->set_state(label, state);
    }
    point_arrow = picture_mode_data->block_left->position(picture_mode_data->block_left);
    point_arrow.y = 60 + 48 * picture_mode_data->focused_item;
    picture_mode_data->block_left->set_position(picture_mode_data->block_left, &point_arrow);

    point_arrow = picture_mode_data->block_right->position(picture_mode_data->block_right);
    point_arrow.y = 60 + 48 * picture_mode_data->focused_item;
    picture_mode_data->block_right->set_position(picture_mode_data->block_right, &point_arrow);

    return 1;
}

struct picture_pre_defined {
    int brighness;
    int contrast;
    int sharpness;
    int color;
    int tint;
} picture_pre_defined_data[] = {
    { 50, 50, 50, 50, 50,}, //STANDARD
    { 55, 55, 50, 55, 50,},	//DYNAMIC
    { 45, 45, 50, 45, 50,},	//MILD
    { 50, 50, 50, 50, 50,},	//USER
};


static void picure_hal_set(osd_label *label, int *pval,
                           int value, int lval, int uval,
                           HAL_RESULT (*hal_func)(int)) {
    value = TV_CLIP(value, lval, uval);
    *pval = value;
    label->set_int(label, value);
    hal_func(value);
}


static void window_picture_mode_load_ui(osd_proc_system_settings_priv *priv,
                                        window_picture_mode_data *picture_mode_data) {
    osd_label *label;
    struct picture_pre_defined *predefined = &picture_pre_defined_data[picture_mode_data->picture_mode];
    label = priv->scene->label(priv->scene, OSD_INGREDIENT_TEXT_BRIGHTNESS);
    picure_hal_set(label,
                   &picture_mode_data->brightness,
                   predefined->brighness,
                   0,
                   99,
                   hal_brightness_set);
    label = priv->scene->label(priv->scene, OSD_INGREDIENT_TEXT_CONTRAST);
    picure_hal_set(label,
                   &picture_mode_data->contrast,
                   predefined->contrast,
                   0,
                   99,
                   hal_contrast_set);

    label = priv->scene->label(priv->scene, OSD_INGREDIENT_TEXT_SHARPNESS);
    picure_hal_set(label,
                   &picture_mode_data->sharpness,
                   predefined->sharpness,
                   0,
                   99,
                   hal_sharpness_set);

    label = priv->scene->label(priv->scene, OSD_INGREDIENT_TEXT_COLOR);
    picure_hal_set(label,
                   &picture_mode_data->color,
                   predefined->color,
                   0,
                   99,
                   hal_color_set);

    label = priv->scene->label(priv->scene, OSD_INGREDIENT_TEXT_TINT);
    picure_hal_set(label,
                   &picture_mode_data->tint,
                   predefined->tint,
                   0,
                   99,
                   hal_tint_set);
}
static int window_picture_mode_change_ui(osd_proc *self,
        osd_window *window_picture_mode,
        window_picture_mode_data *picture_mode_data,
        int direction) {
    osd_label *label;
    int focused_item = picture_mode_data->focused_item;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    TV_ASSERT(direction != 0);
    label = priv->scene->label(priv->scene, picture_menu_item[focused_item]);
    switch (focused_item) {
    case ITEM_PICTURE_MODE:
        picture_mode_data->picture_mode = TV_CLIP(picture_mode_data->picture_mode + direction, 0, 3);
        label->set_text(label, picture_mode_data->picture_mode);
        picture_mode_data->usermode = picture_mode_data->picture_mode == 3;
        window_picture_mode_load_ui(priv, picture_mode_data);
        window_picture_mode_update_ui(self, window_picture_mode, picture_mode_data);
        break;
    case ITEM_BRIGHTNESS:
        picure_hal_set(label,
                       &picture_mode_data->brightness,
                       picture_mode_data->brightness + direction,
                       0,
                       99,
                       hal_brightness_set);
        break;
    case ITEM_CONTRAST:
        picure_hal_set(label,
                       &picture_mode_data->contrast,
                       picture_mode_data->contrast + direction,
                       0,
                       99,
                       hal_contrast_set);
        break;
    case ITEM_SHARPNESS:
        picure_hal_set(label,
                       &picture_mode_data->sharpness,
                       picture_mode_data->sharpness + direction,
                       0,
                       99,
                       hal_sharpness_set);
        break;
    case ITEM_COLOR:
        picure_hal_set(label,
                       &picture_mode_data->color,
                       picture_mode_data->color + direction,
                       0,
                       99,
                       hal_color_set);
        break;
    case ITEM_TINT:
        picure_hal_set(label,
                       &picture_mode_data->tint,
                       picture_mode_data->tint + direction,
                       0,
                       99,
                       hal_tint_set);
        break;
    case ITEM_COLOR_TEMP:
        picture_mode_data->color_temp = TV_CLIP(picture_mode_data->color_temp + direction, 0, 2);
        label->set_text(label, picture_mode_data->color_temp);
        hal_color_temperature_set(picture_mode_data->color_temp);
        break;
    case ITEM_ASPECT_RATIO:
        picture_mode_data->aspect_ratio = TV_CLIP(picture_mode_data->aspect_ratio + direction, 0, 1);
        label->set_text(label, picture_mode_data->aspect_ratio);
        hal_aspect_ratio_set(picture_mode_data->aspect_ratio);
        break;
    default:
        return 0;
    }
    return 1;
}

static int window_picture_mode_keydown(osd_proc *self,
                                       osd_window *window_picture_mode,
                                       window_picture_mode_data *picture_mode_data,
                                       osd_event_type type,
                                       osd_event_data *data) {
    int focused_item;
    osd_key key = data->data.keydown.key;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused_item = picture_mode_data->focused_item;
    switch(key) {
    case OSD_KEY_UP:
        if (picture_mode_data->usermode) {
            focused_item = TV_MAX(0, focused_item - 1);
        } else {
            if (focused_item == ITEM_COLOR_TEMP) {
                focused_item = ITEM_PICTURE_MODE;
            }
        }
        break;
    case OSD_KEY_DOWN:
        if (picture_mode_data->usermode) {
            focused_item = TV_MIN(picture_menu_item_count - 1, focused_item + 1);
        } else {
            if (focused_item == ITEM_PICTURE_MODE) {
                focused_item = ITEM_COLOR_TEMP;
            }
        }
        break;
    case OSD_KEY_BACK:
        priv->scene->set_focused_window(priv->scene, priv->window_left);
        return 1;
    case OSD_KEY_LEFT:
        return window_picture_mode_change_ui(self, window_picture_mode, picture_mode_data, -1);
    case OSD_KEY_RIGHT:
        return window_picture_mode_change_ui(self, window_picture_mode, picture_mode_data, +1);
    default:
        return 0;
    }
    if (focused_item != picture_mode_data->focused_item) {
        picture_mode_data->focused_item = focused_item;
        return window_picture_mode_update_ui(self, window_picture_mode, picture_mode_data);
    } else {
        return 0;
    }
}

int osd_window_picture_mode_proc(osd_window_proc *proc,
                                 osd_event_type type,
                                 osd_event_data *data) {
    window_picture_mode_data *picture_mode_data = (window_picture_mode_data *)proc->priv;
    osd_proc *self = proc->proc;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
        picture_mode_data->block_left = priv->scene->block(priv->scene, OSD_BLOCK_PICTURE_MODE_LEFT_ARROW);
        picture_mode_data->block_right = priv->scene->block(priv->scene, OSD_BLOCK_PICTURE_MODE_RIGHT_ARROW);
        picture_mode_data->picture_mode = 0;
        picture_mode_data->brightness = 50;
        picture_mode_data->contrast = 50;
        picture_mode_data->sharpness = 50;
        picture_mode_data->color = 50;
        picture_mode_data->tint = 50;
        picture_mode_data->color_temp = 0;
        picture_mode_data->aspect_ratio = 50;
        picture_mode_data->focused_item = 0;
        picture_mode_data->usermode = 0;
    case OSD_EVENT_WINDOW_ENTER:
    case OSD_EVENT_WINDOW_LEAVE:
        return window_picture_mode_update_ui(self, proc->window, picture_mode_data);
    case OSD_EVENT_KEYDOWN:
        return window_picture_mode_keydown(self, proc->window, picture_mode_data, type, data);
    default:
        return 0;
    }
}