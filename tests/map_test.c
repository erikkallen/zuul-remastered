#include <stdio.h>
#include <stdlib.h>
#include "map.h"


int main() {
    // Load the map
    Map * map = malloc(sizeof(Map));
    int rc = map_load(map, "../assets/home.tmj");

    // Check if the map was loaded successfully
    if (map == NULL || rc != 0) {
        printf("Failed to load the map\n");
        return 1;
    }

    // Perform operations on the map

    // Free the map
    map_free(map);
    free(map);

    return 0;
}
