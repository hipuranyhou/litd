#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int process_options(int argc, char **argv, int *daemon, int *verbose);
void nsleep(int milisec);
int get_file_value(const char *path, const char *search);
void write_file_value(const char *path, int value);
int calc_disp_val(int sens_val);
int calc_key_val(int sens_val);
void print_info(int disp_val, int key_val, int idle, int reset, int sens_val, int disp_man, int key_man);

#endif
