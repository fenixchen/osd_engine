#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "../../../atv/system_settings.h"

typedef struct _osd_proc_system_settings_priv osd_proc_system_settings_priv;
struct _osd_proc_system_settings_priv {
    osd_scene *scene;
    int current_menu_item;
    int sub_menu_focused;
    int current_sub_menu_item;
};

static int osd_proc_system_settings_timer(osd_proc *self) {
    return 0;
}


static u32 menu_item[] = {
    OSD_INGREDIENT_LABEL_MENU_PICTURE,
    OSD_INGREDIENT_LABEL_MENU_SOUND,
    OSD_INGREDIENT_LABEL_MENU_CHANNEL,
    OSD_INGREDIENT_LABEL_MENU_FEATURE,
    OSD_INGREDIENT_LABEL_MENU_SETUP
};
#define menu_item_count (sizeof(menu_item) / sizeof(menu_item[0]))

static void show_menu_item_state(osd_proc *self) {
    int i;
    osd_label *label;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    for (i = 0; i < menu_item_count; i ++) {
        int state;
        if (i == priv->current_menu_item) {
            state = priv->sub_menu_focused ? 1 : 2;
        } else {
            state = 0;
        }
        label = priv->scene->label(priv->scene, menu_item[i]);
        label->set_state(label, state);
    }
    label = priv->scene->label(priv->scene, OSD_INGREDIENT_LABEL_TITLE);
    label->set_text(label, priv->current_menu_item);
}

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

#define picture_menu_item_count (sizeof(picture_menu_item) / sizeof(picture_menu_item[0]))


static void show_sub_menu_item_state(osd_proc *self) {
    int i;
    osd_label *label;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    for (i = 0; i < picture_menu_item_count; i ++) {
        int state;
        if (i == priv->current_sub_menu_item) {
            state = priv->sub_menu_focused ? 2 : 1;
        } else {
            state = 0;
        }
        label = priv->scene->label(priv->scene, picture_menu_item[i]);
        label->set_state(label, state);
    }
}

static int osd_proc_system_settings_keydown(osd_proc *self, osd_key key) {
    int result = 0;
    int menu_focus_changed = 0;
    int current_menu_item, current_sub_menu_item, sub_menu_focused;
    osd_scene *scene;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    scene = priv->scene;
    current_menu_item = priv->current_menu_item;
    current_sub_menu_item = priv->current_sub_menu_item;
    sub_menu_focused = priv->sub_menu_focused;
    switch (key) {
    case OSD_KEY_UP:
        if (sub_menu_focused) {
            current_sub_menu_item = TV_MAX(0, current_sub_menu_item - 1);
        } else {
            current_menu_item = TV_MAX(0, current_menu_item - 1);
        }
        break;
    case OSD_KEY_DOWN:
        if (sub_menu_focused) {
            current_sub_menu_item = TV_MIN(picture_menu_item_count - 1, current_sub_menu_item + 1);
        } else {
            current_menu_item = TV_MIN(menu_item_count - 1, current_menu_item + 1);
        }
        break;
    case OSD_KEY_ENTER:
        sub_menu_focused = 1;
        break;
    case OSD_KEY_BACK:
        sub_menu_focused = 0;
        break;
    case OSD_KEY_LEFT:
        break;
    case OSD_KEY_RIGHT:
        break;
    default:
        return 0;
    }

    menu_focus_changed = sub_menu_focused != priv->sub_menu_focused;
    if (menu_focus_changed) {
        current_menu_item = TV_CLIP(current_menu_item, 0, menu_item_count);
        current_sub_menu_item = TV_CLIP(current_sub_menu_item, 0, picture_menu_item_count);
        priv->sub_menu_focused = sub_menu_focused;
    }

    if (current_menu_item != priv->current_menu_item || menu_focus_changed) {
        priv->current_menu_item = current_menu_item;
        show_menu_item_state(self);
        result = 1;
    }
    if (current_sub_menu_item != priv->current_sub_menu_item || menu_focus_changed) {
        priv->current_sub_menu_item = current_sub_menu_item;
        show_sub_menu_item_state(self);
        result = 1;
    }
    return result;
}

static int osd_proc_system_settings_event(osd_proc *self,
        osd_trigger_type type,
        osd_trigger_data *data) {
    switch (type) {
    case OSD_TRIGGER_TIMER:
        return osd_proc_system_settings_timer(self);
    case OSD_TRIGGER_KEYDOWN:
        return osd_proc_system_settings_keydown(self, data->data.keydown.key);
    default:
        return 0;
    }
}

static void osd_proc_system_settings_init_ui(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    priv->current_menu_item = 0;
    priv->sub_menu_focused = 0;
    priv->current_sub_menu_item = -1;
    show_menu_item_state(self);
    show_sub_menu_item_state(self);
}

static void osd_proc_system_settings_destroy(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_proc *osd_proc_system_settings_create(osd_scene *scene) {
    osd_proc *self = MALLOC_OBJECT(osd_proc);
    osd_proc_system_settings_priv *priv = MALLOC_OBJECT(osd_proc_system_settings_priv);
    self->priv = priv;
    priv->scene = scene;
    self->destroy = osd_proc_system_settings_destroy;
    self->init_ui = osd_proc_system_settings_init_ui;
    self->event = osd_proc_system_settings_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
