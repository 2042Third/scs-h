//
// Created by Mike Yang on 2024/2/25.
//

#include "linked_list.h"

/**
 * Setup the linked list
 * */
cache_set* setup_linked_list( int sets) {
  cache_set* head = (cache_set *)malloc(sizeof(cache_set));
  cache_set* current = head;
  cache_set * prev = NULL;
  for (int i = 0; i < sets - 1; i++) {
    current->next = (cache_set *)malloc(sizeof(cache_set));
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
void free_linked_list(cache_set* head) {
  cache_set* current = head;
  cache_set* next;
  while (current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
}