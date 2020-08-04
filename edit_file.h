
typedef struct 
{
    char** data;
    size_t size;
    char path[1024];
    int line_index_total;
    int line_index[1024];

} file_info;

void open_file(file_info* file);
void save_file(file_info* file);
void refresh_data(file_info* file, unsigned* y, unsigned* x, int user_char);
