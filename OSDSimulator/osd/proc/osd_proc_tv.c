#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "../../../Debug/tv.h"

typedef struct _osd_proc_tv_priv osd_proc_tv_priv;
struct _osd_proc_tv_priv {
    osd_scene *scene;
};

static int osd_proc_tv_timer(osd_proc *self) {
    return 0;
}


static int col = 3, row = 0;
static int max_col = 4, max_row = 1;
static int x_offset = 20, y_offset = 120;

static int osd_proc_tv_keydown(osd_proc *self, osd_key key) {
    static int old_col, old_row;
    osd_scene *scene;
    osd_window *window_hl;
    osd_window *window_bottom_0_0;
    osd_window *window_bottom_0_3;

    TV_TYPE_GET_PRIV(osd_proc_tv_priv, self, priv);

    old_col = col;
    old_row = row;
    scene = priv->scene;
    window_hl = scene->window(scene, OSD_WINDOW_ITEM_HIGHLIGHT);
    switch (key) {
    case OSD_KEY_UP:
        row = OSD_MAX(0, row - 1);
        break;
    case OSD_KEY_DOWN:
        row = OSD_MIN(max_row, row + 1);
        break;
    case OSD_KEY_LEFT:
        col = OSD_MAX(0, col - 1);
        break;
    case OSD_KEY_RIGHT:
        col = OSD_MIN(max_col, col + 1);
        break;
    }
    if (old_col != col || old_row != row) {
        osd_rect rect;
        rect.width = rect.height = 152;
        rect.x = x_offset + col * 152;
        rect.y = y_offset + row * 152;
        window_hl->set_rect(window_hl, &rect);

        window_bottom_0_0 = scene->window(scene, OSD_WINDOW_BOTTOM_0_0);
        window_bottom_0_3 = scene->window(scene, OSD_WINDOW_BOTTOM_0_3);
        if (row == 0 && col == 0) {
            window_bottom_0_3->set_visible(window_bottom_0_3, 0);
            window_bottom_0_0->set_visible(window_bottom_0_0, 1);
        } else if (row == 0 && col == 3) {
            window_bottom_0_3->set_visible(window_bottom_0_3, 1);
            window_bottom_0_0->set_visible(window_bottom_0_0, 0);
        } else {
            window_bottom_0_3->set_visible(window_bottom_0_3, 0);
            window_bottom_0_0->set_visible(window_bottom_0_0, 0);
        }


        return 1;
    } else {
        return 0;
    }
}


static int osd_proc_tv_event(osd_proc *self,
                             osd_trigger_type type,
                             osd_trigger_data *data) {
    switch (type) {
    case OSD_TRIGGER_TIMER:
        return osd_proc_tv_timer(self);
    case OSD_TRIGGER_KEYDOWN:
        return osd_proc_tv_keydown(self, data->data.keydown.key);
    default:
        return 0;
    }
}

static void osd_proc_tv_destroy(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_tv_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_proc *osd_proc_tv_create(osd_scene *scene) {
    osd_proc *self = MALLOC_OBJECT(osd_proc);
    osd_proc_tv_priv *priv = MALLOC_OBJECT(osd_proc_tv_priv);
    self->priv = priv;
    priv->scene = scene;
    self->destroy = osd_proc_tv_destroy;
    self->event = osd_proc_tv_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
