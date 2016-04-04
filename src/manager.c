#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../include/manager.h"
#include "../include/const.h"
#include "../include/store.h"
#include "../include/log.h"

int handle_read(int *resPipe, request *parsed, log_file log, table_entry *entries);
int handle_update(int *resPipe, request *parsed, log_file log, table_entry *entries);

void initialize(table_entry **entries, log_file *log) {
    init(entries);

    *log = log_init(LOG_FILE);

    if (*log != NULL) {
        info(*log, "Log file opened");
    }
}

int handle_request(table_entry *entries, log_file log, int *reqPipe, int *resPipe) {
    if (entries == NULL && log != NULL) {
        error(log, "entries should not be NULL");
        return 0;
    }

    char req_mesg[STR_LEN];
    read(reqPipe[FD_READ], req_mesg, STR_LEN);

    request parsed;
    parse_request(req_mesg, &parsed, log);

    process_request(entries, &parsed, resPipe, log);

    return parsed.action != Q;
}

void parse_request(const char *req, request *out, log_file log) {
    if (req == NULL) {
        error(log, "Could not parse request: NULL");
        return;
    }

    int origin;
    int origin_pid;
    char action;
    char id[STR_LEN];
    int value = 0;

    sscanf(req, "%d %d %c %s %d", &origin, &origin_pid, &action, id, &value);

    out->origin = origin;
    out->origin_pid = origin_pid;
    strncpy(out->id, id, STR_LEN);

    switch(action) {
        case 'R': out->action = R; break;
        case 'U': out->action = U; out->value = value; break;
        default: out->action = Q; break;
    }
}

int process_request(table_entry *entries, request *parsed, int *resPipe, log_file log) {
    if (parsed == NULL) {
        if (log != NULL) {
            error(log, "parsed cannot be null in process_request");
        }

        return 0;
    }

    if (parsed->action == Q) {
        return 1;
    }

//    char mesg[STR_LEN];
//    if (parsed->action == U) {
//        sprintf(mesg, "Got -- %d %d %c %s %d", parsed->origin, parsed->origin_pid,
//                (char)parsed->action, parsed->id, parsed->value);
//    } else {
//        sprintf(mesg, "Got -- %d %d %c %s", parsed->origin, parsed->origin_pid,
//                (char)parsed->action, parsed->id);
//    }
//
//    info(log, mesg);

    switch(parsed->action) {
        case R: return handle_read(resPipe, parsed, log, entries);
        case U: return handle_update(resPipe, parsed, log, entries);
        default: return 0;
    }
}

int handle_read(int *resPipe, request *parsed, log_file log, table_entry *entries) {
    table_entry result = { "\0", 0 };
    int rv = table_read(entries, parsed->id, &result);

    char mesg[200];
    if (rv) {
        sprintf(mesg, "Request %d %d %c %s -- Found %s %d", parsed->origin, parsed->origin_pid,
                parsed->action, parsed->id, result.id, result.value);
        info(log, mesg);
    } else {
        sprintf(mesg, "Request %d %d %c %s -- No results found", parsed->origin, parsed->origin_pid,
                parsed->action, parsed->id);
        error(log, mesg);
    }

    char res[STR_LEN];
    sprintf(res, "%c %s %d %d", parsed->action, parsed->id, result.value, rv);
    write(resPipe[FD_WRITE], res, STR_LEN);

    return rv;
}

int handle_update(int *resPipe, request *parsed, log_file log, table_entry *entries) {
    int rv = table_update(entries, parsed->id, parsed->value);

    char mesg[200];
    if (rv) {
        sprintf(mesg, "Request %d %d %c %s -- Updated successfully", parsed->origin, parsed->origin_pid,
                parsed->action, parsed->id);
        info(log, mesg);
    } else {
        sprintf(mesg, "Request %d %d %c %s -- Failed to update", parsed->origin, parsed->origin_pid,
                parsed->action, parsed->id);
        error(log, mesg);
    }

    char res[STR_LEN];
    sprintf(res, "%c %s %d %d", parsed->action, parsed->id, parsed->value, rv);
    write(resPipe[FD_WRITE], res, STR_LEN);

    return rv;
}