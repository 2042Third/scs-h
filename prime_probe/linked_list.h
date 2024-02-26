//
// Created by Mike Yang on 2024/2/25.
//

#ifndef LAB1_15793_LINKED_LIST_H
#define LAB1_15793_LINKED_LIST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct cache_line cache_line;

struct cache_line {
    cache_line *next;
    cache_line *prev;

    uint32_t start;
    uint32_t end;
    uint32_t timing;

    uint64_t lineAddr;

};

cache_line* setup_linked_list(int ways, int sets);
void free_linked_list(cache_line* head);

#endif //LAB1_15793_LINKED_LIST_H
