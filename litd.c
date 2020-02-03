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
    config.poll = 400;
    config.idle = 5000;
    config.reset = 3600000;
    int err = 0;

    // Process command line options
    if (!process_options(argc, argv, &config)) {
        fprintf(stderr, "Unknown option!\n");
        return 1;
    }

    // Check if we are not root
    if(!check_root()) {
        fprintf(stderr, "Running as root not allowed!\n");
        return 1;
    }

    // Generate config file
    if ((err = generate_config_file(&config)) != 0) {
        fprintf(stderr, "Error while generating config file.\n");
        switch (err) {
            case 1:
                fprintf(stderr, "Check permissions.\n");
                return 1;
            case 2:
                fprintf(stderr, "Cannot find user HOME directory.\n");
                return 1;
            case 3:
                fprintf(stderr, "Path for config file too long.\n");
                return 1;
            case 4:
                fprintf(stderr, "Cannot create litd config directory.\n");
                return 1;
        }
    }

    // Read config file
    if ((err = read_config_file(&config)) != 0) {
        fprintf(stderr, "Error while reading config file.\n");
        switch (err) {
            case -1:
                fprintf(stderr, "Check permissions.\n");
                return 1;
            default:
                fprintf(stderr, "Line %d\n", err);
                return 1;
        }
    }

    // Daemonize litd
    if (config.daemon) {
        daemonize(config);
        syslog(LOG_NOTICE, "Started.");
        signal(SIGUSR1, set_reset_manual);
        signal(SIGHUP, set_reload_config);
    }

    // Start main brightness controlling loop
    if ((err = start_control(&config)) != 0) {
        if (err == 3 && config.daemon) {
            syslog(LOG_ERR, "Error while reloading config file.");
            syslog(LOG_ERR, "Try running litd not in daemon mode to get more info.");
            syslog(LOG_NOTICE, "Terminating.");
            closelog();
        } else if (err == 3) {
            fprintf(stderr, "Error while reloading config file.");
            fprintf(stderr, "Try running litd not in daemon mode to get more info.");
            fprintf(stderr, "Terminating.");
        } else if (config.daemon) {
            syslog(LOG_ERR, "Error writing value to or getting value from file.");
            syslog(LOG_ERR, "Check permissions on all files.");
            syslog(LOG_NOTICE, "Terminating.");
            closelog();
        } else {
            fprintf(stderr, "Error writing value to or getting value from file.\n");
            fprintf(stderr, "Check permissions on all files.\n");
            fprintf(stderr, "Terminating.\n");
        }
        return 1;
    }

    // We never get here
    return 0;

}