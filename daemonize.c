/*
* daemonize.c
* This example daemonizes a process.
* This is an answer to the stackoverflow question:
* https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux/17955149#17955149
* Fork this code: https://github.com/pasce/daemon-skeleton-linux-c
* Read about it: https://nullraum.net/how-to-create-a-daemon-in-c/
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include "config.h"
#include "control.h"
#include "daemonize.h"

void handle_exit_signal(int sig) {
    // Exit after every prepared signal
    syslog(LOG_NOTICE, "Terminated.");
    closelog();
    exit(sig);
}

void prepare_signals(void) {
    // Prepare all signals after which we terminate
    signal(SIGABRT, handle_exit_signal);
    signal(SIGHUP, handle_exit_signal);
    signal(SIGINT, handle_exit_signal);
    signal(SIGQUIT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);
}

void daemonize(CONFIG config) {

    char pid_path[138];

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

    // Get full path of .pid file and write to it PID of daemon
    snprintf(pid_path, sizeof(pid_path), "%s%s", config.config_path, "/litd.pid\0");
    write_file_value(pid_path, (int)getpid());

    // Open the log file
    openlog("litd", LOG_PID, LOG_DAEMON);

    return;
}
