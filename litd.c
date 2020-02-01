// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include "config.h"
#include "daemonize.h"
#include "control.h"

extern volatile int reset_man;
extern volatile int reload_config;

void set_reset_manual(int sig) {
    reset_man = 1;
    return;
}

void set_reload_config(int sig) {
    // TODO: SIGHUP reload config
    reload_config = 1;
    return;
}

int main(int argc, char **argv) {

    CONFIG config;
    config.daemon = 0;
    config.verbose = 0;
    int err = 0;

    // Generate config file
    // TODO: generate config only if it does not exist
    if ((err = generate_config_file("/home/hipuranyhou/litd.conf")) != 0) {
        switch (err) {
            case 1:
                fprintf(stderr, "Error while generating config file.\n");
                fprintf(stderr, "Cannot open config file.\n");
                return 1;
        }
    }

    // Read config file
    if ((err = read_config_file("/home/hipuranyhou/litd.conf", &config)) != 0) {
        switch (err) {
            case -1:
                fprintf(stderr, "Error reading config file. Line %d\n", err);
                return 1;
            default:
                fprintf(stderr, "Error reading config file. Line %d\n", err);
                return 1;
        }
    }

    // Check if we are not root
    if(!check_root()) {
        fprintf(stderr, "Running as root not allowed!\n");
        return 1;
    }

    // Process command line options
    if (!process_options(argc, argv, &config)) {
        fprintf(stderr, "Unknown option!\n");
        return 1;
    }

    // Daemonize litd
    if (config.daemon) {
        daemonize();
        syslog (LOG_NOTICE, "Started.");
        signal(SIGUSR1, set_reset_manual);
        // TODO: SIGHUP reload config
        signal(SIGHUP, set_reload_config);
    }

    // Start main brightness controlling loop
    start_control(config);

    // We never get here
    return 0;
}