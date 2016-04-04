#include "../include/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void timestamp(char *out);
void timestamp(char *out) {
    time_t t;

    time(&t);
    struct tm *timeInfo;

    timeInfo = localtime(&t);

    strftime(out, 50, "%F %T", timeInfo);
}

log_file log_init(const char *path) {
    log_file log;

    if ((log = fopen(path, "a")) != NULL) {
        fprintf(log, "----------\n");
        return log;
    } else {
        printf("Error opening log.\n");
    }

    return NULL;
}

void info(log_file log, const char *message) {
    char time_str[50];
    timestamp(time_str);
    if (log != NULL) {
        fprintf(log, "INFO\t[%s] -- %s\n", time_str, message);
    }
}

void error(log_file log, const char *message) {
    char time_str[50];
    timestamp(time_str);
    if (log != NULL) {
        fprintf(log, "ERROR\t[%s] -- %s\n", time_str, message);
    } else {
        printf("Log file null.\n");
    }
}

void log_close(log_file log) {
    if (log != NULL) {
        info(log, "Log file closing");
        fprintf(log, "----------\n");
        fclose(log);
     } else {
        printf("Log file null.\n");
     }
}

