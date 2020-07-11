#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct 
{
    char** data;
    size_t size;
    char path[1024];
    unsigned line_index_total;
    unsigned line_index[1024];

} file_info;

void open_file(file_info* file);
void refresh_data(file_info* file, unsigned* y, unsigned* x, int user_char);
void save_file(file_info* file);

int main()
{
    file_info file;
    file.line_index_total = 0;
    unsigned exit = 0;
    unsigned y = 0, x = 0;
    unsigned i = 0;
    unsigned max_row, max_col;
    int user_char;
    
    initscr();
    keypad(stdscr, true); 

    addstr("Enter the path and file name: ");
    scanw("%s", file.path);

    open_file(&file);

    while (true)
    {
        getmaxyx(stdscr, max_row, max_col);

        for (i = 0; i < file.line_index_total; i++)
        {
            addstr(file.data[i]);
        }

        mvaddstr(max_row - 1, 1, "F1 - Exit");
        mvaddstr(max_row - 1, 15, "F2 - Save");
        //For debug
        //mvprintw(max_row - 3, 1, "%d", y);
        //mvprintw(max_row - 3, 3, "%d", x);
        move(y, x);
        refresh();

        user_char = getch();

        //Cursor movement, save and exit
        switch (user_char)
        {
            case KEY_UP:
                if (y)
                    if (file.line_index[y-1] > x)
                        y--; 
                    else
                    {
                        x = file.line_index[y-1] - 1;
                        y--;
                    }
                break;
            case KEY_DOWN:
                if (y != max_row && y < (file.line_index_total - 1))
                    if (file.line_index[y+1] > x)
                        y++;
                    else
                    {
                        x = file.line_index[y+1] - 1;
                        y++;
                    }
                break;
            case KEY_LEFT:
                if (x)
                    x--; 
                break;
            case KEY_RIGHT:
                if (x != max_col && x <= (file.line_index[y] - 1))
                    x++; 
                break;
            case KEY_F(1):
            {
                exit = 1;
                break;
            }
            case KEY_F(2):
            {
                save_file(&file);
                break;
            }
            default:
            {
                refresh_data(&file, &y, &x, user_char);
                break;
            }
        }

        clear();
        if (exit == 1)
            break;
    }
    refresh();

    free(file.data);
    endwin();
    return 0;
}

void open_file(file_info* file)
{
    FILE *fp;
    char* buffer;
    unsigned line_index_cur = 0;
    unsigned line_index_prev = 0;
    unsigned string_size;
    unsigned j = 0, i = 0, k = 0;
    

    if ((fp = fopen(file->path, "rb")) == NULL)
    {
        addstr("Can't open file\n");
        addstr("Create file\n");
        refresh();
        if ((fp = fopen(file->path, "w")) == NULL)
        {
            addstr("Cant create file\n");
            refresh();
        }
        file->size = sizeof(char)*1024;
        file->data = calloc(1024, file->size);
        addstr("Press any key...\n");
        refresh();
        getchar();
        return;
    }

    //Determining file size and reading to clipboard
    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(file->size + 1);
    fread(buffer, 1, file->size, fp);
    buffer[file->size] = 0;


    for (i = 0; i <= file->size; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == 0)
        {
            //Number of Rows
            file->line_index_total = file->line_index_total + 1;
            if (k == 0)
            {
                //index '\n'
                file->line_index[k] = i;
                j = i;
            }
            else
            {
                file->line_index[k] = i - j - 1;
                j = i;
            }
            k++;
        }
    }

    
    file->data = (char **)calloc(file->line_index_total, sizeof(char *));


    //write to two-dimensional array
    j = 0;
    for (i = 0; i <= file->size; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == 0)
        {

            if (j == 0)
            {
                line_index_cur = i;
                string_size = line_index_cur - line_index_prev + 1;
                file->data[j] = calloc(string_size + 1, sizeof(char));
                memcpy(file->data[j], &buffer[line_index_prev], string_size);
                line_index_prev = line_index_cur;
            }
            else
            {
                line_index_cur = i;
                string_size = line_index_cur - line_index_prev;
                file->data[j] = calloc(string_size + 1, sizeof(char));
                memcpy(file->data[j], &buffer[line_index_prev + 1], string_size);
                line_index_prev = line_index_cur;
            }
            j++;
        }
    }

    addstr("File read\n");
    refresh();
    getchar();
    clear();

    free(buffer);
    fclose(fp);
}


void refresh_data(file_info* file, unsigned* y, unsigned* x, int user_char)
{
    size_t line_size;
    char* buffer;
    unsigned i = 0;
    unsigned prev_line_index = 0;

    //handle key backspace
    if (user_char == KEY_BACKSPACE)
    {
        if (*x != 0)
        {
            line_size = strlen(file->data[*y]);
            buffer = calloc(line_size - 1, sizeof(char));

            memcpy(buffer, file->data[*y], *x - 1);
            memcpy(&buffer[*x - 1], &file->data[*y][*x], line_size - *x);

            free(file->data[*y]);
            file->data[*y] = calloc(line_size - 1, sizeof(char));
            memcpy(file->data[*y], buffer, line_size - 1);
            file->line_index[*y] = file->line_index[*y] - 1;
            if (*x != 0)
                *x = *x - 1;
            free(buffer);
        }
        return;
    }

    //handle key enter
    if (user_char == '\n')
    {
        for(i = file->line_index_total - 1; i > *y; i--)
        {
            line_size = strlen(file->data[i]);
            file->data[i+1] = calloc(line_size, sizeof(char));
            memcpy(file->data[i+1], file->data[i], line_size);
            file->line_index[i+1] = file->line_index[i];
            free(file->data[i]);
        }

        line_size = strlen(&file->data[*y][*x]);
        file->data[*y+1] = calloc(line_size, sizeof(char));
        memcpy(file->data[*y+1], &file->data[*y][*x], line_size);

        line_size = *x;
        buffer = calloc(line_size + 1, sizeof(char));
        memcpy(buffer, file->data[*y], line_size);
        buffer[line_size] = '\n';
        free(file->data[*y]);
        file->data[*y] = calloc(line_size + 2, sizeof(char));
        memcpy(file->data[*y], buffer, line_size + 1);

        file->line_index[*y+1] = file->line_index[*y] - *x;
        file->line_index[*y] = *x;

        file->line_index_total = file->line_index_total + 1;

        free(buffer);
        return;
    }


    //add user symbol
    line_size = strlen(file->data[*y]);
    buffer = calloc(line_size + 1, sizeof(char));
    memcpy(buffer, file->data[*y], *x);
    buffer[*x] = (char)user_char;
    memcpy(&buffer[*x+1], &file->data[*y][*x], line_size - *x);

    free(file->data[*y]);
    file->data[*y] = calloc(line_size + 1, sizeof(char));
    memcpy(file->data[*y], buffer, line_size + 1);
    file->line_index[*y] = file->line_index[*y] + 1;
    *x = *x + 1;
    return;

}

void save_file(file_info* file)
{
    FILE *fp;

    if ((fp = fopen(file->path, "w")) == NULL)
    {
        addstr("Cant save file\n");
        refresh();
        getchar();
        return;
    }

    for (unsigned i = 0; i < file->line_index_total; i++)
        fputs(file->data[i], fp);

    clear();
    addstr("File save\n");
    addstr("Press any key...\n");
    refresh();
    getchar();
}