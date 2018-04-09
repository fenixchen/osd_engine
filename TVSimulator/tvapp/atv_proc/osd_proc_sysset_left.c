#include "osd_scene.h"
#include "osd_label.h"
#include "osd_proc.h"
#include "osd_bitmap.h"
#include "osd_proc_sysset.h"

#define OSD_ENABLE_MACROS_SYSTEM_SETTINGS
#include "../../../atv/system_settings.h"

static u32 menu_item[] = {
    OSD_INGREDIENT_LABEL_MENU_PICTURE,
    OSD_INGREDIENT_LABEL_MENU_SOUND,
    OSD_INGREDIENT_LABEL_MENU_CHANNEL,
    OSD_INGREDIENT_LABEL_MENU_FEATURE,
    OSD_INGREDIENT_LABEL_MENU_SETUP
};

static u32 center_window_id[] = {
    OSD_WINDOW_PICTURE_MODE,
    OSD_WINDOW_SOUND_MODE,
    OSD_WINDOW_CHANNEL,
    OSD_WINDOW_FEATURE,
    OSD_WINDOW_SETUP,
};

#define menu_item_count (sizeof(menu_item) / sizeof(menu_item[0]))

struct _window_left_data {
    int focused_item;
    osd_bitmap *bitmap_logo;
} left_data;

static int window_left_update_ui(osd_proc *self, osd_window *window_left, window_left_data *left_data,
                                 osd_event_type type, osd_event_data *data) {
    int i, focused;
    osd_label *label;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused = priv->scene->focused_window(priv->scene) == window_left;
    for (i = 0; i < menu_item_count; i ++) {
        int state;
        if (i == left_data->focused_item) {
            state = focused ? 2 : 1;
        } else {
            state = 0;
        }
        label = priv->scene->label(priv->scene, menu_item[i]);
        label->set_state(label, state);
    }
    label = priv->scene->label(priv->scene, OSD_INGREDIENT_LABEL_TITLE);
    label->set_text(label, left_data->focused_item);
    for (i = 0; i < CENTER_WINDOW_MAX; i ++) {
        osd_window *window = priv->window_center[i];
        if (i == left_data->focused_item) {
            osd_window_show(window);
        } else {
            osd_window_hide(window);
        }
    }
    left_data->bitmap_logo->set_current(left_data->bitmap_logo, left_data->focused_item);
    return 1;
}

static int window_left_keydown(osd_proc *self, osd_window *window_left, window_left_data *left_data,
                               osd_event_type type, osd_event_data *data) {
    int focused_item;
    osd_key key = data->data.keydown.key;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused_item = left_data->focused_item;
    switch(key) {
    case OSD_KEY_UP:
        focused_item = TV_MAX(0, focused_item - 1);
        break;
    case OSD_KEY_DOWN:
        focused_item = TV_MIN(menu_item_count - 1, focused_item + 1);
        break;
    case OSD_KEY_ENTER:
    case OSD_KEY_RIGHT:
        priv->scene->set_focused_window(priv->scene, priv->window_picture_mode);
        return 1;
    default:
        return 0;
    }
    if (focused_item != left_data->focused_item) {
        left_data->focused_item = focused_item;
        return window_left_update_ui(self, window_left, left_data, type, data);
    } else {
        return 0;
    }
}

int osd_window_left_proc(osd_window_proc *proc,
                         osd_event_type type, osd_event_data *data) {
    window_left_data *left_data = (window_left_data *)proc->priv;
    osd_proc *self = proc->proc;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
        left_data->focused_item = 0;
        left_data->bitmap_logo = priv->scene->bitmap(priv->scene, OSD_INGREDIENT_ICON_LOGO);
    case OSD_EVENT_WINDOW_ENTER:
    case OSD_EVENT_WINDOW_LEAVE:
        return window_left_update_ui(self, proc->window, left_data, type, data);
    case OSD_EVENT_KEYDOWN:
        return window_left_keydown(self, proc->window, left_data, type, data);
    default:
        return 0;
    }
}