/*
 * Hipuranyhou - litd - v1.0.0 - 03.02.2020
 * Daemon for automatic management of keyboard and display brightness
 * using applesmc light sensor (for Mac on Linux.)
 */

#ifndef CONTROL_H
#define CONTROL_H

void nsleep(int milisec);
int get_file_value(const char *path, const char *search);
int write_file_value(const char *path, int value);
int calc_disp_val(int disp_max, int disp_min, int sens_val);
int calc_key_val(int key_max, int key_min, int sens_val);
void print_verbose_info(int disp_val, int key_val, int idle, int reset, int sens_val, int disp_man, int key_man);
int start_control(CONFIG *config);

#endif
