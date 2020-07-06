#include "vbtree.h"

struct _find{
	struct _node* node;
	int index;
};

// Checks if given pointer is a valid root node
// Can't handle if given ptr is no struct _node ptr, but compiler returns some warnings
int check_root_pointer(struct _node* node)
{
	if(node == NULL)
		return BTREE_ERROR_POINTER_IS_NULL;
	if(!node->root)
		return BTREE_ERROR_POINTER_NOT_ROOT;
	
	return BTREE_ERROR_SUCCESS;
}

// Calculates the separator in dependence of the degree
int get_separator(int degree_of_tree)
{
  if(degree_of_tree %  2 == 0)
  {
      return (degree_of_tree / 2) - 1;
  }
  return degree_of_tree / 2;
}

// Init a new created node with default values
void init_node(struct _node* node, int degree_of_tree)
{
  node->entries = 0;
  node->leaf = true;
  node->root = false;
  node->pointer_parent = NULL;

  for(int i = 0; i <= degree_of_tree; i++)
  {
    node->items[i].key = UINT_MAX;
    node->pointer_children[i] = NULL;
  }
}

// Function to create a new node
struct _node* create_node(int degree_of_tree)
{	
	struct _item* items_ptr = (struct _item*)malloc(sizeof(struct _item) * (degree_of_tree + 1)); // without +1 leak some DoT creates a DEADLYSIGNAL
  	struct _node* node = (struct _node*)malloc(sizeof(struct _node) + sizeof(struct _node*) * (degree_of_tree + 1)); //before sizeof(struct _node) * degree_of_tree

	node->items = items_ptr;
  	init_node(node, degree_of_tree);
  	return node;
}


// Function to create the tree
struct _node* create_btree(int degree_of_tree)
{
	if(degree_of_tree < 3)
	{
		fprintf(stderr, "Degree of tree is to low, use at least 3!\n");
		return NULL;
	}

    int separator = get_separator(degree_of_tree);
    struct _config* config = (struct _config*)malloc(sizeof(struct _config));
    struct _node* root = create_node(degree_of_tree);

	// Writes the config data in the config struct which is the parent of the root
    config->degree_of_tree = degree_of_tree;
    config->separator = separator;

    root->pointer_parent = config;
    root->root = true;

    return root;
}


// Return pointer of next node dependend on the key
struct _node* next_node(struct _node* node, unsigned int key)
{ 
  for(int i = 0; i <= node->entries; i++)
  {		
		if(node->items[i].key > key)
		{
			return node->pointer_children[i];
		}
  }
  // When if not triggerd only most right path possible
  return node->pointer_children[node->entries];
}


// Compare Function for qsort() | ? Source: https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
//Hint: https://stackoverflow.com/questions/13372688/sorting-members-of-structure-array
// Hint: https://en.cppreference.com/w/c/algorithm/qsort
int compare_func_sort(const void * a, const void * b)
{
  struct _item* ptr_a = (struct _item*)a;
  struct _item* ptr_b = (struct _item*)b;

  unsigned int key_a = ptr_a->key;
  unsigned int key_b = ptr_b->key;

  if(key_a < key_b)
    return -1;

  if(key_a > key_b)
    return 1;

  return 0;
}

// Function for bsearch 
int compare_func_search(const void* a, const void* b)
{
	struct _item* ptr_b = (struct _item*)b;

	unsigned int key_a = *(unsigned int*)a;
	unsigned int key_b = ptr_b->key;

	if(key_a < key_b)
    	return -1;

	if(key_a > key_b)
		return 1;

  	return 0;
}


// After root node overflow split the node and create two new nodes
void root_overflow_split(struct _node* node, int degree_of_tree)
{
  // Array hold the two pointers for the child_nodes
  struct _node* nodes[2];

  for(int i = 0; i < 2; i++)
  {
    nodes[i] = create_node(degree_of_tree);
    node->pointer_children[i] = nodes[i];
    nodes[i]->pointer_parent = node;
  }
}

