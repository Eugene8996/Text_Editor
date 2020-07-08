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

} file_info;

void open_file(file_info* file);
void refresh_file(file_info* file, unsigned y, unsigned x, int user_char);

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
        mvprintw(max_row - 3, 1, "%d", y);
        move(y, x);
        refresh();

        user_char = getch();

        switch (user_char)
        {
            case KEY_UP:
                if (y)
                    y--; 
                break;
            case KEY_DOWN:
                if (y != max_row)
                    y++;
                break;
            case KEY_LEFT:
                if (x)
                    x--; 
                break;
            case KEY_RIGHT:
                if (x != max_col)
                    x++; 
                break;
            case KEY_F(1):
            {
                exit = 1;
                break;
            }
            default:
            {
                refresh_file(&file, y, x, user_char);
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
    unsigned j = 0, i = 0;
    

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

    fseek(fp, 0, SEEK_END);
    file->size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = malloc(file->size + 1);
    fread(buffer, 1, file->size, fp);
    buffer[file->size] = 0;


    for (i = 0; i < file->size; i++)
    {
        if (buffer[i] == '\n')
            file->line_index_total = file->line_index_total + 1;                          //Колиство перевода строк
    }
    
    file->data = (char **)calloc(file->line_index_total, sizeof(char *));



    for (i = 0; i < file->size; i++)
    {
        if (buffer[i] == '\n')
        {
            line_index_cur = i;
            string_size = line_index_cur - line_index_prev;
            file->data[j] = calloc(string_size + 1, sizeof(char));
            memcpy(file->data[j], &buffer[line_index_prev], string_size);
            line_index_prev = line_index_cur;
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


void refresh_file(file_info* file, unsigned y, unsigned x, int user_char)
{
    size_t line_size;
    char* buffer;

    line_size = strlen(file->data[y]);
    buffer = calloc(line_size + 1, sizeof(char));
    memcpy(buffer, file->data[y], x);
    buffer[x] = (char)user_char;
    memcpy(&buffer[x+1], &file->data[y][x], strlen(file->data[y]) - x);

    free(file->data[y]);
    file->data[y] = calloc(line_size + 1, sizeof(char));
    memcpy(file->data[y], buffer, line_size + 1);

}