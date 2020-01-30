#ifndef LITD_DAEMONIZE_H
#define LITD_DAEMONIZE_H

void handle_signal(int sig);
void prepare_signals(void);
void daemonize(void);

#endif
