# BTree

**Functions:**

Create a new BTree:
```c
btree* create_btree(int degree_of_tree);
```

Destroy the BTree:
```c
bt_error_t destroy_btree(btree* node);
```

Insert new data in BTree:
```c
bt_error_t btree_insert(btree* node, unsigned int key, void* data); 
```

Find data in BTree:
```c
item* btree_find_item(btree* node, unsigned int key);
```

Delete data from BTree:
```c
bt_error_t btree_delete_item(btree* node, unsigned int key);
```


**Visualization:**
The visualization of the b-tree based on ncurses. If make failed, because ncurses in missing, it should be enough to install 'libncursesw5-dev'.
