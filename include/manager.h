#ifndef PROCO_MANAGER_H
#define PROCO_MANAGER_H

#include "const.h"
#include "store.h"
#include "log.h"

typedef enum actions { Q = 'Q', R = 'R', U = 'U' } Actions;

typedef struct request {
    Actions action;
    int origin;
    int origin_pid;
    char id[STR_LEN];
    union {
        int value;
    };
} request;

void initialize(table_entry **entries, log_file *log);
int handle_request(table_entry *entries, log_file log, int *reqPipe, int *resPipe);
void parse_request(const char *req, request *out, log_file log);
int process_request(table_entry *entries, request *req, int *resPipe, log_file log);

#endif //PROCO_MANAGER_H
