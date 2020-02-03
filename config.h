/*
 * Hipuranyhou - litd - v1.0.0 - 03.02.2020
 * Daemon for automatic management of keyboard and display brightness
 * using applesmc light sensor (for Mac on Linux.)
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct config {
    char config_path[128];
    int daemon;
    int verbose;
    int poll;
    int idle;
    int reset;
    int disp_max;
    int disp_min;
    int key_max;
    int key_min;
} CONFIG;

int check_root(void);
int process_options(int argc, char **argv, CONFIG *config);
int get_user_home_dir(const char **homedir);
int check_directory(const char *path);
void init_config(CONFIG *config);
void print_config_file(FILE *fp);
int generate_config_file(CONFIG *config);
int read_config_file(CONFIG *config);

#endif
