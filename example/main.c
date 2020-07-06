#include "btree.h"

void btree_error_check(bt_error_t error)
{
    if(error != BTREE_ERROR_SUCCESS)
    {
        fprintf(stderr, "Error occurred - Code: %d\n", error);
    }
}

int main(void)
{
    int value = 452;

    // Create a new BTree
    btree* tree = create_btree(5);

    // Insert a new value in BTree - key and a pointer of var
    bt_error_t error = btree_insert(tree, 24, &value);
    btree_error_check(error);
    
    // Insert a new value in BTree - key and NULL
    error = btree_insert(tree, 20, NULL);
    btree_error_check(error);

    error = btree_insert(tree, 55, NULL);
    btree_error_check(error);

    // Error because the key 55 already exists in the BTree
    error = btree_insert(tree, 55, NULL);
    btree_error_check(error);




    // Search for an item in BTree
    item* searched_item = btree_find_item(tree, 24);

    printf("Value behind key: %u was: %d\n", searched_item->key, *(int*)(searched_item->data) );

    // Delete an item out of the BTree (here it returns an error, because value doesn't not exists)
    error = btree_delete_item(tree, 400);
    btree_error_check(error);


    // Destroy the whole BTree
    error = destroy_btree(tree);
    btree_error_check(error);


    return 0;
}