//
// Created by Mike Yang on 2024/2/25.
//

#include "linked_list.h"

/**
 * Setup the linked list
 * */
cache_line* setup_linked_list(int ways, int sets) {
  cache_line* head = (cache_line *)malloc(sizeof(cache_line));
  cache_line* current = head;
  cache_line * prev = NULL;
  for (int i = 0; i < sets * ways - 1; i++) {
    current->next = (cache_line *)malloc(sizeof(cache_line));
    current->prev = prev;
    prev = current;
    current = current->next;

  }

  current->next = NULL;
  return head;
}

/**
 * Free the linked list
 * */
void free_linked_list(cache_line* head) {
  cache_line* current = head;
  cache_line* next;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
}