#include "osd_scene.h"
#include "osd_common.h"
#include "osd_proc.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_label.h"
#include "../../../scenes/tv.h"

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
static t_wchar *string_big[] = {
    L"快速指南",
    L"连接",
    L"遥控器和外围设备",
    L"智能功能",
    L"电视观看",
    L"图像和声音",
    L"系统和支持",
    L"故障排除",
    L"注意事项和说明",
    L"使用说明书",
};
static int osd_proc_tv_keydown(osd_proc *self, osd_key key) {
    static int old_col, old_row;
    int msgbox_visible;
    osd_scene *scene;
    osd_window *window_hl, *window_msgbox;
    osd_label *label_big, *label_small;
    TV_TYPE_GET_PRIV(osd_proc_tv_priv, self, priv);
    old_col = col;
    old_row = row;
    scene = priv->scene;

    window_hl = scene->window(scene, OSD_WINDOW_ITEM_HIGHLIGHT);

    window_msgbox = scene->window(scene, OSD_WINDOW_MESSAGEBOX);
    msgbox_visible = window_msgbox->visible(window_msgbox);
    if (key == OSD_KEY_ENTER) {
        window_msgbox->set_visible(window_msgbox, !msgbox_visible);
        return 1;
    }

    if (msgbox_visible) return 0;

    switch (key) {
    case OSD_KEY_UP:
        row = TV_MAX(0, row - 1);
        break;
    case OSD_KEY_DOWN:
        row = TV_MIN(max_row, row + 1);
        break;
    case OSD_KEY_LEFT:
        col = TV_MAX(0, col - 1);
        break;
    case OSD_KEY_RIGHT:
        col = TV_MIN(max_col, col + 1);
        break;
    default:
        TV_LOG("KEY %d pressed\n", key);
        return 0;
    }
    window_hl = scene->window(scene, OSD_WINDOW_ITEM_HIGHLIGHT);
    if (old_col != col || old_row != row) {
        osd_rect rect;
        rect.width = rect.height = 152;
        rect.x = x_offset + col * 152;
        rect.y = y_offset + row * 152;
        window_hl->set_rect(window_hl, &rect);

        label_big = (osd_label*)scene->ingredient(scene, OSD_INGREDIENT_LABEL_BOTTOM_BIG);
        label_small = (osd_label*)scene->ingredient(scene, OSD_INGREDIENT_LABEL_BOTTOM_SMALL);

        label_big->set_string(label_big, string_big[row * 5 + col]);

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