// Copies left side of median in new node and resets the left side
void leftside_copy_node(struct _node* node, struct _node* new_node, int separator)
{
  for(int i = 0; i < separator; i++)
  {
	new_node->items[i] = node->items[i];
	new_node->entries = separator;

	node->items[i].key = UINT_MAX;
	node->items[i].data = NULL;
  }
}

// Copies right side of median in new node and reset the right side
void rightside_copy_node(struct _node* node, struct _node* new_node, int separator, int degree_of_tree)
{
  for(int i = separator+1; i < degree_of_tree; i++)
  {
    new_node->items[i-separator-1] = node->items[i];
    new_node->entries = degree_of_tree / 2; 

    node->items[i].key = UINT_MAX;
    node->items[i].data = NULL;
  }
}

void overflow_copy_children(struct _node* node, struct _node* new_node, int separator, int degree_of_tree)
{
	// Copies children pointers left from median in new node
	for(int i = 0; i < separator+1; i++)
	{
		new_node->pointer_children[i] = node->pointer_children[i];
		new_node->pointer_children[i]->pointer_parent = new_node;
	}

	// Copies the right child pointer to left side of node and reset old pos
	for(int i = 0; i <= (degree_of_tree / 2); i++)
	{
		node->pointer_children[i] = node->pointer_children[separator+1+i];
		node->pointer_children[separator+1+i] = NULL;
	}
}

// Splits non-root nodes and copy values | if node is a non-root parent also manage children pointer
void overflow_split(struct _node* node, int degree_of_tree, int separator, bool copy_children)
{
			struct _node* new_node = create_node(degree_of_tree);
			leftside_copy_node(node, new_node, separator);
			

			// Inserts median element of node with overflow at end of the parent node
			((struct _node*)node->pointer_parent)->items[((struct _node*)node->pointer_parent)->entries] = node->items[separator];
			((struct _node*)node->pointer_parent)->entries++;
			qsort(((struct _node*)node->pointer_parent)->items, degree_of_tree, sizeof(struct _item), compare_func_sort);


			// Search key in 'array' and returns a pointer to key if found
			struct _item* item_ptr = bsearch(&node->items[separator].key, ((struct _node*)node->pointer_parent)->items, degree_of_tree, sizeof(struct _item), compare_func_search);
			int found_index = item_ptr - ((struct _node*)node->pointer_parent)->items;

			// Shifts the pointer of childs
			for(int i = degree_of_tree-1; i >= found_index; i--)
			{
				((struct _node*)node->pointer_parent)->pointer_children[i+1] = ((struct _node*)node->pointer_parent)->pointer_children[i];
			}

			((struct _node*)node->pointer_parent)->pointer_children[found_index] = new_node;
			new_node->pointer_parent = node->pointer_parent;

			// Removes median value from node
			node->items[separator].key = UINT_MAX;
			node->items[separator].data = NULL;

			qsort(node->items, degree_of_tree, sizeof(struct _item), compare_func_sort);
			node->entries = degree_of_tree / 2; //separator; //Wrong!

			if(copy_children)
			{
				overflow_copy_children(node, new_node, separator, degree_of_tree);
				new_node->leaf = false;
			}
}

// Inserts key and data in item array of node and sorts it
void item_insert(struct _node* node, unsigned int key, void* data)
{	
	
	//Shoud improve performance without the qsort
	for(int i = 0; i <= node->entries; i++)
	{
		if(key < node->items[i].key)
		{
			for(int j = node->entries-1; j >= i; j--)
			{
				node->items[j+1] = node->items[j];
			}
			node->items[i].key = key;
			node->items[i].data = data;
			node->entries++;
			return;
		}
		
	}


	node->items[node->entries].key = key;
	node->items[node->entries].data = data;
	node->entries++;
	
	return;
}


