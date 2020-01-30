/*
* daemonize.c
* This example daemonizes a process, writes a few log messages,
* sleeps 20 seconds and terminates afterwards.
* This is an answer to the stackoverflow question:
* https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
* Fork this code: https://github.com/pasce/daemon-skeleton-linux-c
* Read about it: https://nullraum.net/how-to-create-a-daemon-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "daemonize.h"
#include "functions.h"

void handle_signal(int sig) {
    // Exit after every prepared signal
    exit(1);
}

void prepare_signals(void) {
    // Prepare all signals which we are able to handle
    signal(SIGABRT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGTERM, handle_signal);
}

void daemonize(void) {

    pid_t pid = 0;
    int fd;

    // Fork off the parent
    pid = fork();

    // Error during forking
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Terminate parent
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Child becomes leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Handle signals
    prepare_signals();

    // Fork off second time
    pid = fork();

    // Error during forking
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Terminate parent
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Set new file permissions
    umask(0);

    // Change to root directory
    chdir("/");

    // Close all open file descriptors
    for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
        close(fd);

    /* Try to write PID of daemon to lockfile */
    write_file_value("/home/hipuranyhou/.config/litd/litd.pid", (int)getpid());

    return;
}