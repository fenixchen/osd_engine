#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"

#define OSD_ENABLE_MACROS_NO_SIGNAL
#include "../../../atv/no_signal.h"

#include "../../../atv/system_settings.h"

#include "tv_app.h"

typedef struct _osd_proc_no_signal_priv osd_proc_no_signal_priv;
struct _osd_proc_no_signal_priv {
    osd_scene *scene;
    tv_app *app;
};

static int x_dir, y_dir;
static int first = 1;

static int osd_proc_no_signal_timer(osd_proc *self) {
    osd_window *window;
    int x, y, max_x, max_y;
    osd_rect window_rect, scene_rect;
    osd_scene *scene;
    const int MOVE_STEP = 10;
    TV_TYPE_GET_PRIV(osd_proc_no_signal_priv, self, priv);

    scene = priv->scene;
    window = scene->window(scene, OSD_WINDOW_NO_SIGNAL);
    if (!window->visible(window)) {
        return 0;
    }
    if (first) {
        first = 0;
        x_dir = tv_get_rand_boolean() ? 1 : -1;
        y_dir = tv_get_rand_boolean() ? 1 : -1;
    }

    window_rect = window->rect(window);
    scene_rect = scene->rect(scene);
    max_x = scene_rect.width - window_rect.width - 1;
    max_y = scene_rect.height - window_rect.height - 1;

    x = TV_MIN(TV_MAX(0, (int)window_rect.x + MOVE_STEP * x_dir), max_x);
    y = TV_MIN(TV_MAX(0, (int)window_rect.y + MOVE_STEP * y_dir), max_y);
    window->move_to(window, x, y);

    if (x == 0 || x == max_x) {
        x_dir = -x_dir;
    }

    if (y == 0 || y == max_y) {
        y_dir = -y_dir;
    }
    return 1;
}

static int osd_proc_no_signal_keydown(osd_proc *self, osd_key key) {
    TV_TYPE_GET_PRIV(osd_proc_no_signal_priv, self, priv);
    switch (key) {
    case OSD_KEY_MENU:
        priv->app->load(priv->app, "../atv/"OSD_SCENE_SYSTEM_SETTINGS_BINARY);
        return 1;
    default:
        return 0;
    }
}

static int osd_proc_no_signal_event(osd_proc *self,
                                    osd_event_type type,
                                    osd_event_data *data) {
    switch (type) {
    case OSD_EVENT_TIMER:
        return osd_proc_no_signal_timer(self);
    case OSD_EVENT_KEYDOWN:
        return osd_proc_no_signal_keydown(self, data->data.keydown.key);
    default:
        return 0;
    }
}

static void osd_proc_no_signal_init_ui(osd_proc *self) {

}

static void osd_proc_no_signal_destroy(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_no_signal_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_proc *osd_proc_no_signal_create(tv_app *app, osd_scene *scene) {
    osd_proc *self = MALLOC_OBJECT(osd_proc);
    osd_proc_no_signal_priv *priv = MALLOC_OBJECT(osd_proc_no_signal_priv);
    self->priv = priv;
    priv->scene = scene;
    priv->app = app;
    self->destroy = osd_proc_no_signal_destroy;
    self->init_ui = osd_proc_no_signal_init_ui;
    self->event = osd_proc_no_signal_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
