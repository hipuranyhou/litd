/*
* daemonize.c
* This example daemonizes a process.
* This is an answer to the stackoverflow question:
* https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
* Fork this code: https://github.com/pasce/daemon-skeleton-linux-c
* Read about it: https://nullraum.net/how-to-create-a-daemon-in-c/
*/

#ifndef DAEMONIZE_H
#define DAEMONIZE_H

void handle_exit_signal(int sig);
void prepare_signals(void);
void daemonize(CONFIG config);

#endif
