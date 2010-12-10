#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "induction.h"
#include "interface.h"
#include "data.h"
#include "ip.h"

/* Struct Manipulation */

i_ip* i_ip_create ()
{
  i_ip *ip;

  ip = (i_ip *) malloc (sizeof(i_ip));
  if (!ip)
  { i_printf (1, "i_ip_create failed to malloc ip struct"); return NULL; }
  memset (ip, 0, sizeof(i_ip));

  return ip;
}

void i_ip_free (void *ipptr)
{
  i_ip *ip = ipptr;

  if (!ip) return;
  if (ip->str) free (ip->str);
  free (ip);

  return;
}

i_ip* i_ip_duplicate (i_ip *ip)
{
  i_ip *dup;

  dup = i_ip_create ();
  if (!dup)
  { i_printf (1, "i_ip_duplicate failed to create dup ip struct"); return NULL; }
  if (ip->str) dup->str = strdup (ip->str);
  dup->addr.s_addr = ip->addr.s_addr;

  return dup;
}

/* Data/Struct Conversion */

char* i_ip_struct_to_data (i_ip *ip, int *datasizeptr)
{
  char *data;
  char *dataptr;
  int datasize;

  memset (datasizeptr, 0, sizeof(int));

  datasize = sizeof(int);
  if (ip->str) datasize += strlen (ip->str)+1;

  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_ip_struct_to_data failed to malloc data (%i bytes)", datasize); return NULL; }
  memset (data, 0, datasize);
  dataptr = data;
  
  dataptr = i_data_add_string (data, dataptr, datasize, ip->str);
  if (!dataptr)
  { i_printf (1, "i_ip_struct_to_data failed to add ip->ip_str to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_ip* i_ip_data_to_struct (char *data, int datasize)
{
  int offset;
  char *dataptr = data;
  i_ip *ip;

  ip = i_ip_create ();
  if (!ip)
  { i_printf (1, "i_ip_data_to_struct failed to create ip struct"); return NULL; }

  ip->str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_ip_data_to_struct failed to get str from data"); i_ip_free (ip); return NULL; }
  dataptr += offset;

  if (ip->str) ip->addr.s_addr = inet_addr (ip->str);

  return ip;
}


