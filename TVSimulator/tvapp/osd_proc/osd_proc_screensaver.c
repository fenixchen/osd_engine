#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "../../../scenes/screensaver.h"


typedef struct _osd_proc_screensaver_priv osd_proc_screensaver_priv;
struct _osd_proc_screensaver_priv {
    osd_scene *scene;
};

static int x_dir, y_dir;
static int first = 1;
static int init = 15;
static int visible = 1;
static int counter = 0;

static int osd_proc_screesaver_timer(osd_proc *self) {
    osd_window *window;
    osd_label *label;
    int x, y, max_x, max_y;
    osd_rect window_rect, scene_rect;
    osd_scene *scene;
    const int MOVE_STEP = 10;
    TV_TYPE_GET_PRIV(osd_proc_screensaver_priv, self, priv);

    scene = priv->scene;
    window = scene->window(scene, OSD_WINDOW_MAIN);
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
    if (--counter <= 0) {
        label = scene->label(scene, OSD_INGREDIENT_LABEL_CENTER_2);
        label->set_int(label, init--);
        if (init < 0) {
            window->set_visible(window, 0);
        }
        counter = 5;
    }
    return 1;
}

static int osd_proc_screensaver_event(osd_proc *self,
                                      osd_event_type type,
                                      osd_event_data *data) {
    switch (type) {
    case OSD_EVENT_TIMER:
        return osd_proc_screesaver_timer(self);
    default:
        return 0;
    }
}

static void osd_proc_screensaver_init_ui(osd_proc *self) {

}

static void osd_proc_screensaver_destroy(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_screensaver_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_proc *osd_proc_screensaver_create(osd_scene *scene) {
    osd_proc *self = MALLOC_OBJECT(osd_proc);
    osd_proc_screensaver_priv *priv = MALLOC_OBJECT(osd_proc_screensaver_priv);
    self->priv = priv;
    priv->scene = scene;
    self->destroy = osd_proc_screensaver_destroy;
    self->init_ui = osd_proc_screensaver_init_ui;
    self->event = osd_proc_screensaver_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
