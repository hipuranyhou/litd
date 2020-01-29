#include <stdio.h>
#include <time.h>
#include "xidle.h"

#define SENSOR_PATH "/sys/devices/platform/applesmc.768/light"
#define DISP_PATH "/sys/class/backlight/intel_backlight/brightness"
#define DISP_ACT_PATH "/sys/class/backlight/intel_backlight/actual_brightness"
#define KEY_PATH "/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness"

int get_file_value(const char *path) {
    FILE *fp = NULL;
    int value = 1000;
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error reading value from %s\n", path);
        return -1;
    }
    fscanf(fp, "%d", &value);
    fclose(fp);
    return value;
}

int main(void) {

    

    return 0;
}