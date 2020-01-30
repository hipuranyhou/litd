#include <stdio.h>
#include <time.h>
#include "functions.h"

#define DISP_MAX 1953
#define KEY_MAX 255

void nsleep(int milisec) {
    // Helper function for nanosleep()
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = milisec * 1000000L;
    nanosleep(&req, NULL);
    return;
}

int get_file_value(const char *path, const char *search) {
    // Gets brightness value from file
    FILE *fp = NULL;
    int value = 100;
    fp = fopen(path, "r");
    fscanf(fp, search, &value);
    fclose(fp);
    return value;
}

void write_file_value(const char *path, int value) {
    // Writes new brightness value into file
    FILE *fp = NULL;
    fp = fopen(path, "w+");
    fprintf(fp, "%d", value);
    fclose(fp);
    return;
}

int calc_disp_val(int sens_val) {
    // Calculates new display brightness value based on sensor value in range 190 - DISP_MAX
    if (sens_val < 30)
        return 190;
    if (sens_val > 225)
        return DISP_MAX;
    return ((sens_val - 30) / (225 - 30)) * (DISP_MAX - 190) + 190;
}

int calc_key_val(int sens_val) {
    // Calculates new keyboard brightness value based on sensor value in range 15 - KEY_MAX
    if (sens_val < 30)
        return 25;
    if (sens_val > 225)
        return KEY_MAX;
    return sens_val;
}

void print_info(int disp_val, int key_val, int idle, int reset, int sens_val, int disp_man, int key_man) {
    printf("Display: %d\n", disp_val);
    printf("Keyboard: %d\n", key_val);
    printf("Idle: %d\n", idle);
    printf("Reset: %d\n", reset);
    printf("Sensor: %d\n", sens_val);
    printf("Disp_man: %d Key_man: %d\n", disp_man, key_man);
    printf("\n");
    return;
}

