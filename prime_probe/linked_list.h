//
// Created by Mike Yang on 2024/2/25.
//

#ifndef LAB1_15793_LINKED_LIST_H
#define LAB1_15793_LINKED_LIST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct cache_set cache_set;
typedef struct set_line_addr set_line_addr;

struct cache_set {
    uint64_t lineAddr;
    cache_set *next;
    cache_set *prev;

    uint32_t timing;

    uint16_t setNum;

};


cache_set* setup_linked_list(int sets);
void free_linked_list(cache_set* head);

#endif //LAB1_15793_LINKED_LIST_H
