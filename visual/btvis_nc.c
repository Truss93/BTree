
#include "btvis_nc.h"

struct w_coords
{
	unsigned int depth; 
	unsigned int first_win_x;
	unsigned int last_win_x;
	unsigned int win_x;
	unsigned int last_win_x_leaf;
};


// Returns the width of a given digit
int digit_offset(unsigned int digit)
{
  int node_offset = 0;
  int divider = 1;
	if(digit == 0)
		return 1;

  while(digit / divider > 0)
      {
        
        if(digit / divider > 0)
          node_offset++;
        divider = divider * 10;
      }
      return node_offset;

}

// Returns the width off all indicies (digits) in a whole node
// Important for the right x_Size of the windows
int digit_offset_mul(btree* node)
{
  int node_offset = 0;
  int divider;

  for(int i = 0; i < node->entries; i++)
  {
	  if(node->items[i].key == 0)
	  	node_offset++;
   divider = 1;
    while(node->items[i].key / divider > 0)
    {
        
      if(node->items[i].key / divider > 0)
        node_offset++;
    divider = divider * 10;
    }
  }

  return node_offset;
}

// Draws a node of the tree
void draw_node(btree* node, unsigned int depth, unsigned int x_offset, unsigned int node_width, unsigned int degree_of_tree)
{
	// If the degree of the tree grows it needs more vertical space	
	if(degree_of_tree < 6)
		degree_of_tree = 6;


	node->node_win = newwin(3, node_width, depth*degree_of_tree, x_offset);
	
	box(node->node_win, '*', '*');
	
	int pos_offset = 1;
	for(int i = 0; i < node->entries; i++)
	{
		int p_offset = digit_offset(node->items[i].key);
        pos_offset++;
        mvwprintw(node->node_win, 1, pos_offset,"%u", node->items[i].key);
        pos_offset = pos_offset + p_offset;
        refresh();
     
	}
	wrefresh(node->node_win);
	

}

// Returns the own index of an node in relation to the parent node
int get_own_ind(btree* node)
{
	
	for(int i = 0; i <= ((btree*)node->pointer_parent)->entries; i++)
	{
		if(node == ((btree*)node->pointer_parent)->pointer_children[i])
			return i;
	}
	return 0;
}

// Return most left leaf node
// This is start point to draw the whole tree 
btree* get_leaf(btree* node, struct w_coords* win_coords)
{
	bool hit_leaf = false;
	 while(!hit_leaf)
    {
    	if(node->pointer_children[0] == NULL)
      		hit_leaf = true;

    	if(node->pointer_children[0] != NULL)
		{
			node = node->pointer_children[0];
			win_coords->depth++;

		}
    }
	return node;
}

// Draws the lines between the nodes
void draw_clines(btree* node)
{
	if(node->leaf)
		return;

	unsigned int y_beg_par, x_beg_par, y_beg_child, x_beg_child;
	unsigned int y_max_par, x_max_par, x_max_child;
	unsigned int x_digit_item_offset = 1;
	unsigned int x_dist, lower, x_dir_offset;
	
	getbegyx(node->node_win, y_beg_par, x_beg_par);
	getmaxyx(node->node_win, y_max_par, x_max_par);

	
	mvaddstr(y_beg_par+y_max_par-1, x_beg_par-1, "/");
	refresh();
	mvaddstr(y_beg_par+y_max_par-1, x_beg_par+x_max_par, "\\");
	refresh();
		

	if(node->entries > 1)
	{
		for(int i = 0; i < node->entries-1; i++)
		{
			x_digit_item_offset = x_digit_item_offset + 1 + digit_offset(node->items[i].key);
			mvaddstr(y_beg_par + y_max_par-1, x_beg_par + x_digit_item_offset, "|" );
			refresh();
		}
		
	}
	

	for(int i = 0; i <= node->entries; i++)
	{
		getbegyx(node->pointer_children[i]->node_win, y_beg_child, x_beg_child);
		x_max_child = getmaxx(node->pointer_children[i]->node_win);


		if(i <= node->entries/2)
		{
			lower = i;
		}
		else if(i > node->entries/2)
		{	
			lower = -i + node->entries;	
		}

		if(i == 0)
		{
			x_dir_offset = x_beg_par-1;
		}
		else if(i == node->entries)
		{
			x_dir_offset = x_beg_par+x_max_par;
		}
		else
		{	
			
			x_dir_offset = x_dir_offset + digit_offset(node->items[i].key) +2;
		}
		
		x_dist = x_dir_offset - x_beg_child - x_max_child/2;
		
		if(x_dir_offset < x_beg_child + x_max_child/2) //Test to right side
		{
			attron(A_BOLD);
			mvhline(y_beg_par+y_max_par-1+lower, 1+x_beg_par+x_max_par, '_', x_beg_child + x_max_child/2 -x_beg_par-x_max_par);
			if(i != 0 && i != node->entries)
				mvaddstr(y_beg_par+y_max_par-1+lower, x_beg_child+(x_max_child/2)+x_dist, "\\" );
			attron(A_NORMAL);
			refresh();

		}
		else if(x_dir_offset > x_beg_child + x_max_child/2) // Test to left side
		{
			attron(A_BOLD);
			mvhline(y_beg_par+y_max_par-1+lower, x_beg_child+(x_max_child/2), '_', x_dist);
			if(i != 0 && i != node->entries)
				mvaddstr(y_beg_par+y_max_par-1+lower, x_beg_child+(x_max_child/2)+x_dist, "/" );
			attron(A_NORMAL);
			refresh();
		}
		
		refresh();
		
		mvvline(y_beg_par+y_max_par+lower , x_beg_child+x_max_child/2, '|', y_beg_child-y_beg_par-y_max_par-lower);

		refresh();
	}


}

// Draws the whole tree with nodes and lines
void draw_tree(btree* node)
{
	clear();

	unsigned int degree_of_tree = ((struct _config*)node->pointer_parent)->degree_of_tree;

	struct w_coords win_coords = 
	{
		.depth = 0
	};

		   
	bool hit_root = false;
	bool should_draw_lines = false;
	node = get_leaf(node, &win_coords);

	while(!hit_root)
	{	
		if(node->root)
			hit_root = true;

		int node_width = digit_offset_mul(node) + 4 +  (node->entries)-1;
		
				
		draw_node(node, win_coords.depth, win_coords.win_x + 1, node_width, degree_of_tree);
		if(should_draw_lines)
		{
			draw_clines(node);
			should_draw_lines = false;

		}
			
		win_coords.first_win_x = getbegx(node->node_win);
		win_coords.last_win_x = win_coords.first_win_x + getmaxx(node->node_win);

		if(node->leaf)
			win_coords.last_win_x_leaf = win_coords.first_win_x + getmaxx(node->node_win);


		int own_ind = get_own_ind(node);

		if(own_ind < ((btree*)node->pointer_parent)->entries)
		{		
			node = ((btree *)node->pointer_parent)->pointer_children[own_ind+1];
			if(!node->leaf)
				node = get_leaf(node, &win_coords);
			win_coords.win_x = win_coords.last_win_x_leaf;
		}
		else
		{	
			if(!node->root)
			{
				node = node->pointer_parent;
				win_coords.depth--;

				int first_child_x = getbegx(node->pointer_children[0]->node_win);
				win_coords.win_x =  (first_child_x + (win_coords.last_win_x - first_child_x) / 2) - (node_width / 2);
				should_draw_lines = true;
			}
		}

	}

		
}