// Inserts key and data in tree | TODO: maybe shoud returns a int with error code instead of void
bt_error_t btree_insert(struct _node* node, unsigned int key, void* data)
{	
	// Checks the given pointer to the root of the tree
	int error = check_root_pointer(node);
	if(error != 0)
		return error;
	
	struct _item* check_key = btree_find_item(node, key);
	if(check_key != NULL && check_key->key == key)
		return BTREE_ERROR_IND_ALREADY_EXISTS;


	int degree_of_tree  = ((struct _config*)node->pointer_parent)->degree_of_tree;
  	int separator = ((struct _config*)node->pointer_parent)->separator;

	// Inserts when root is a leaf
	if(node->leaf == true)
	{	
		item_insert(node, key, data);
		

  		// Overflow handling
    	if(node->entries >= degree_of_tree)
    	{
      		root_overflow_split(node, degree_of_tree);
    		leftside_copy_node(node, node->pointer_children[0], separator);
      		rightside_copy_node(node, node->pointer_children[1], separator, degree_of_tree);
			
      		node->leaf = false;
      		node->entries = 1; // Check ! - parent node shoud be only one entry
      
      		qsort(node->items, degree_of_tree, sizeof(struct _item), compare_func_sort);
		}
	}
  	else if(node->leaf == false)
  	{
		// Loops to matchig leaf node
    	while(!node->leaf)
		{
			node = next_node(node,key);	
		}

		item_insert(node, key, data);

		// Checks for overflow in current node
    	if(node->entries >= degree_of_tree)
    	{ 
			overflow_split(node, degree_of_tree, separator, false);

			while( ((struct _node*)node->pointer_parent)->entries >= degree_of_tree) 
			// Checks for overflow in parent node
			{
				
				// Checks if parent node is the root
				if( ((struct _node*)node->pointer_parent)->root == false )
				{
					overflow_split(node->pointer_parent, degree_of_tree, separator, true);
				}
				else if( ((struct _node*)node->pointer_parent)->root == true ) 
				{
					struct _node* ptr_parent = node->pointer_parent;
					struct _node* nodes[2];

					for(int i = 0; i < 2; i++)
					{
						nodes[i] = create_node(degree_of_tree);
						nodes[i]->leaf = false;
						nodes[i]->entries = degree_of_tree / 2; 
						nodes[i]->pointer_parent = node->pointer_parent; 
					}
					
					for(int i = 0; i <= separator; i++)
					{
						nodes[0]->pointer_children[i] = ptr_parent->pointer_children[i];
						nodes[0]->pointer_children[i]->pointer_parent = nodes[0];
					}
					
					for(int i = separator+1; i <= degree_of_tree; i++)
  					{
						
						nodes[1]->pointer_children[i-separator-1] = ptr_parent->pointer_children[i]; 
						nodes[1]->pointer_children[i-separator-1]->pointer_parent = nodes[1];
					}
	
					for(int i = 0; i < degree_of_tree+1; i++)
					{
						ptr_parent->pointer_children[i] = NULL;
					}
				
					leftside_copy_node(ptr_parent, nodes[0], separator);
					rightside_copy_node(ptr_parent, nodes[1], separator, degree_of_tree);

					qsort(ptr_parent->items , degree_of_tree, sizeof(struct _item), compare_func_sort);

					ptr_parent->entries = 1;

					for(int i = 0; i < 2; i++)
					{	
						ptr_parent->pointer_children[i] = nodes[i];	
					}		
				}
				node = node->pointer_parent;
			}
		}	
	}
	return BTREE_ERROR_SUCCESS;
}

// Function to destroy the tree
struct _node* leaf_destroy(struct _node* node)
{
	struct _node* node_parent;
	
	while(!node->pointer_children[0]->leaf)
	{
		node = node->pointer_children[0];
	}
	
	for(int i = 0; i <= node->entries; i++)
	{
		free(node->pointer_children[i]->items);
		free(node->pointer_children[i]);
	}
	
