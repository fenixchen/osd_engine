#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_label.h"
#include "osd_window.h"

struct _osd_label_priv {
    u32 unused;
};


void osd_label_paint(osd_ingredient *self,
                     osd_window *window,
                     osd_block *block,
                     u32 *window_label_buffer,
                     u32 y) {
}


static u32 osd_label_start_y(osd_ingredient *self) {
    return 0;
}

static u32 osd_label_height(osd_ingredient *self, osd_window *window) {
    return 0;
}

static void osd_label_dump(osd_ingredient *ingredient) {
    OSD_LOG("Label\n");
}

static void osd_label_destroy(osd_ingredient *self) {
    osd_label *label_self = (osd_label *)self;
    FREE_OBJECT(label_self->priv);
    FREE_OBJECT(self);
}

osd_label *osd_label_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_label *self = MALLOC_OBJECT(osd_label);
    self->priv = MALLOC_OBJECT(osd_label_priv);

    self->parent.destroy = osd_label_destroy;
    self->parent.paint = osd_label_paint;
    self->parent.destroy = osd_label_destroy;
    self->parent.start_y = osd_label_start_y;
    self->parent.height = osd_label_height;
    self->parent.dump = osd_label_dump;
    return self;
}