#include <stdio.h>
#include <stdlib.h>

typedef struct list_item {
    int value;
    struct list_item *next;
} list_item;


void append(struct list_item *first, int x){
  while(first->next != NULL) {
    first = first->next;
  }
  list_item *new_node = (list_item *) malloc(sizeof(list_item));
  new_node->value = x;
  new_node->next = NULL;
  first->next = new_node;
}

void prepend(struct list_item *first, int x) {
    list_item *new_node = (list_item *) malloc(sizeof(list_item));
    new_node->value = x;
    new_node->next = first->next;
    first->next = new_node;
}

void print(struct list_item *first) {
  while(first->next != NULL) {
    first = first->next;
    printf("%i\n", first->value);
  }
}
  
void input_sorted(struct list_item *first, int x) {
  while(first->next->value < x) {
      first = first->next;
  } 
  prepend(first, x); 
}

void clear(struct list_item *first) {
  list_item *next = first->next;
  first->next = NULL;
  while(next != NULL){
    first = next;
    next = first->next;
    free(first); 
  }
}




 int main( int argc, char ** argv)
  {
    struct list_item root;
    root.value = -1; /* This value is always ignored */
    root.next = NULL;

    append(&root, 2);
    append(&root, 3);
    append(&root, 4);

    print(&root);

    append(&root, 42);

    print(&root);

    prepend(&root, 1);

    print(&root);

    input_sorted(&root, 30); 
    input_sorted(&root, 3); 

    print(&root);

    clear(&root); 

    print(&root);
  }
