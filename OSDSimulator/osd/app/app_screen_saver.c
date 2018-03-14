#include "osd_scene.h"
#include "osd_common.h"
#include "osd_app.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "../../../Debug/test.h"


typedef struct _osd_app_screensaver_priv osd_app_screensaver_priv;
struct _osd_app_screensaver_priv {
    osd_scene *scene;
};

static int x_dir, y_dir;
static int first = 1;
static int init = 15;
static int visible = 1;
static int counter = 0;

static int osd_app_screesaver_timer(osd_app *self) {
    osd_window *window;
    osd_ingredient *ingredient;
    osd_label *label;
    int x, y, max_x, max_y;
    osd_rect window_rect, scene_rect;
    osd_scene *scene;
    const int MOVE_STEP = 10;
    TV_TYPE_GET_PRIV(osd_app_screensaver_priv, self, priv);

    scene = priv->scene;
    window = scene->window(scene, OSD_WINDOW_MAIN);
    if (!window->visible(window)) {
        return 0;
    }
    if (first) {
        first = 0;
        x_dir = osd_get_rand_boolean() ? 1 : -1;
        y_dir = osd_get_rand_boolean() ? 1 : -1;
    }

    window_rect = window->rect(window);
    scene_rect = scene->rect(scene);
    max_x = scene_rect.width - window_rect.width - 1;
    max_y = scene_rect.height - window_rect.height - 1;

    x = OSD_MIN(OSD_MAX(0, (int)window_rect.x + MOVE_STEP * x_dir), max_x);
    y = OSD_MIN(OSD_MAX(0, (int)window_rect.y + MOVE_STEP * y_dir), max_y);
    window->move_to(window, x, y);

    if (x == 0 || x == max_x) {
        x_dir = -x_dir;
    }

    if (y == 0 || y == max_y) {
        y_dir = -y_dir;
    }
    if (--counter <= 0) {
        ingredient = scene->ingredient(scene, OSD_INGREDIENT_LABEL_CENTER_2);
        TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_LABEL);
        label = (osd_label *)ingredient;
        label->set_value(label, init--);
        if (init < 0) {
            window->set_visible(window, 0);
        }
        counter = 5;
    }
    return 1;
}

static int osd_app_screensaver_event(osd_app *self,
                                     osd_trigger_type type,
                                     osd_trigger_data *data) {
    switch (type) {
    case OSD_TRIGGER_TIMER:
        return osd_app_screesaver_timer(self);
    default:
        return 0;
    }
}

static void osd_app_screensaver_destroy(osd_app *self) {
    TV_TYPE_GET_PRIV(osd_app_screensaver_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_app *osd_app_screensaver_create(osd_scene *scene) {
    osd_app *self = MALLOC_OBJECT(osd_app);
    osd_app_screensaver_priv *priv = MALLOC_OBJECT(osd_app_screensaver_priv);
    self->priv = priv;
    priv->scene = scene;
    self->destroy = osd_app_screensaver_destroy;
    self->event = osd_app_screensaver_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
