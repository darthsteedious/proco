//
// Created by Ed McDonald on 4/1/16.
//

#ifndef PROCO_STORE_H
#define PROCO_STORE_H

#include "const.h"

typedef struct table_entry {
    char id[STR_LEN];
    int value;
} table_entry;

void init(table_entry **entries);
int table_read(table_entry *entries, char *id, table_entry *value);
int table_update(table_entry *entries, char *id, int value);

#endif //PROCO_STORE_H
