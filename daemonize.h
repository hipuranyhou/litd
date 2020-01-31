// Hipuranyhou - litd - ver 0.2 - 30.01.2020

#ifndef DAEMONIZE_H
#define DAEMONIZE_H

void handle_signal(int sig);
void prepare_signals(void);
void daemonize(void);

#endif
