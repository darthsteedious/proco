#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "include/const.h"
#include "include/store.h"
#include "include/log.h"
#include "include/manager.h"

void client(int *reqPipe, int *resPipe, int origin, FILE *file);

void close_pipes(int *pfds);

int main() {
    int pfds_p1_req[2];
    int pfds_p1_res[2];

    int pfds_p2_req[2];
    int pfds_p2_res[2];

    if (pipe(pfds_p1_req) == -1) exit(2);
    if (pipe(pfds_p1_res) == -1) exit(2);
    if (pipe(pfds_p2_req) == -1) exit(2);
    if (pipe(pfds_p2_res) == -1) exit(2);

    table_entry *entries = NULL;
    log_file log = NULL;

    int i;
    pid_t pid;
    pid_t pids[2];
    for (i = 0; i < 2; i++) {
        pid = fork();
        pids[i] = pid;

        if (pid == -1) {
            printf("Error forking process.\n");
        } else if (pid == 0) {
            int *req = i == 0 ? (int *) pfds_p1_req : pfds_p2_req;
            int *res = i == 0 ? (int *) pfds_p1_res : pfds_p2_res;

            printf("pid -- %d\n", getpid());

            char workerFile[STR_LEN];
            sprintf(workerFile, "%s/proc%d.dat", BASE, i + 1);
            printf("%d\n", i);
            printf("%s\n", workerFile);
            if (i == 0) {
                FILE *p1;
                if ((p1 = fopen(workerFile, "r")) != NULL) {
                    client(req, res, i, p1);
                    fclose(p1);
                }
            } else {
                FILE *p2;
                if ((p2 = fopen(workerFile, "r")) != NULL) {
                    client(req, res, i, p2);
                    fclose(p2);
                }
            }
            exit(0);
        }
    }

    initialize(&entries, &log);

    char pid_mesg[STR_LEN];
    sprintf(pid_mesg, "Parent pid: %d", getpid());
    info(log, pid_mesg);

    int p1Alive, p2Alive = 1;

    do {
        // Handle P1 Request
        p1Alive = handle_request(entries, log, pfds_p1_req, pfds_p1_res);

        // Handle P2 Request
        p2Alive = handle_request(entries, log, pfds_p2_req, pfds_p2_res);
    } while (p1Alive || p2Alive);

    printf("waiting\n");

    waitpid(pids[0], NULL, 0);
    waitpid(pids[1], NULL, 0);

    info(log, "Closing pipes");

    close_pipes(pfds_p1_req);
    close_pipes(pfds_p1_res);
    close_pipes(pfds_p2_req);
    close_pipes(pfds_p2_res);

    log_close(log);
    free(entries); // TODO: Need to write the final output of entries to the init.dat file

    return 0;
}

void client(int *reqPipe, int *resPipe, int origin, FILE *file) {
    int res = 0;
    char action;
    char id[STR_LEN];
    int value;
    char response[STR_LEN];
    char request[STR_LEN];

    while (res != EOF) {
        res = fscanf(file, "%s %s %d", &action, id, &value);

        if (res != EOF) {
            sprintf(request, "%d %d %c %s", origin, getpid(), action, id);

            write(reqPipe[FD_WRITE], request, STR_LEN);

            read(resPipe[FD_READ], response, STR_LEN);
            printf("res -- %s\n", response);
        }
    }

    // Write the last message
    sprintf(request, "%d %d Q %s", origin, getpid(), EMPTY);
    write(reqPipe[FD_WRITE], request, STR_LEN);
}

void close_pipes(int *pfds) {
    close(pfds[0]);
    close(pfds[1]);
}


