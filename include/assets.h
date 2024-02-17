#ifndef ASSETS_H
#define ASSETS_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct asset
{
    char *name;
    char *path;
    char *filename;
};

int asset_init();
char *asset_path(char *filename);
void asset_free();

#endif // ASSETS_H