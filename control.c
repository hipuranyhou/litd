// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include "config.h"
#include "control.h"
#include "xidle.h"

// TODO: config default max brightness, otherwise use default

#define DISP_MAX 1953
#define KEY_MAX 255

#define SENS_PATH "/sys/devices/platform/applesmc.768/light"
#define DISP_PATH "/sys/class/backlight/intel_backlight/brightness"
#define KEY_PATH "/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness"

extern volatile int reset_man;
extern volatile int reload_config;

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
    if ((fp = fopen(path, "r")) == NULL)
        return -1;
    fscanf(fp, search, &value);
    fclose(fp);
    return value;
}

int write_file_value(const char *path, int value) {
    // Writes new brightness value into file
    FILE *fp = NULL;
    if ((fp = fopen(path, "w+")) == NULL)
        return 0;
    fprintf(fp, "%d\n", value);
    fclose(fp);
    return 1;
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
    // Calculates new keyboard brightness value based on sensor value in range 25 - KEY_MAX
    if (sens_val < 30)
        return 25;
    if (sens_val > 225)
        return KEY_MAX;
    return sens_val;
}

void print_verbose_info(int disp_val, int key_val, int idle, int reset, int sens_val, int disp_man, int key_man) {
    // Prints all info to stdout in verbose mode
    printf("Display: %d\n", disp_val);
    printf("Keyboard: %d\n", key_val);
    printf("Idle: %d\n", idle);
    printf("Reset: %d\n", reset);
    printf("Sensor: %d\n", sens_val);
    printf("Disp_man: %d Key_man: %d\n", disp_man, key_man);
    printf("\n");
    return;
}

int start_control(CONFIG *config) {

    // Prepare all values on start
    int disp_val, disp_val_last,  key_val, key_val_last,  sens_val, idle_time;
    int disp_man = 0, key_man = 0, reset_time = 0;
    if ((disp_val = get_file_value(DISP_PATH, "%d")) == -1)
        return 1;
    if ((key_val = get_file_value(KEY_PATH, "%d")) == -1)
        return 1;
    disp_val_last = disp_val;
    key_val_last = key_val;

    // Main brightness controlling loop
    while (1) {
        // TODO: 0 for off idle and reset
        // Get values every POLL milliseconds
        idle_time = get_user_idle_time();
        reset_time += config->poll;
        if ((disp_val = get_file_value(DISP_PATH, "%d")) == -1)
            return 1;
        if ((key_val = get_file_value(KEY_PATH, "%d")) == -1)
            return 1;
        if ((sens_val = get_file_value(SENS_PATH, "(%d,")) == -1)
            return 1;

        // Reset manual mode if SIGUSR1 received
        if (reset_man) {
            disp_man = 0;
            key_man = 0;
            reset_man = 0;
            if (config->daemon)
                syslog(LOG_NOTICE, "Manual mode resetted.");
            else
                printf("Manual mode resetted.\n");
        }

        // Reload config if SIGHUP received
        if (reload_config) {
            if (read_config_file(config) != 0)
                return 3;
            disp_man = 0;
            key_man = 0;
            reload_config = 0;
            if (config->daemon)
                syslog(LOG_NOTICE, "Config reloaded.");
            else
                printf("Config reloaded.\n");
        }

        // Reset manual mode after RESET_MAN milliseconds
        if (reset_time - config->poll > config->reset) {
            disp_val_last = disp_val;
            key_val_last = key_val;
            reset_time = 0;
            disp_man = 0;
            key_man = 0;
        }

        // Ignore sensor for keyboard or display or both if user changes brightness manually
        if (disp_val_last != disp_val) {
            disp_val_last = disp_val;
            disp_man = 1;
        }
        if (key_val_last != key_val) {
            key_val_last = (key_val == 0) ? key_val_last : key_val;
            key_man = 1;
        }

        // Calculate brightness values
        disp_val = (disp_man) ? disp_val_last : calc_disp_val(sens_val);
        key_val = (key_man) ? key_val_last : calc_key_val(sens_val);
        disp_val_last = disp_val;
        key_val_last = key_val;

        // Turn off keyboard after IDLE milliseconds
        if (idle_time > config->idle)
            key_val = 0;

        // Adjust values in brightness files
        if (!write_file_value(DISP_PATH, disp_val))
            return 2;
        if (!write_file_value(KEY_PATH, key_val))
            return 2;

        // Print debug info when not in daemon mode (-v flag)
        if (config->verbose && !config->daemon)
            print_verbose_info(disp_val, key_val, idle_time, reset_time, sens_val, disp_man, key_man);

        // Wait for POLL millisecond
        nsleep(config->poll);

    }

    // We never get here
    return 0;

}