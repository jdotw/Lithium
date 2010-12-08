#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include "device/avail.h"

#include "xserve_intel.h"
#include "ipmitool.h"

/* IPMItool Apple Proprietary */

char* v_ipmitool_string (void *data, size_t datasize, size_t offset)
{
  /* Returns the string in the datablock at offset */
  char *dataptr = data;
  uint8_t size_octet;
  memcpy (&size_octet, dataptr + offset, 1);
  size_t strsize = (size_t) size_octet;
  if (strsize < 1) 
  { i_printf (2, "v_ipmitool_string strsize < 1 (%li)", strsize); return NULL; }
  if (offset + strsize >= datasize) 
  { i_printf (2, "v_ipmitool_string avoided overflow (%li + %li >= %li)", offset, strsize, datasize); return NULL; }
  char *str = malloc (strsize);
  memcpy (str, (data + offset + 1), strsize);
  return str;
}

uint32_t v_ipmitool_uint32 (void *data, size_t datasize, size_t offset)
{
  char *dataptr = data;
  if ((offset + 4) >= datasize)
  { i_printf (2, "v_ipmitool_uint32 avoided overflow (%li + 4) > %li", offset, datasize); return 0; }
  char *hex;
  uint8_t octet_1 = (uint8_t) dataptr[offset + 3];
  uint8_t octet_2 = (uint8_t) dataptr[offset + 2];
  uint8_t octet_3 = (uint8_t) dataptr[offset + 1];
  uint8_t octet_4 = (uint8_t) dataptr[offset];
  asprintf (&hex, "0x%.2x%.2x%.2x%.2x", octet_1, octet_2, octet_3, octet_4);
  uint32_t value = strtoul (hex, NULL, 16);
  free (hex);
  return value;
}

int v_ipmitool_octet (void *data, size_t datasize, size_t offset)
{
  char *dataptr = data;
  uint8_t size_octet;
  memcpy (&size_octet, dataptr + offset, 1);
  return size_octet;
}
