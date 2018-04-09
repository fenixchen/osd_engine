#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "osd_bitmap.h"

#define OSD_ENABLE_MACROS_ANIMATION
#include "../../../scenes/animation.h"

typedef struct _osd_proc_animation_priv osd_proc_animation_priv;

struct _osd_proc_animation_priv {
    osd_scene *scene;
    osd_bitmap *bitmap;
};

static int osd_proc_animation_timer(osd_proc *self) {
    osd_scene *scene;
    osd_bitmap *bitmap;
    int count, current;
    TV_TYPE_GET_PRIV(osd_proc_animation_priv, self, priv);
    if (!priv->bitmap) {
        scene = priv->scene;
        priv->bitmap = scene->bitmap(scene, OSD_INGREDIENT_BITMAP_ANI);
    }
    TV_ASSERT(priv->bitmap);
    bitmap = priv->bitmap;
    count = bitmap->count(bitmap);
    current = bitmap->current(bitmap);
    bitmap->set_current(bitmap, (current + 1) % count);

    return 1;
}


static int osd_proc_animation_keydown(osd_proc *self, osd_key key) {
    return 0;
}


static int osd_proc_animation_event(osd_proc *self,
                                    osd_event_type type,
                                    osd_event_data *data) {
    switch (type) {
    case OSD_EVENT_TIMER:
        return osd_proc_animation_timer(self);
    case OSD_EVENT_KEYDOWN:
        return osd_proc_animation_keydown(self, data->data.keydown.key);
    default:
        return 0;
    }
}

static void osd_proc_animation_init_ui(osd_proc *self) {

}

static void osd_proc_animation_destroy(osd_proc *self) {
    TV_TYPE_GET_PRIV(osd_proc_animation_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_proc *osd_proc_animation_create(tv_app *app, osd_scene *scene) {
    osd_proc *self = MALLOC_OBJECT(osd_proc);
    osd_proc_animation_priv *priv = MALLOC_OBJECT(osd_proc_animation_priv);
    self->priv = priv;
    priv->scene = scene;
    self->destroy = osd_proc_animation_destroy;
    self->init_ui = osd_proc_animation_init_ui;
    self->event = osd_proc_animation_event;
    TV_TYPE_FP_CHECK(self->destroy, self->event);
    return self;
}
