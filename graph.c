#include <ncurses.h>
#include "edit_file.h"
#include "graph.h"


//print data and menu
void print_data(file_info* file, unsigned* max_row, unsigned* max_col, int* y, int* x)
{
	getmaxyx(stdscr, *max_row, *max_col);

	unsigned i = 0;
	for (i = 0; i < file->line_index_total; i++)
    	addstr(file->data[i]);

	mvaddstr(*max_row - 1, 1, "F1 - Exit");
	mvaddstr(*max_row - 1, 15, "F2 - Save");

	//For debug
    mvprintw(*max_row - 3, 1, "Y = %d", *y);
    mvprintw(*max_row - 3, 10, "X = %d", *x);
    mvprintw(*max_row - 4, 1, "line_index = %d", file->line_index[*y]);

	move(*y, *x);
	refresh();
}

//Cursor movement, save and exit
void cursor_handler(file_info* file, int user_char, int* y, int* x, unsigned* exit, unsigned* max_row, unsigned* max_col)
{
	switch (user_char)
	{
	    case KEY_UP:
	        if (*y)
	            if (file->line_index[*y-1] > *x)
	                (*y)--; 
	            else
	            {
	                *x = file->line_index[*y-1];
	                (*y)--;
	            }
	        break;
	    case KEY_DOWN:
	        if (*y != *max_row && *y < (file->line_index_total - 1))
	            if (file->line_index[*y+1] > *x)
	                (*y)++;
	            else
	            {
	                *x = file->line_index[*y+1];
	                (*y)++;
	            }
	        break;
	    case KEY_LEFT:
	        if (*x)
	            (*x)--; 
	        break;
	    case KEY_RIGHT:
	        if (*x != *max_col && *x <= (file->line_index[*y] - 1))
	            (*x)++; 
	        break;
	    case KEY_F(1):
	    {
	        *exit = 1;
	        break;
	    }
	    case KEY_F(2):
	    {
	        save_file(file);
	        break;
	    }
	    default:
	    {
	        refresh_data(file, y, x, user_char);
	        break;
	    }
	}
}