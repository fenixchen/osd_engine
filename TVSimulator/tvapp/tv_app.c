#include "tv_app.h"
#include "osd_scene.h"
#include "osd_proc.h"
#include "../../scenes/screensaver.h"
#include "../../scenes/tv.h"
#include "../../scenes/animation.h"
#include "../../atv/system_settings.h"
#include "../../atv/no_signal.h"

struct _tv_app_priv {
    osd_scene *scene;
    osd_proc *proc;
};

extern osd_proc *osd_proc_screensaver_create(tv_app *app, osd_scene *scene);
extern osd_proc *osd_proc_tv_create(tv_app *app, osd_scene *scene);
extern osd_proc *osd_proc_animation_create(tv_app *app, osd_scene *scene);
extern osd_proc *osd_proc_system_settings_create(tv_app *app, osd_scene *scene);
extern osd_proc *osd_proc_no_signal_create(tv_app *app, osd_scene *scene);

struct tv_scene_proc {
    const char *title;
    const char *binary;
    osd_proc* (*proc_create)(tv_app *app, osd_scene *scene);
} scene_procs[] = {
    {
        OSD_SCENE_SCREENSAVER_TITLE,
        OSD_SCENE_SCREENSAVER_BINARY,
        osd_proc_screensaver_create,
    },
    {
        OSD_SCENE_TV_TITLE,
        OSD_SCENE_TV_BINARY,
        osd_proc_tv_create,
    },
    {
        OSD_SCENE_ANIMATION_TITLE,
        OSD_SCENE_ANIMATION_BINARY,
        osd_proc_animation_create,
    },
    {
        OSD_SCENE_SYSTEM_SETTINGS_TITLE,
        OSD_SCENE_SYSTEM_SETTINGS_BINARY,
        osd_proc_system_settings_create,
    },
    {
        OSD_SCENE_NO_SIGNAL_TITLE,
        OSD_SCENE_NO_SIGNAL_BINARY,
        osd_proc_no_signal_create,
    },
    {
        NULL, NULL, NULL,
    },
};

static void tv_app_destroy(tv_app *self) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    if (priv->scene) {
        priv->scene->destroy(priv->scene);
    }
    if (priv->proc) {
        priv->proc->destroy(priv->proc);
    }
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void tv_app_paint(tv_app *self, u32 *framebuffer) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    if (priv->scene) {
        priv->scene->paint(priv->scene, framebuffer);
    }
}

static int tv_app_trigger(tv_app *self, osd_event_type type, osd_event_data *data) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    if (priv->scene) {
        return priv->scene->trigger(priv->scene, type, data);
    } else {
        return 0;
    }
}

static osd_scene* tv_app_active_scene(tv_app *self) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    return priv->scene;
}

static osd_rect tv_app_rect(tv_app *self) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    TV_ASSERT(priv->scene);
    return priv->scene->rect(priv->scene);
}

static int tv_app_load(tv_app *self, const char *osd_file) {
    const char *title;
    struct tv_scene_proc *scene_proc;
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    TV_TYPE_FREE(priv->scene);
    TV_TYPE_FREE(priv->proc);
    TV_ASSERT(osd_file);

    priv->scene = osd_scene_create(osd_file, NULL);
    if (!priv->scene) {
        return 0;
    }
    title = priv->scene->title(priv->scene);

    scene_proc = scene_procs;
    while (scene_proc->title && scene_proc->proc_create) {
        if (stricmp(title, scene_proc->title) == 0) {
            priv->proc = scene_proc->proc_create(self, priv->scene);
            priv->scene->set_proc(priv->scene, priv->proc);
        }
        scene_proc ++;
    }
    return 1;
}


static const char* tv_app_title(tv_app *self) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    TV_ASSERT(priv->scene);
    return priv->scene->title(priv->scene);
}

static int tv_app_timer_interval(tv_app *self) {
    TV_TYPE_GET_PRIV(tv_app_priv, self, priv);
    TV_ASSERT(priv->scene);
    return priv->scene->timer_interval(priv->scene);
}

tv_app* tv_app_create(void) {
    tv_app *self = MALLOC_OBJECT(tv_app);
    tv_app_priv *priv = MALLOC_OBJECT(tv_app_priv);
    self->priv = priv;
    self->destroy = tv_app_destroy;
    self->paint = tv_app_paint;
    self->trigger = tv_app_trigger;
    self->active_scene = tv_app_active_scene;
    self->rect = tv_app_rect;
    self->title = tv_app_title;
    self->timer_interval = tv_app_timer_interval;
    self->load = tv_app_load;
    TV_TYPE_FP_CHECK(self->destroy, self->load);
    priv->proc = NULL;
    priv->scene = NULL;

    return self;
}