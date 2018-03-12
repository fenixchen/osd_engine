#include "osd_common.h"
#include "osd_binary.h"

#define GLOBAL_FILE		"global.bin"
#define PALETTES_FILE	"palettes.bin"
#define WINDOWS_FILE	"windows.bin"
#define INGREDIENT_FILE	"ingredients.bin"
#define MODIFIERS_FILE	"modifiers.bin"
#define RAM_FILE		"ram.bin"


struct _osd_binary_priv {
    u8 *global;
    u32 global_size;

    u8 *palettes;
    u32 palettes_size;

    u8 *ingredients;
    u32 ingredients_size;

    u8 *windows;
    u32 windows_size;

    u8 *ram;
    u32 ram_size;
};

static u32 osd_binary_data_size(osd_binary *self, binary_type type) {
    TV_TYPE_GET_PRIV(osd_binary_priv, self, priv);
    switch (type) {
    case BINARY_TYPE_GLOBAL:
        return priv->global_size;
    case BINARY_TYPE_PALETTE:
        return priv->palettes_size;
    case BINARY_TYPE_INGREDIENT:
        return priv->ingredients_size;
    case BINARY_TYPE_WINDOW:
        return priv->windows_size;
    case BINARY_TYPE_RAM:
        return priv->ram_size;
    default:
        TV_ASSERT(0);
    }
    return 0;
}

static u8* osd_binary_data(osd_binary *self, binary_type type) {
    TV_TYPE_GET_PRIV(osd_binary_priv, self, priv);
    switch (type) {
    case BINARY_TYPE_GLOBAL:
        return priv->global;
    case BINARY_TYPE_PALETTE:
        return priv->palettes;
    case BINARY_TYPE_INGREDIENT:
        return priv->ingredients;
    case BINARY_TYPE_WINDOW:
        return priv->windows;
    case BINARY_TYPE_RAM:
        return priv->ram;
    default:
        TV_ASSERT(0);
    }
    return 0;
}

static void osd_binary_destroy(osd_binary *self) {
    TV_TYPE_GET_PRIV(osd_binary_priv, self, priv);
    FREE_OBJECT(priv->global);
    FREE_OBJECT(priv->palettes);
    FREE_OBJECT(priv->ingredients);
    FREE_OBJECT(priv->windows);
    FREE_OBJECT(priv->ram);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_binary *osd_binary_create(const char *target_folder) {
    osd_binary *self = MALLOC_OBJECT(osd_binary);
    osd_binary_priv *priv = MALLOC_OBJECT(osd_binary_priv);
    self->priv = priv;
    self->destroy = osd_binary_destroy;
    self->data_size = osd_binary_data_size;
    self->data = osd_binary_data;

    priv->global = osd_read_file(target_folder, GLOBAL_FILE, &priv->global_size);
    TV_ASSERT(priv->global);

    priv->palettes = osd_read_file(target_folder, PALETTES_FILE, &priv->palettes_size);
    TV_ASSERT(priv->palettes);

    priv->ingredients = osd_read_file(target_folder, INGREDIENT_FILE, &priv->ingredients_size);
    TV_ASSERT(priv->ingredients);

    priv->windows = osd_read_file(target_folder, WINDOWS_FILE, &priv->windows_size);
    TV_ASSERT(priv->windows);

    priv->ram = osd_read_file(target_folder, RAM_FILE, &priv->ram_size);
    TV_ASSERT(priv->ram);

    return self;
}