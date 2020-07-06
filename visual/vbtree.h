#ifndef BTREE
#define BTREE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include <ncurses.h>


typedef int bt_error_t;
// Errorcodes
#define BTREE_ERROR_SUCCESS             0
#define BTREE_ERROR_POINTER_IS_NULL     1
#define BTREE_ERROR_POINTER_NOT_ROOT    2
#define BTREE_ERROR_IND_ALREADY_EXISTS  3
#define BTREE_ERROR_IND_NOT_FOUND       4


// Struct for the index with the data pointer
typedef struct _item{
  void* data;
  unsigned int key;
} item;

struct _config{
    int degree_of_tree;
    int separator; 
}; 


// Struct for nodes of tree
typedef struct _node{
  void* pointer_parent; //Pointer to parent of this node
  struct _item* items; // Pointer to an 'array' of struct _item
  bool leaf; // Bool if node is a leaf
  bool root;  
  int entries;  // Count for entries (number of items) in the node
  WINDOW* node_win; // Pointer that store window ptr from ncurses
  struct _node* pointer_children[];
} btree;


// Function to create a new btree
// Return NULL if degree_of_tree is smaller 3
struct _node* create_btree(int degree_of_tree);

// Destroy whole btree
bt_error_t destroy_btree(struct _node* node);


bt_error_t btree_insert(struct _node* node, unsigned int key, void* data);
struct _item* btree_find_item(struct _node* node, unsigned int key);
bt_error_t tree_delete_item(struct _node* node, unsigned int key);


#endif