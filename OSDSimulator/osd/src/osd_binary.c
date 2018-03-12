#include "osd_common.h"
#include "osd_binary.h"

#define GLOBAL_FILE		"global.bin"
#define PALETTES_FILE	"palettes.bin"
#define WINDOWS_FILE	"windows.bin"
#define INGREDIENT_FILE	"ingredients.bin"
#define MODIFIERS_FILE	"modifiers.bin"
#define RAM_FILE		"ram.bin"

osd_binary *osd_binary_new(const char *target_folder) {
    osd_binary *binary = MALLOC_OBJECT(osd_binary);

    binary->global = osd_read_file(target_folder, GLOBAL_FILE, &binary->global_size);
    assert(binary->global);

    binary->palettes = osd_read_file(target_folder, PALETTES_FILE, &binary->palettes_size);
    assert(binary->palettes);

    binary->ingredients = osd_read_file(target_folder, INGREDIENT_FILE, &binary->ingredients_size);
    assert(binary->ingredients);

    binary->windows = osd_read_file(target_folder, WINDOWS_FILE, &binary->windows_size);
    assert(binary->windows);

    binary->ram = osd_read_file(target_folder, RAM_FILE, &binary->ram_size);
    assert(binary->ram);

    return binary;
}

void osd_binary_delete(osd_binary *binary) {
    assert(binary);
    FREE_OBJECT(binary->global);
    FREE_OBJECT(binary->palettes);
    FREE_OBJECT(binary->ingredients);
    FREE_OBJECT(binary->windows);
    FREE_OBJECT(binary->ram);
    FREE_OBJECT(binary);
}