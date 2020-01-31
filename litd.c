// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#include <signal.h>
#include <syslog.h>
#include "xidle.h"
#include "daemonize.h"
#include "functions.h"

#define SENS_PATH "/sys/devices/platform/applesmc.768/light"
#define DISP_PATH "/sys/class/backlight/intel_backlight/brightness"
#define KEY_PATH "/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness"

#define POLL 400
#define IDLE 5000
#define RESET_MAN 3600000

static volatile int reset_man = 0;

void set_reset_man(int sig) {
    reset_man = 1;
    return;
}

int main(int argc, char **argv) {

    // Prepare all values on start
    int disp_val_last,  key_val_last,  sens_val, idle;
    int disp_man = 0, key_man = 0, reset = 0, daemon = 0, verbose = 0;
    int disp_val = get_file_value(DISP_PATH, "%d");
    int key_val = get_file_value(KEY_PATH, "%d");
    disp_val_last = disp_val;
    key_val_last = key_val;

    // Check if we are not root
    if(!check_user())
        return 1;

    // Process command line options
    if (!process_options(argc, argv, &daemon, &verbose))
        return 1;

    // Daemonize litd
    if (daemon) {
        daemonize();
        syslog (LOG_NOTICE, "Started.");
        signal(SIGUSR1, set_reset_man);
    }

    // Main brightness controlling loop
    while (1) {

        // Get values every POLL milliseconds
        idle = get_user_idle_time();
        reset += POLL;
        disp_val = get_file_value(DISP_PATH, "%d");
        key_val = get_file_value(KEY_PATH, "%d");
        sens_val = get_file_value(SENS_PATH, "(%d,");

        // Reset manual mode if SIGUSR1 received
        if (reset_man) {
            disp_man = 0;
            key_man = 0;
            reset_man = 0;
            syslog(LOG_NOTICE, "Manual mode resetted.");
        }

        // Reset manual mode after RESET_MAN milliseconds
        if (reset - POLL > RESET_MAN) {
            disp_val_last = disp_val;
            key_val_last = key_val;
            reset = 0;
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
        if (idle > IDLE)
            key_val = 0;

        // Adjust values in brightness files
        write_file_value(DISP_PATH, disp_val);
        write_file_value(KEY_PATH, key_val);

        // Print debug info when not in daemon mode (-v flag)
        if (verbose && !daemon)
            print_info(disp_val, key_val, idle, reset, sens_val, disp_man, key_man);

        // Wait for POLL millisecond
        nsleep(POLL);

    }

    // We never get here
    return 0;
}