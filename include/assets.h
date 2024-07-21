#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct asset {
    char *name;
    char *path;
    char *filename;
};

int asset_init();
char *asset_path(char *filename);
void asset_free();

#endif // ASSETS_H
