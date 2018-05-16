#include "osd_scene.h"
#include "osd_common.h"
#include "osd_binary.h"
#include "osd_window.h"
#include "osd_proc.h"

struct _osd_scene_priv {
    osd_scene_hw *hw;
    osd_proc *proc;
    osd_binary *binary;
    osd_window *focused_window;
    osd_window **windows;
};

static int osd_scene_trigger(osd_scene *self, osd_event_type type,
                             osd_event_data *data) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    if (priv->proc) {
        TV_ASSERT(priv->proc->event);
        return priv->proc->event(priv->proc, type, data);
    }
    return 0;
}

static int osd_scene_timer_interval(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->timer_ms;
}

static void osd_scene_dump(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);
    TV_LOGI("scene\n\tname:%s, width:%d, height:%d, target_address:%#x\n",
            scene->hw->title,
            scene->hw->width, scene->hw->height, scene->hw->target_address);
}

static osd_window* osd_scene_window(osd_scene *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(index < priv->hw->window_count);
    return priv->windows[index];
}

static const char * osd_scene_title(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->title;
}

static u16 osd_scene_timer_ms(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->timer_ms;
}

static void osd_scene_paint(osd_scene *self,
                            u32 *framebuffer) {
    s32 y, i;
    u16 width, height;
    int painted = 0;
    u32 *line_buffer, *window_line_buffer;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(framebuffer);

    width = priv->hw->width;
    height = priv->hw->height;

    line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    window_line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    for (y = 0; y < height; y ++) {
        memset(line_buffer, 0, sizeof(u32) * width);
        for (i = 0; i < priv->hw->window_count; i ++) {
            osd_window *window = priv->windows[i];
            osd_rect rect;
            if (!window->visible(window)) {
                continue;
            }
            rect = window->rect(window);

            if (rect.y <= y && y < rect.y + (s32)rect.height) {
                u16 len = TV_MIN((s32)priv->hw->width - rect.x, (s32)rect.width);
                memset(window_line_buffer, 0, sizeof(u32) * width);
                if (window->paint(window, window_line_buffer, y)) {
                    osd_merge_line(line_buffer, window_line_buffer,
                                   len, rect.x, window->alpha(window));
                    painted = 1;
                }
            }
        }
        TV_MEMCPY(framebuffer + y * width, line_buffer, sizeof(u32) * width);
    }
    FREE_OBJECT(window_line_buffer);
    FREE_OBJECT(line_buffer);
}

osd_rect osd_scene_rect(osd_scene *self) {
    osd_rect rect;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    rect.x = rect.y = 0;
    rect.width = priv->hw->width;
    rect.height = priv->hw->height;
    return rect;
}

static u8* osd_scene_ram(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->binary->data(priv->binary, NULL);
}


static void osd_scene_set_proc(osd_scene *self, osd_proc *proc) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    priv->proc = proc;
    proc->init_ui(proc);
}

static void osd_scene_set_focused_window(osd_scene *self, osd_window *window) {
    osd_window *window_lost_focus;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    if (priv->focused_window == window) {
        return;
    }
    window_lost_focus = priv->focused_window;
    priv->focused_window = window;

    if (window_lost_focus) {
        window_lost_focus->send_message(window_lost_focus, OSD_EVENT_WINDOW_LEAVE, NULL);
    }
    if (window) {
        window->send_message(window, OSD_EVENT_WINDOW_ENTER, NULL);
    }
}
static osd_window* osd_scene_focused_window(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->focused_window;
}

static void osd_scene_destroy(osd_scene *self) {
    u32 i;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);

    for (i = 0; i < priv->hw->window_count; i ++) {
        osd_window *window = priv->windows[i];
        window->destroy(window);
    }
    priv->binary->destroy(priv->binary);

    FREE_OBJECT(priv->windows);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_scene *osd_scene_create(const char *osd_file, osd_proc *proc) {
    u32 count, i, ram_offset;
    osd_binary *binary;

    u8 *ram;
    osd_scene *self = MALLOC_OBJECT(osd_scene);
    osd_scene_priv *priv = MALLOC_OBJECT(osd_scene_priv);
    self->priv = priv;
    priv->proc = proc;
    self->destroy = osd_scene_destroy;
    self->paint = osd_scene_paint;
    self->window = osd_scene_window;
    self->title = osd_scene_title;
    self->timer_interval = osd_scene_timer_interval;
    self->trigger = osd_scene_trigger;
    self->rect = osd_scene_rect;
    self->ram = osd_scene_ram;
    self->set_proc = osd_scene_set_proc;
    self->set_focused_window = osd_scene_set_focused_window;
    self->focused_window = osd_scene_focused_window;
    self->dump = osd_scene_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);

    TV_ASSERT(osd_file);

    TV_LOGI("OSD_SCENE_HEADER_SIZE:%d\n", OSD_SCENE_HEADER_SIZE);
    TV_ASSERT(OSD_SCENE_HEADER_SIZE == 8 * sizeof(u32));

    TV_LOGI("OSD_GLYPH_HEADER_SIZE:%d\n", OSD_GLYPH_HEADER_SIZE);
    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == 4 * sizeof(u32));

    TV_LOGI("OSD_INGREDIENT_HEADER_SIZE:%d\n", OSD_INGREDIENT_HEADER_SIZE);
    TV_ASSERT(OSD_INGREDIENT_HEADER_SIZE == 4 * sizeof(u32));

    TV_LOGI("OSD_PALETTE_HEADER_SIZE:%d\n", OSD_PALETTE_HEADER_SIZE);
    TV_ASSERT(OSD_PALETTE_HEADER_SIZE == 2 * sizeof(u32));

    TV_LOGI("OSD_WINDOW_HEADER_SIZE:%d\n", OSD_WINDOW_HEADER_SIZE);
    TV_ASSERT(OSD_WINDOW_HEADER_SIZE == 9 * sizeof(u32));

    binary = osd_binary_create(osd_file);
    priv->binary = binary;
    ram = binary->data(binary, NULL);

    priv->hw = (osd_scene_hw *)ram;

    OSD_OBJECT_DUMP(self);

    ram_offset = OSD_SCENE_HEADER_SIZE;

    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == priv->hw->glyph_header_size);
    TV_ASSERT(OSD_INGREDIENT_HEADER_SIZE == priv->hw->ingredient_header_size);
    TV_ASSERT(OSD_WINDOW_HEADER_SIZE == priv->hw->window_header_size);
    TV_ASSERT(OSD_PALETTE_HEADER_SIZE == priv->hw->palette_header_size);


    //load windows
    count = priv->hw->window_count;
    priv->windows = MALLOC_OBJECT_ARRAY(osd_window*, count);
    for (i = 0; i < count; i ++) {
        osd_window_hw *hw = (osd_window_hw *)(ram + ram_offset);
        priv->windows[i] = osd_window_create(self, hw);
        ram_offset += OSD_WINDOW_HEADER_SIZE;
    }

    return self;
}


