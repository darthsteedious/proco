#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/store.h"

#define MAX_ENTRIES 50

int parse(FILE *file, table_entry *entry);
int parse(FILE *file, table_entry *entry) {
    int result;
    char id[STR_LEN];
    int value;

    result = fscanf(file, "%s %d", id, &value);

    memcpy(entry->id, id, STR_LEN);
    entry->value = value;

    return result;
}

void parseFile(const char *fileName, table_entry *entries);
void parseFile(const char *fileName, table_entry *entries) {
    FILE *initFile;

    if ((initFile = fopen(fileName, "r")) == NULL) {
        printf("fopen -- %s\n", fileName);
        exit(1);
    }

    int result = 0;
    int index = 0;
    while (result != EOF) {
        table_entry newEntry;

        result = parse(initFile, &newEntry);

        if (result != EOF) {
            entries[index++] = newEntry;
        }
    }

    fclose(initFile);
}

int indexOf(table_entry *entries, const char *id);
int indexOf(table_entry *entries, const char *id) {
    if (entries == NULL) {
        perror("indexOf - entries");

        return -1;
    }

    int i;
    for (i = 0; i < MAX_ENTRIES; i++) {
        size_t len = strlen(id);
        table_entry entry = entries[i];
        if (strncmp(id, entry.id, len) == 0) {
            return i;
        }
    }

    return -1;
}

void init(table_entry **entries) {
    if (*entries == NULL) {
        *entries = (table_entry *)calloc(MAX_ENTRIES, sizeof(table_entry));

        parseFile(FILE_NAME, *entries);
    }
}

int table_update(table_entry *entries, char *id, int value) {
    if (entries == NULL) {
        perror("table_update -- entries");
        return 0;
    }

    int index = indexOf(entries, id); // TODO: Need a better structure

    int hasIndex = index != -1;

    if (hasIndex) {
        entries[index].value = value;
    }

    return hasIndex;
}

int table_read(table_entry *entries, char *id, table_entry *entry) {
    if (entries == NULL) {
        perror("table_read -- entries");
        return 0;
    }

    int index = indexOf(entries, id); // TODO: Need a better structure

    int hasIndex = index != -1;

    if (hasIndex) {
        *entry = entries[index];
    }

    return hasIndex;
}