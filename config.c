// Hipuranyhou - litd - v1.0 - 03.02.2020

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
    return 0;
}

int process_options(int argc, char **argv, CONFIG *config) {
    int opt;
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

int get_user_home_dir(const char **homedir) {
    // https://stackoverflow.com/questions/2910377/get-home-directory-in-linux (josch && 9999years)
    if ((*homedir = getenv("HOME")) == NULL || *homedir[0] == '\0') {
        *homedir = getpwuid(getuid())->pw_dir;
    }
    if (*homedir == NULL)
        return 0;
    return 1;
}

int check_directory(const char *path) {
    // https://codeforwin.org/2018/03/c-program-check-file-or-directory-exists-not.html
    struct stat stats = {0};
    stat(path, &stats);
    if (S_ISDIR(stats.st_mode))
        return 1;
    return 0;
}

int check_file(const char *path) {
    if (access(path, F_OK) == -1)
        return 0;
    return 1;
}

void init_config(CONFIG *config) {
    config->daemon = 0;
    config->verbose = 0;
    config->poll = 400;
    config->idle = 5000;
    config->reset = 3600000;
    config->disp_max = 0;
    config->disp_min= 0;
    config->key_max = 0;
    config->key_min = 0;
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
    fputs("# Reset time in seconds (after how long should litd ignore manual brightness settings.)\n", fp);
    fputs("# default: 3600\n", fp);
    fputs("reset = 3600\n", fp);
    fputs("\n", fp);
    fputs("# Max display brightness value from /sys/class/backlight/intel_backlight/max_brightness\n", fp);
    fputs("# default: 0 (has to be changed)\n", fp);
    fputs("display_max = 0\n", fp);
    fputs("\n", fp);
    fputs("# Min display brightness value for sensor value of 0 (in absolute darkness)\n", fp);
    fputs("# Recommended 1/10 of max\n", fp);
    fputs("# default: 0 (has to be changed)\n", fp);
    fputs("display_min = 0\n", fp);
    fputs("\n", fp);
    fputs("# Max keyboard brightness value from /sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/max_brightness\n", fp);
    fputs("# default: 0 (has to be changed)\n", fp);
    fputs("keyboard_max = 0\n", fp);
    fputs("\n", fp);
    fputs("# Min keyboard brightness value for sensor value of 0 (in absolute darkness)\n", fp);
    fputs("# Recommended 1/10 of max\n", fp);
    fputs("# default: 0 (has to be changed)\n", fp);
    fputs("keyboard_min = 0\n", fp);
}

int generate_config_file(CONFIG *config) {

    FILE *fp = NULL;
    const char *base_path;
    char folder_path[118];
    char file_path[128];

    /*
     * Decide if we should create our config folder and file inside XDH_HOME_DIRS
     * system variable or inside user HOME directory
     */
    if ((base_path = getenv("XDG_HOME_DIRS")) == NULL || base_path[0] == '\0') {
        if (!get_user_home_dir(&base_path)) {
            return 2;
        }
        if (snprintf(folder_path, sizeof(folder_path), "%s%s", base_path, "/.config/litd\0") >= 118)
            return 3;
    } else {
        if (snprintf(folder_path, sizeof(folder_path), "%s%s", base_path, "/litd\0") >= 118)
            return 3;
    }

    // Create litd directory at proper location
    if (!check_directory(folder_path))
        if (mkdir(folder_path, 0700) == -1)
            return 4;

    // Put location of litd config folder inside our global config structure
    memcpy(config->config_path, folder_path, sizeof(folder_path));

    // Get full path of config file
    snprintf(file_path, sizeof(file_path), "%s%s", folder_path, "/litd.conf\0");

    // Generate config only if it does not exist
    if (check_file(file_path))
        return 0;

    if ((fp = fopen(file_path, "w+")) == NULL) {
        return 1;
    }

    print_config_file(fp);

    fclose(fp);
    return 0;
}

int read_config_file(CONFIG *config) {

    FILE *fp = NULL;
    char buffer[32] = {0}, *line = NULL, file_path[138];
    int value = 0, line_cnt = 0;
    size_t len = 0;

    // Get full path of config file
    snprintf(file_path, sizeof(file_path), "%s%s", config->config_path, "/litd.conf\0");

    if ((fp = fopen(file_path, "r")) == NULL) {
        return -1;
    }

    while (getline(&line, &len, fp) && !feof(fp)) {

        line_cnt += 1;

        // Ignore comments and empty lines;
        if (line[0] == '#' || line[0] == '\n')
            continue;

        // Get value from config line_cnt-th line
        if (sscanf(line, "%31s = %d", buffer, &value) != 2) {
            free(line);
            return line_cnt;
        }

        // Assign proper config value into config structure
        if (strcmp("poll", buffer) == 0)
            config->poll = value;
        else if (strcmp("idle", buffer) == 0)
            config->idle = value * 1000;
        else if (strcmp("reset", buffer) == 0)
            config->reset = value * 1000;
        else if (strcmp("display_max", buffer) == 0) {
            if (value == 0)
                return -2;
            config->disp_max = value;
        } else if (strcmp("display_min", buffer) == 0) {
            if (value == 0)
                return -2;
            config->disp_min = value;
        } else if (strcmp("keyboard_max", buffer) == 0) {
            if (value == 0)
                return -2;
            config->key_max = value;
        } else if (strcmp("keyboard_min", buffer) == 0) {
            if (value == 0)
                return -2;
            config->key_min = value;
        } else {
            free(line);
            return line_cnt;
        }

    }

    free(line);
    fclose(fp);
    return 0;

}
