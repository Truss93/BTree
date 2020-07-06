#include "vbtree.h"
#include "btvis_nc.h"

int main(void)
{
    // Init screen (ncurses)
    initscr();
	
    // Create empty btree of order 4
    btree* tree = create_btree(4);

    // Insert elements in the btree (empty data pointer)


    btree_insert(tree, 10, NULL);
    btree_insert(tree, 20, NULL);
    btree_insert(tree, 30, NULL);

    btree_insert(tree, 12, NULL);
    btree_insert(tree, 19, NULL);
    btree_insert(tree, 32, NULL);

    btree_insert(tree, 92, NULL);
    btree_insert(tree, 12, NULL);
    btree_insert(tree, 41, NULL);

    btree_insert(tree, 3, NULL);
    btree_insert(tree, 1, NULL);
    btree_insert(tree, 5, NULL);

    btree_insert(tree, 44, NULL);
    btree_insert(tree, 53, NULL);
    btree_insert(tree, 172, NULL);

    btree_insert(tree, 7, NULL);
    btree_insert(tree, 11, NULL);
    btree_insert(tree, 60, NULL);

    

    // Draw the whole tree
    draw_tree(tree);
    
    // Destroy the btree
    destroy_btree(tree);


    getch();
    endwin();
   return 0;
}