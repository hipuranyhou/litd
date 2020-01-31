// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

int generate_config_file(const char *path) {
    // TODO: generate config in home directory (implement finding home dir)

    FILE *fp = NULL;
    fp = fopen(path, "w+");
    fputs("########################################\n", fp);
    fputs("# THIS IS DEFAULT CONFIG FILE FOR litd #\n", fp);
    fputs("########################################\n", fp);
    fputs("\n", fp);
    fputs("# Poll time in milliseconds (how often should litd check for user inactivity.)\n", fp);
    fputs("# Default poll = 400\n", fp);
    fputs("poll = 400\n", fp);
    fputs("\n", fp);
    fputs("# Idle time in seconds (after how long should keyboard turn off.)\n", fp);
    fputs("# Default idle = 5 (0 for off)\n", fp);
    fputs("idle = 5\n", fp);
    fputs("\n", fp);
    fputs("# Idle time in seconds (after how long should keyboard turn off.)\n", fp);
    fputs("# Default reset = 3600 (0 for off)\n", fp);
    fputs("reset = 3600\n", fp);
    fclose(fp);
    return 1;
}

int read_config_file(const char *path, CONFIG *config) {

    FILE *fp = NULL;
    char buffer[256] = {0}, *line = NULL;
    int value = 0;
    size_t len = 0;

    fp = fopen(path, "r");

    while (getline(&line, &len, fp) && !feof(fp)) {

        // Ignore comments and empty lines;
        if (line[0] == '#' || line[0] == '\n')
            continue;

        // Get value from config line
        if (sscanf(line, "%255s = %d", buffer, &value) != 2) {
            free(line);
            return 0;
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
    return 1;

}
