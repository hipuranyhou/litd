#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include "xidle.h"

#define SENSOR_PATH "/sys/devices/platform/applesmc.768/light"
#define DISP_PATH "/sys/class/backlight/intel_backlight/brightness"
#define KEY_PATH "/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness"

void handle_signal(int sig) {
    printf("Catched signal: %d\n", sig);
    exit(1);
}

void prepare_signals(void) {
    signal(SIGABRT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGTERM, handle_signal);
}

void nsleep(int milisec) {
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = milisec * 1000000L;
    nanosleep(&req, NULL);
    return;
}

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

    prepare_signals();

    while (1) {
        printf("Keyboard: %d\n", get_file_value(KEY_PATH));
        nsleep(400);
    }

    return 0;
}