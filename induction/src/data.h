char* i_data_add_int (char *data, char *dataptr, int datasize, int *intptr);
char* i_data_add_uint (char *data, char *dataptr, int datasize, unsigned int *uintptr);
char* i_data_add_ushort (char *data, char *dataptr, int datasize, unsigned short *ushortptr);
char* i_data_add_short (char *data, char *dataptr, int datasize, short *ushortptr);
char* i_data_add_long (char *data, char *dataptr, int datasize, long *longptr);
char* i_data_add_double (char *data, char *dataptr, int datasize, double *dblptr);
char* i_data_add_chunk (char *data, char *dataptr, int datasize, char *chunkptr, int chunksize);
char* i_data_add_string (char *data, char *dataptr, int datasize, char *str);

int i_data_get_int (char *data, char *dataptr, int datasize, int *offsetptr);
unsigned int i_data_get_uint (char *data, char *dataptr, int datasize, int *offsetptr);
unsigned short i_data_get_ushort (char *data, char *dataptr, int datasize, int *offsetptr);
short i_data_get_short (char *data, char *dataptr, int datasize, int *offsetptr);
long i_data_get_long (char *data, char *dataptr, int datasize, int *offsetptr);
double i_data_get_double (char *data, char *dataptr, int datasize, int *offsetptr);
char* i_data_get_chunk (char *data, char *dataptr, int datasize, int *chunksize_ptr, int *offsetptr);
char* i_data_get_string (char *data, char *dataptr, int datasize, int *offsetptr);