	// Special handling for tree with no internal node | depth: 2
	if(node->root)
	{
		node->entries = -1;
		return node;
	}

	node_parent = node->pointer_parent;
	node_parent->entries--; 
	
	free(node->items);
	free(node);
	
	return node_parent;	
}

// Function to destroy the tree | remove input node and return parent node 
struct _node* parent_negative(struct _node* node)
{	
	struct _node* node_del = node;

	node = node->pointer_parent;
	node->entries--;
	free(node_del->items);
	free(node_del);

	return node;
}

// Function to destroy the whole tree | user function
bt_error_t destroy_btree(struct _node* node)
{
	// begin on left side child[0] until child node is leaf? 
	// delete this node and go in parent
	// check entries of parent node for more children (local count compare to entries of node)
	// have parent node also no childs delelte parent and go up in parent of parent ! it shoud be checked if parent is root!

	int error = check_root_pointer(node);
	if(error != 0)
		return error;

	struct _node* head = node;
	if(!head->leaf)
	{
		while(head->entries > -1)
		{
			node = leaf_destroy(node);

			if(node->entries < 0 && !node->root) // means no childs left because entries 0 can hold one child
			{	
				do{
					node = parent_negative(node);
				}while(node->entries < 0 && !node->root);
			}
			if(!node->leaf)
				node = node->pointer_children[node->entries+1];
		}
	}	
	// At the end also free the hole root with items and the config
	free(head->items);
	free(head->pointer_parent);
  	free(head);

	return BTREE_ERROR_SUCCESS;	
}

// Function that searchs the node and index of a given key
struct _find find_node(struct _node* node, unsigned int key)
{
	bool hit_leaf = false;
	struct _find find;

	// It loops as long as it hits a leaf node or found the given key
	while(!hit_leaf)
	{		
		if(node->leaf)
			hit_leaf = true;	
		
		for(int i = 0; i <= node->entries; i++)
		{	
			// If the key was found returns the node with the index of the item in the node
			if(node->items[i].key == key)
			{
				find.node = node;
				find.index = i;
				return find;
			}	
		}
		node = next_node(node, key);
	}
	// Return this when it can't find the key in the tree
	find.node = NULL;
	find.index = UINT_MAX;
	return find;
}

// Function for user to search data in tree
struct _item* btree_find_item(struct _node* node, unsigned int key)
{
	// Checks the given pointer to the root of the tree
	int error = check_root_pointer(node);
	if(error != 0)
		return NULL;

	struct _find find = find_node(node, key);
	if(find.node == NULL)
		return NULL;
	return &find.node->items[find.index];
}

// Deletes an item from the given node on the given index and if it was set, also sort the node
void delete_item(struct _node* node, int index, int degree_of_tree, bool sort)
{
	node->items[index].key = UINT_MAX;
	node->items[index].data = NULL;
	node->entries--;
	if(sort)
		qsort(node->items, degree_of_tree, sizeof(struct _item), compare_func_sort);
}

// Function that shifts the child pointer 
void shift_child_pointer_left(struct _node* node, int own_index, int degree_of_tree)
{
	for(int i = own_index-1; i < degree_of_tree  ;i++)
	{
		node->pointer_children[i] = node->pointer_children[i+1];
	}
	node->pointer_children[degree_of_tree] = NULL;
}

