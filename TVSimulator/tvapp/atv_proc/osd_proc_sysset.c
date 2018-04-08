#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "osd_block.h"
#include "osd_proc_sysset.h"
#include "../../../atv/system_settings.h"

osd_window_proc window_proc_data[] = {
    {OSD_WINDOW_LEFT,   NULL, NULL, &left_data,		osd_window_left_proc,	},
    {OSD_WINDOW_PICTURE_MODE, NULL, NULL, &picture_mode_data,	osd_window_picture_mode_proc,	},
};

#define window_data_count (sizeof(window_proc_data) / sizeof(window_proc_data[0]))

static int osd_proc_system_settings_event(osd_proc *self,
        osd_event_type type,
        osd_event_data *data) {
    osd_window *window;
    TV_TYPE_GET_PRIV(osd_proc_system_settings_priv, self, priv);
    switch (type) {
    case OSD_EVENT_WINDOW_INIT:
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

    priv->window_left = scene->window(scene, OSD_WINDOW_LEFT);
    priv->window_picture_mode = scene->window(scene, OSD_WINDOW_PICTURE_MODE);
    return self;
}

