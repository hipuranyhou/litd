// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#ifndef CONFIG_H
#define CONFIG_H

typedef struct config {
    int poll;
    int idle;
    int reset;
} CONFIG;

int generate_config_file(const char *path);
int read_config_file(const char *path, CONFIG *config);

#endif
