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
};

static u32 menu_item[] = {
    OSD_INGREDIENT_LABEL_MENU_PICTURE,
    OSD_INGREDIENT_LABEL_MENU_SOUND,
    OSD_INGREDIENT_LABEL_MENU_CHANNEL,
    OSD_INGREDIENT_LABEL_MENU_FEATURE,
    OSD_INGREDIENT_LABEL_MENU_SETUP
};
#define menu_item_count (sizeof(menu_item) / sizeof(menu_item[0]))

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

struct window_left_data {
    int focused_item;
} left_data;

struct window_center_data {
    int focused_item;
} center_data;

static int window_left_update_ui(osd_proc *self, osd_window *window_left, struct window_left_data *left_data,
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
    return 1;
}

static int window_left_keydown(osd_proc *self, osd_window *window_left, struct window_left_data *left_data,
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
        priv->scene->set_focused_window(priv->scene, priv->scene->window(priv->scene, OSD_WINDOW_CENTER));
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

static int osd_window_left_proc(osd_window_proc *proc,
                                osd_event_type type, osd_event_data *data) {
    struct window_left_data *priv_data = (struct window_left_data *)proc->priv;
    osd_proc *self = proc->proc;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
        priv_data->focused_item = 0;
        return window_left_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_WINDOW_ENTER:
        return window_left_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_WINDOW_LEAVE:
        return window_left_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_KEYDOWN:
        return window_left_keydown(self, proc->window, priv_data, type, data);
    default:
        return 0;
    }
}


static int window_center_update_ui(osd_proc *self, osd_window *window_center, struct window_center_data *center_data,
                                   osd_event_type type, osd_event_data *data) {
    int i, focused;
    osd_label *label;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused = priv->scene->focused_window(priv->scene) == window_center;
    for (i = 0; i < picture_menu_item_count; i ++) {
        int state;
        if (i == center_data->focused_item) {
            state = focused ? 2 : 1;
        } else {
            state = 0;
        }
        label = priv->scene->label(priv->scene, picture_menu_item[i]);
        label->set_state(label, state);
    }
    return 1;
}

static int window_center_keydown(osd_proc *self, osd_window *window_center, struct window_center_data *center_data,
                                 osd_event_type type, osd_event_data *data) {
    int focused_item;
    osd_key key = data->data.keydown.key;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    focused_item = center_data->focused_item;
    switch(key) {
    case OSD_KEY_UP:
        focused_item = TV_MAX(0, focused_item - 1);
        break;
    case OSD_KEY_DOWN:
        focused_item = TV_MIN(picture_menu_item_count - 1, focused_item + 1);
        break;
    case OSD_KEY_BACK:
        priv->scene->set_focused_window(priv->scene, priv->scene->window(priv->scene, OSD_WINDOW_LEFT));
        return 1;
    default:
        return 0;
    }
    if (focused_item != center_data->focused_item) {
        center_data->focused_item = focused_item;
        return window_center_update_ui(self, window_center, center_data, type, data);
    } else {
        return 0;
    }
}

static int osd_window_center_proc(osd_window_proc *proc,
                                  osd_event_type type, osd_event_data *data) {
    struct window_center_data *priv_data = (struct window_center_data *)proc->priv;
    osd_proc *self = proc->proc;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
        priv_data->focused_item = 0;
        return window_center_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_WINDOW_ENTER:
        return window_center_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_WINDOW_LEAVE:
        return window_center_update_ui(self, proc->window, priv_data, type, data);
    case OSD_EVENT_KEYDOWN:
        return window_center_keydown(self, proc->window, priv_data, type, data);
    default:
        return 0;
    }
}


osd_window_proc window_proc_data[] = {
    {OSD_WINDOW_LEFT,   NULL, NULL, &left_data,		osd_window_left_proc,	},
    {OSD_WINDOW_CENTER, NULL, NULL, &center_data,	osd_window_center_proc,	},
};

#define window_data_count (sizeof(window_proc_data) / sizeof(window_proc_data[0]))



static int osd_proc_system_settings_event(osd_proc *self,
        osd_event_type type,
        osd_event_data *data) {
    osd_window *window;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
    case OSD_EVENT_WINDOW_DEINIT:
    case OSD_EVENT_WINDOW_ENTER:
    case OSD_EVENT_WINDOW_LEAVE: {
        window = data->data.window.window;
        return window->send_message(window, type, data);
    }
    case OSD_EVENT_KEYDOWN: {
        osd_window *window = priv->scene->focused_window(priv->scene);
        if (window) {
            return window->send_message(window, type, data);
        }
        break;
    }
    }
    return 0;
}



static void osd_proc_system_settings_init_ui(osd_proc *self) {
    int i;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);

    for (i = 0; i < window_data_count; i ++) {
        osd_window_proc* wp = &window_proc_data[i];
        osd_window *window = priv->scene->window(priv->scene, wp->window_id);
        wp->proc = self;
        wp->window = window;
        window->set_window_proc(window, wp);
        window->send_message(window, OSD_EVENT_WINDOW_INIT, NULL);
    }
    priv->scene->set_focused_window(priv->scene, window_proc_data[0].window);
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