// Underflow function which merge two nodes
void merge_nodes(struct _node* node, struct _node* sibling_node, int own_index, int degree_of_tree)
{
	struct _node* ptr_parent = node->pointer_parent;
	
	sibling_node->items[sibling_node->entries].key = ptr_parent->items[own_index-1].key;
	sibling_node->items[sibling_node->entries].data = ptr_parent->items[own_index-1].data;
	sibling_node->entries++;
	
	for(int i = 0; i < sibling_node->entries; i++)
	{	
		node->items[node->entries].key = sibling_node->items[i].key;
		node->items[node->entries].data = sibling_node->items[i].data;
		node->entries++;
	}
	qsort(node->items, degree_of_tree, sizeof(struct _item), compare_func_sort);
	shift_child_pointer_left(ptr_parent, own_index, degree_of_tree);
	delete_item(ptr_parent, own_index - 1, degree_of_tree, true);
	
	// Handling for internal nodes
	if(!node->leaf && !node->root)
	{	
		for(int i = node->entries + sibling_node->entries-1; i >= sibling_node->entries; i--)
		{		
			node->pointer_children[i] = node->pointer_children[i-sibling_node->entries]; 
		}
		
		for(int i = 0; i <= sibling_node->entries-1; i++)
		{
			node->pointer_children[i] = sibling_node->pointer_children[i];
			sibling_node->pointer_children[i]->pointer_parent = node;
		}
	}
	
	free(sibling_node->items);
	free(sibling_node);

	// Handling if root is empty, because the last item was used for a merge
	if(ptr_parent->root && ptr_parent->entries < 1)
	{
		for(int i = 0; i <= node->entries; i++)
		{
			ptr_parent->pointer_children[i] = node->pointer_children[i];
			if(!node->leaf)
		 		node->pointer_children[i]->pointer_parent = ptr_parent;
		}
		
		for(int i = 0; i < node->entries; i++)
		{
			ptr_parent->items[i] = node->items[i];
		}

		ptr_parent->entries = node->entries;
		
		// Sets root as leaf when node was already a leaf
		if(node->leaf)
			ptr_parent->leaf = true;

		free(node->items);
		free(node);
	}
}
// Undeflow function : rotate key from left sibling
void rotate_left_sibling(struct _node* node, int own_index, int degree_of_tree)
{
	//remove key from node | write 'biggest' key from left sibling in parent and key from parent in node
	struct _node* ptr_parent = node->pointer_parent;
	struct _node* left_sibling = ptr_parent->pointer_children[own_index-1];
	unsigned int key_from_parent = ptr_parent->items[own_index-1].key;
	void* data_from_parent = ptr_parent->items[own_index-1].data;

	item_insert(node, key_from_parent, data_from_parent);
					
	ptr_parent->items[own_index-1].key = left_sibling->items[left_sibling->entries-1].key;
	ptr_parent->items[own_index-1].data = left_sibling->items[left_sibling->entries-1].data;

	delete_item(left_sibling, left_sibling->entries-1, degree_of_tree, false);

	if(!node->leaf)
	{
		for(int i = node->entries; i >= 0; i--)
		{
			node->pointer_children[i+1] = node->pointer_children[i];
		}

		node->pointer_children[0] = left_sibling->pointer_children[left_sibling->entries+1];
		left_sibling->pointer_children[left_sibling->entries+1]->pointer_parent = node;
		left_sibling->pointer_children[left_sibling->entries+1] = NULL;
	}					
}
// Underflow function: rotate key from right sibling
void rotate_right_sibling(struct _node* node, int own_index, int degree_of_tree)
{
	struct _node* ptr_parent = node->pointer_parent;
	struct _node* right_sibling = ptr_parent->pointer_children[own_index+1];

	// Moves items from right sibling in parent and from parent in node
	node->items[node->entries].key = ptr_parent->items[own_index].key;
	node->items[node->entries].data = ptr_parent->items[own_index].data;
	node->entries++;
	ptr_parent->items[own_index].key = right_sibling->items[0].key;
	ptr_parent->items[own_index].data = right_sibling->items[0].data;

	delete_item(right_sibling, 0, degree_of_tree, true);

	if(!node->leaf)
	{
		right_sibling->pointer_children[0]->pointer_parent = node;
		node->pointer_children[node->entries] = right_sibling->pointer_children[0];
		
		shift_child_pointer_left(right_sibling, 1, degree_of_tree);
	}
}


