#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>

#include "edit_file.h"

//extern file_info file;

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
        file->line_index_total = 1;
        file->line_index[0] = 0;
        file->data = (char **)calloc(1024, sizeof(char *));
        file->size = sizeof(char)*1;
        file->data[0] = calloc(1, file->size);
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
        else
        {
            if (*y != 0)
            {
                line_size = strlen(file->data[*y-1]) - 1;
                buffer = calloc(line_size, sizeof(char));
                memcpy(buffer, file->data[*y-1], line_size);
                free(file->data[*y-1]);
                file->data[*y-1] = calloc(line_size + strlen(file->data[*y]), sizeof(char));
                memcpy(file->data[*y-1], buffer, line_size);
                memcpy(&file->data[*y-1][line_size], file->data[*y], strlen(file->data[*y]));
                //file->line_index_total = file->line_index_total - 1;
                file->line_index[*y-1] = line_size + strlen(file->data[*y]) - 1;
                free(buffer);
                free(file->data[*y]);
                file->line_index_total = file->line_index_total - 1;

                for(i = *y; i < file->line_index_total; i++)
                {
                    line_size = strlen(file->data[i+1]);
                    file->data[i] = calloc(line_size, sizeof(char));
                    memcpy(file->data[i], file->data[i+1], line_size);
                    file->line_index[i] = file->line_index[i+1];
                    free(file->data[i+1]);
                }
                if (*y == file->line_index_total)
                {
                    (*y)--;
                    *x = file->line_index[*y];
                }
                
            }
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

        if (*x == file->line_index[*y])
        {
            file->data[*y][*x] = '\n';
            file->data[*y+1] = calloc(2, sizeof(char));
            file->data[*y+1][0] = '\n';
            file->line_index_total = file->line_index_total + 1;
            file->line_index[*y+1] = 0;
            *x = 0;
            (*y)++;
            return;
        }

        line_size = strlen(&file->data[*y][*x]);
        file->data[*y+1] = calloc(line_size, sizeof(char));
        memcpy(file->data[*y+1], &file->data[*y][*x], line_size);

        line_size = *x;
        buffer = calloc(line_size + 1, sizeof(char));
        memcpy(buffer, file->data[*y], line_size);
        buffer[line_size] = '\n';
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