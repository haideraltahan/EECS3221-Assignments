extern void fatalerr(char *name, int lerrno, char *mssg);
extern void open_array(char *filename, array_t *arrayp, int *sizep);
extern void close_array(array_t *array, int size);
extern void set_entry(array_t array, char *name, int index, float age);
extern void get_entry(array_t array, char **name, int index, float *age);
extern void create_array(char *filename, int index, array_t *array);
extern void print_array(array_t array, int size);
extern void delete_entry(array_t array, int index);