struct _node* delete_decision(struct _node* node, int item_index, int degree_of_tree)
{
	if(!node->leaf)
	{	
		struct _node* head = node;
		
		node = node->pointer_children[item_index];
		while(!node->leaf)
		{
			node = node->pointer_children[node->entries];
		}
		
		head->items[item_index].key = node->items[node->entries-1].key;
		head->items[item_index].data = node->items[node->entries-1].data;

		delete_item(node, node->entries-1, degree_of_tree, false);
		return node;
	}
	else
	{	
		delete_item(node, item_index, degree_of_tree, true);
		return node;
	}
}

// Function that handle the differnt cases for underflow
void delete_underflow_handling(struct _node* node, int own_index, int min_before_underflow, int degree_of_tree)
{
	struct _node* ptr_parent = node->pointer_parent;
	
	// Node with underflow isn't a side leaf node
	if(own_index > 0 && own_index < ptr_parent->entries)
	{
		
		// Left sibling node
		if(ptr_parent->pointer_children[own_index-1]->entries > min_before_underflow)	
		{	
			rotate_left_sibling(node, own_index, degree_of_tree);
		}
		// Right sibling
		else if(ptr_parent->pointer_children[own_index+1]->entries > min_before_underflow)
		{	
			rotate_right_sibling(node, own_index, degree_of_tree);
		}
		// No sibling for rotate - merge with left sibling
		else
		{	
			struct _node* left_sibling = ptr_parent->pointer_children[own_index-1];
			merge_nodes(node, left_sibling, own_index, degree_of_tree);		
		}
	}
	// Outside left leaf
	else if(own_index == 0)
	{
		// Right sibling
		if(ptr_parent->pointer_children[own_index+1]->entries > min_before_underflow)
		{
			rotate_right_sibling(node, own_index, degree_of_tree);
		}
		// Right sibling can't rotate - merge with right sibling
		else
		{										
			struct _node* sibling_node = ptr_parent->pointer_children[own_index+1];
			merge_nodes(sibling_node, node, own_index+1, degree_of_tree);
		}
	}
	// Outside right leaf
	else if(own_index == ptr_parent->entries)
	{
		// Left sibling
		if(ptr_parent->pointer_children[own_index-1]->entries > min_before_underflow)
		{
			rotate_left_sibling(node, own_index, degree_of_tree);
		}
		// Left sibling can't rotate - merge with left sibling
		else
		{	
			struct _node* sibling_node = ptr_parent->pointer_children[own_index-1];
			merge_nodes(node, sibling_node, own_index, degree_of_tree);
		}
	}
}


// User function: deletes an item from the tree
bt_error_t btree_delete_item(struct _node* node, unsigned int key)
{
	// Checks the given pointer to the root of the tree
	int error = check_root_pointer(node);
	if(error != 0)
		return error;

	int degree_of_tree  = ((struct _config*)node->pointer_parent)->degree_of_tree;
	int min_before_underflow = ((degree_of_tree % 2) + degree_of_tree) / 2 - 1;
	int own_index;
	struct _node* ptr_parent;

	struct _find node_found = find_node(node, key);
	node = node_found.node;
	
	// Leave function if item with key does not exists
	if(node == NULL)
		return BTREE_ERROR_IND_NOT_FOUND;	

	node = delete_decision(node, node_found.index, degree_of_tree);
	while(node->entries < min_before_underflow && !node->root)
	{
		
		// Find node ptr in pointer_children of parent for index to get ptr of siblings
		ptr_parent = node->pointer_parent;
		for(int i = 0; i <= ptr_parent->entries; i++)
		{
			if(ptr_parent->pointer_children[i] == node)
			{
				own_index = i;
			}
		}

		delete_underflow_handling(node, own_index, min_before_underflow, degree_of_tree);
		if(ptr_parent->entries > min_before_underflow)
		{
			// If parent has no underflow return at this point
			return BTREE_ERROR_SUCCESS;
		}
		
		node = ptr_parent;	
	}
	return BTREE_ERROR_SUCCESS;
}


