#include <netinet/in.h>

typedef struct i_ip_s
{
  char *str;
  struct in_addr addr;
} i_ip;

/* ip.c */

i_ip *i_ip_create ();
void i_ip_free (void *ipptr);
i_ip* i_ip_duplicate (i_ip *ip);
char* i_ip_struct_to_data (i_ip *ip, int *datasizeptr);
i_ip* i_ip_data_to_struct (char *data, int datasize);
