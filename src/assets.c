#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "assets.h"
#include "log.h"

// uint64_t asset_hash(const void *item, uint64_t seed0, uint64_t seed1) {
//     const struct asset *asset = item;
//     return hashmap_sip(asset->name, strlen(asset->name), seed0, seed1);
// }

// int user_compare(const void *a, const void *b, void *udata) {
//     const struct asset *ua = a;
//     const struct asset *ub = b;
//     return strcmp(ua->name, ub->name);
// }

// struct hashmap *asset_map;
static struct asset * asset_map;
static size_t asset_count = 0;

int asset_init() {
//    asset_map = hashmap_new(sizeof(struct asset), 0, 0, 0, 
//                                      asset_hash, user_compare, NULL, NULL);
  // Load assets.json
    FILE * file = fopen("assets.json", "r");
    if (file == NULL) {
        log_error("Failed to open assets.json the game will not start without it");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char * buffer = malloc(size);
    fread(buffer, 1, size, file);
    fclose(file);
    cJSON * json = cJSON_Parse(buffer);
    if (json == NULL) {
        return -1;
    }
    cJSON * assets = cJSON_GetObjectItem(json, "assets");
    if (assets == NULL) {
        return -1;
    }
    cJSON * asset = NULL;
    int asset_index = 0;
    asset_map = calloc(cJSON_GetArraySize(assets), sizeof(struct asset));
    asset_count = cJSON_GetArraySize(assets);
    cJSON_ArrayForEach(asset, assets) {
        cJSON * name = cJSON_GetObjectItem(asset, "name");
        cJSON * path = cJSON_GetObjectItem(asset, "path");
        if (name == NULL || path == NULL) {
            continue;
        }
        struct asset * a = &asset_map[asset_index];
        a->name = malloc(strlen(name->valuestring) + 1);
        strcpy(a->name, name->valuestring);
        a->path = malloc(strlen(path->valuestring) + 1);
        strcpy(a->path, path->valuestring);
        a->filename = malloc(strlen(name->valuestring) + strlen(path->valuestring) + 2);
        sprintf(a->filename, "%s/%s", a->path, a->name);
        log_debug("Asset: %s %s %s", a->name, a->path, a->filename);
        if (!access(a->filename, F_OK) == 0) {
            // file does not exists
            log_error("Asset file does not exist: %s", a->filename);
            cJSON_Delete(json);
            exit(1);
        } 
        asset_index++;
    }
    cJSON_Delete(json);
    return 0;
}

char * asset_path(char * filename) {
    log_debug("Looking for asset: %s in total of %d assets", filename, asset_count);
    for (size_t i = 0; i < asset_count; i++)
    {
        if (strcmp(asset_map[i].name, filename) == 0) {
            log_debug("Found asset: %s", asset_map[i].filename);
            return asset_map[i].filename;
        }
    }
    log_error("Asset not found: %s", filename);
    return NULL;
}

void asset_free() {
    for (size_t i = 0; i < asset_count; i++)
    {
        free(asset_map[i].name);
        free(asset_map[i].path);
        free(asset_map[i].filename);
    }
    free(asset_map);
}