// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "config.h"

volatile int reset_man = 0;
volatile int reload_config = 0;

int check_root(void) {
    uid_t uid = getuid();
    if (uid != 0)
        return 1;
    else
        return 0;
}

int process_options(int argc, char **argv, CONFIG *config) {
    int opt;
    // Process command line options
    while ((opt = getopt(argc, argv, ":dv")) != -1) {
        switch (opt) {
            // Daemonize
            case 'd':
                config->daemon = 1;
                break;
                // Verbose
            case 'v':
                config->verbose = 1;
                break;
                // Unknown option
            case '?':
                return 0;
        }
    }
    return 1;
}

int get_user_home_dir(char *homedir) {
    // https://stackoverflow.com/questions/2910377/get-home-directory-in-linux (josch && 9999years)
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return 1;
}

void print_config_file(FILE *fp) {
    fputs("########################################\n", fp);
    fputs("# THIS IS DEFAULT CONFIG FILE FOR litd #\n", fp);
    fputs("########################################\n", fp);
    fputs("\n", fp);
    fputs("# Poll time in milliseconds (how often should litd check for user inactivity.)\n", fp);
    fputs("# default: 400\n", fp);
    fputs("poll = 400\n", fp);
    fputs("\n", fp);
    fputs("# Idle time in seconds (after how long should keyboard turn off.)\n", fp);
    fputs("# default: 5 (0 for off)\n", fp);
    fputs("idle = 5\n", fp);
    fputs("\n", fp);
    fputs("# Idle time in seconds (after how long should keyboard turn off.)\n", fp);
    fputs("# default: 3600 (0 for off)\n", fp);
    fputs("reset = 3600\n", fp);
}

int generate_config_file(const char *path = NULL) {

    FILE *fp = NULL;
    //struct stat st = {0};
    /*
    // TODO: generate config in proper directory
    if (path == NULL) {

        if (stat("/home") == -1)
    }
    */
    if ((fp = fopen(path, "w+")) == NULL) {
        return 1;
    }

    print_config_file(fp);

    fclose(fp);
    return 0;
}

int read_config_file(const char *path, CONFIG *config) {

    FILE *fp = NULL;
    char buffer[256] = {0}, *line = NULL;
    int value = 0, line_cnt = 0;
    size_t len = 0;

    if((fp = fopen(path, "r")) == NULL) {
        return -1;
    }

    while (getline(&line, &len, fp) && !feof(fp)) {

        line_cnt += 1;

        // Ignore comments and empty lines;
        if (line[0] == '#' || line[0] == '\n')
            continue;

        // Get 1 value from config line_cnt-th line
        if (sscanf(line, "%255s = %d", buffer, &value) != 2) {
            free(line);
            return line_cnt;
        }

        // Assign proper config value
        if (strcmp("poll", buffer) == 0)
            config->poll = value;
        else if (strcmp("idle", buffer) == 0)
            config->idle = value * 1000;
        else if (strcmp("reset", buffer) == 0)
            config->reset = value * 1000;

    }

    free(line);
    fclose(fp);
    return 0;

}
