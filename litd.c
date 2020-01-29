#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include "xidle.h"

#define SENS_PATH "/sys/devices/platform/applesmc.768/light"
#define DISP_PATH "/sys/class/backlight/intel_backlight/brightness"
#define KEY_PATH "/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness"

#define DISP_MAX 1953
#define KEY_MAX 255
#define POLL 400
#define IDLE 5000

void handle_signal(int sig) {
    // Exit after every prepared signal
    fprintf(stderr, "\nCatched signal: %d\n", sig);
    exit(1);
}

void prepare_signals(void) {
    // Prepare all signals which we are able to handle
    signal(SIGABRT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGTERM, handle_signal);
}

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
    if (sens_val < 30) return 190;
    if (sens_val > 225) return DISP_MAX;
    return ((sens_val - 30) / (225 - 30)) * (DISP_MAX - 190) + 190;
}

int calc_key_val(int sens_val) {
    // Calculates new keyboard brightness value based on sensor value in range 15 - KEY_MAX
    if (sens_val < 30) return 15;
    if (sens_val > 225) return KEY_MAX;
    return sens_val;
}

int main(void) {
    // Prepare all values on start
    int disp_val_last, disp_man = 0, key_val_last, key_man = 0, sens_val, idle;
    int disp_val = get_file_value(DISP_PATH, "%d");
    int key_val = get_file_value(KEY_PATH, "%d");
    disp_val_last = disp_val;
    key_val_last = key_val;

    prepare_signals();

    while (1) {
        // Get values every POLL milliseconds
        idle = get_user_idle_time();
        disp_val = get_file_value(DISP_PATH, "%d");
        key_val = get_file_value(KEY_PATH, "%d");
        sens_val = get_file_value(SENS_PATH, "(%d,");
        // Ignore sensor for keyboard or display or both if user changes brightness manually
        if (disp_val_last != disp_val) {
            disp_val_last = disp_val;
            disp_man = 1;
        }
        if (key_val_last != key_val) {
            key_val_last = key_val;
            key_man = 1;
        }
        if (disp_man && key_man) {
            disp_val = disp_val_last;
            key_val = key_val_last;
        } else if (disp_man && !key_man) {
            disp_val = disp_val_last;
            key_val = calc_key_val(sens_val);
            key_val_last = key_val;
        } else if (!disp_man && key_man) {
            disp_val = calc_disp_val(sens_val);
            disp_val_last = disp_val;
            key_val = key_val_last;
        } else if (!disp_man && !key_man) {
            disp_val = calc_disp_val(sens_val);
            disp_val_last = disp_val;
            key_val = calc_key_val(sens_val);
            key_val_last = key_val;
        }
        // Turn off keyboard after IDLE milliseconds
        if (idle > IDLE) {
            write_file_value(DISP_PATH, disp_val);
            write_file_value(KEY_PATH, 0);
        } else {
            write_file_value(DISP_PATH, disp_val);
            write_file_value(KEY_PATH, key_val);
        }
        printf("Display: %d\n", disp_val);
        printf("Keyboard: %d\n", key_val);
        printf("Idle: %d\n", idle);
        printf("Sensor: %d\n", sens_val);
        printf("Disp_man: %d Key_man: %d\n", disp_man, key_man);
        nsleep(POLL);
    }

    // We never get here
    return 0;
}