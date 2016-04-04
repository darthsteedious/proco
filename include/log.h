#ifndef PROCO_LOG_H
#define PROCO_LOG_H

#include <stdio.h>
#include <stdlib.h>

typedef FILE* log_file;

log_file log_init(const char *path);
void info(log_file log, const char *message);
void error(log_file log, const char *message);
void log_close(log_file log);

#endif //PROCO_LOG_H
