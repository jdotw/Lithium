#include <stdlib.h>
#include <string.h>

#include "induction.h"

/* Functions for manipulating data blocks */

char* i_data_add_int (char *data, char *dataptr, int datasize, int *intptr)
{
  /* Add the int at intptr to the data block data at offset dataptr without
   * over running pass datasize. Returns a pointer to data offset to after the
   * int just added.
   */

  if (!data || !dataptr || datasize < 1 || !intptr) return NULL; 
  
  if ((dataptr + sizeof(int)) > (data + datasize))
  {
    i_printf (1, "i_data_add_int failed, overflow detected by %i bytes",
      (dataptr + sizeof(int)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, intptr, sizeof(int));
  dataptr += sizeof(int);

  return dataptr;
}

int i_data_get_int (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  int num;

  /* The value copied to offsetptr is used to decide if the func
   * was successful or not, hence it must always be updated. offset < 1 is a
   * sign the function failed
   */

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  if ((dataptr + sizeof(int)) > (data + datasize))
  {
    i_printf (1, "i_data_get_int failed, overflow detected by %i bytes",
      (dataptr + sizeof(int)) - (data + datasize));
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  memcpy (&num, dataptr, sizeof(int));
  dataptr += sizeof(int);
  offset += sizeof(int);
  memcpy (offsetptr, &offset, sizeof(int));

  return num;
}

char* i_data_add_uint (char *data, char *dataptr, int datasize, unsigned int *uintptr)
{
  /* Add the uint at intptr to the data block data at offset dataptr without
   * over running pass datasize. Returns a pointer to data offset to after the
   * int just added.
   */

  if (!data || !dataptr || datasize < 1 || !uintptr) return NULL;

  if ((dataptr + sizeof(unsigned int)) > (data + datasize))
  {
    i_printf (1, "i_data_add_uint failed, overflow detected by %i bytes",
      (dataptr + sizeof(unsigned int)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, uintptr, sizeof(unsigned int));
  dataptr += sizeof(unsigned int);

  return dataptr;
}

unsigned int i_data_get_uint (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  unsigned int uint;

  /* The value copied to offsetptr is used to decide if the func
   * was successful or not, hence it must always be updated. offset < 1 is a
   * sign the function failed
   */

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(unsigned int));
    return -1;
  }
  
  if ((dataptr + sizeof(unsigned int)) > (data + datasize))
  {
    i_printf (1, "i_data_get_uint failed, overflow detected by %i bytes",
      (dataptr + sizeof(unsigned int)) - (data + datasize));
    memset (offsetptr, 0, sizeof(unsigned int));
    return -1;
  }
  
  memcpy (&uint, dataptr, sizeof(unsigned int));
  dataptr += sizeof(unsigned int);
  offset += sizeof(unsigned int);
  memcpy (offsetptr, &offset, sizeof(unsigned int));

  return uint;
}

char* i_data_add_ushort (char *data, char *dataptr, int datasize, unsigned short *ushortptr)
{
  /* Add the ushort at ushortptr to the data block data at offset dataptr without
   * over running pass datasize. Returns a pointer to data offset to after the
   * short just added.
   */

  if (!data || !dataptr || datasize < 1 || !ushortptr) return NULL;

  if ((dataptr + sizeof(unsigned short)) > (data + datasize))
  {
    i_printf (1, "i_data_add_uint failed, overflow detected by %i bytes",
      (dataptr + sizeof(unsigned short)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, ushortptr, sizeof(unsigned short));
  dataptr += sizeof(unsigned short);

  return dataptr;
}

unsigned short i_data_get_ushort (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  unsigned short ushort;

  /* The value copied to offsetptr is used to decide if the func
   * was successful or not, hence it must always be updated. offset < 1 is a
   * sign the function failed
   */

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  if ((dataptr + sizeof(unsigned short)) > (data + datasize))
  {
    i_printf (1, "i_data_get_ushort failed, overflow detected by %i bytes",
      (dataptr + sizeof(unsigned short)) - (data + datasize));
    memset (offsetptr, 0, sizeof(unsigned short));
    return -1;
  }

  memcpy (&ushort, dataptr, sizeof(unsigned short));
  dataptr += sizeof(unsigned short);
  offset += sizeof(unsigned short);
  memcpy (offsetptr, &offset, sizeof(int));

  return ushort;
}

char* i_data_add_short (char *data, char *dataptr, int datasize, short *shortptr)
{
  /* Add the short at shortptr to the data block data at offset dataptr without
   * over running pass datasize. Returns a pointer to data offset to after the
   * short just added.
   */

  if (!data || !dataptr || datasize < 1 || !shortptr) return NULL;

  if ((dataptr + sizeof(short)) > (data + datasize))
  {
    i_printf (1, "i_data_add_uint failed, overflow detected by %i bytes",
      (dataptr + sizeof(short)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, shortptr, sizeof(short));
  dataptr += sizeof(short);

  return dataptr;
}

short i_data_get_short (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  short val;

  /* The value copied to offsetptr is used to decide if the func
   * was successful or not, hence it must always be updated. offset < 1 is a
   * sign the function failed
   */

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  if ((dataptr + sizeof(short)) > (data + datasize))
  {
    i_printf (1, "i_data_get_ushort failed, overflow detected by %i bytes",
      (dataptr + sizeof(short)) - (data + datasize));
    memset (offsetptr, 0, sizeof(short));
    return -1;
  }

  memcpy (&val, dataptr, sizeof(short));
  dataptr += sizeof(short);
  offset += sizeof(short);
  memcpy (offsetptr, &offset, sizeof(int));

  return val;
}


char* i_data_add_long (char *data, char *dataptr, int datasize, long *longptr)
{
  if (!data || !dataptr || datasize < 1 || !longptr) return NULL;

  if ((dataptr + sizeof(long)) > (data + datasize))
  {
    i_printf (1, "i_data_add_long failed, overflow detected by %i bytes",
      (dataptr + sizeof(long)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, longptr, sizeof(long));
  dataptr += sizeof(long);

  return dataptr;          
}

long i_data_get_long (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  long num;

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  if ((dataptr + sizeof(long)) > (data + datasize))
  {
    i_printf (1, "i_data_get_long failed, overflow detected by %i bytes",
      (dataptr + sizeof(long)) - (data + datasize));
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  memcpy (&num, dataptr, sizeof(long));
  dataptr += sizeof(long);
  offset += sizeof(long);
  memcpy (offsetptr, &offset, sizeof(int));

  return num;                  
}

char* i_data_add_double (char *data, char *dataptr, int datasize, double *dblptr)
{
  if (!data || !dataptr || datasize < 1 || !dblptr) return NULL;

  if ((dataptr + sizeof(double)) > (data + datasize))
  {
    i_printf (1, "i_data_add_double failed, overflow detected by %i bytes",
      (dataptr + sizeof(double)) - (data + datasize));
    return NULL;
  }

  memcpy (dataptr, dblptr, sizeof(double));
  dataptr += sizeof(double);

  return dataptr;
}

double i_data_get_double (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int offset = 0;
  double num;

  if (!data || !dataptr || datasize < 1 || !offsetptr)
  {
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  if ((dataptr + sizeof(double)) > (data + datasize))
  {
    i_printf (1, "i_data_get_double failed, overflow detected by %i bytes",
      (dataptr + sizeof(double)) - (data + datasize));
    memset (offsetptr, 0, sizeof(int));
    return -1;
  }

  memcpy (&num, dataptr, sizeof(double));
  dataptr += sizeof(double);
  offset += sizeof(double);
  memcpy (offsetptr, &offset, sizeof(int));

  return num;
}

char* i_data_add_chunk (char *data, char *dataptr, int datasize, char *chunkptr, int chunksize)
{
  if (!data || !dataptr || datasize < 1) return NULL; 

  dataptr = i_data_add_int (data, dataptr, datasize, &chunksize);
  if (!data) { i_printf (1, "i_data_add_chunk unable to add chunksize int"); return NULL; }

  if (chunksize > 0 && chunkptr)
  {
    /* There is actual data */

    if ((dataptr + chunksize) > (data + datasize))
    {
      i_printf (1, "i_data_add_chunk failed, overflow detected by %i bytes",
        (dataptr + chunksize) - (data + datasize));
      return NULL;
    }
  
    memcpy (dataptr, chunkptr, chunksize);
    dataptr += chunksize;
  }

  return dataptr;
}

char* i_data_get_chunk (char *data, char *dataptr, int datasize, int *chunksize_ptr, int *offsetptr)
{
  char *chunk = NULL;
  int chunksize;
  int offset = 0;

  if (!data || !dataptr || datasize < 1 || !chunksize_ptr || !offsetptr) return NULL;
  
  chunksize = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { 
    i_printf (1, "i_data_get_chunk unable to get chunksize int"); 
    memset (chunksize_ptr, 0, sizeof(int));
    memset (offsetptr, 0, sizeof(int));
    return NULL; 
  }

  memcpy (chunksize_ptr, &chunksize, sizeof(int));
  memcpy (offsetptr, &offset, sizeof(int));
  dataptr += offset;

  if (chunksize > 0)
  {
    /* There is trailing data */

    if ((dataptr + chunksize) > (data + datasize))
    {
      i_printf (1, "i_data_get_chunk failed, overflow detected by %i bytes",
        (dataptr + chunksize) - (data + datasize));
      memset (offsetptr, 0, sizeof(int));
      return NULL;
    }

    chunk = (char *) malloc (chunksize);
    if (!chunk) 
    { 
      i_printf (1, "i_data_get_chunk unable to malloc chunk (%i bytes)", chunksize);
      memset (offsetptr, 0, sizeof(int));
      return NULL;
    }

    memcpy (chunk, dataptr, chunksize);
    dataptr += chunksize;
    offset += chunksize;
    memcpy (offsetptr, &offset, sizeof(int));
  }

  return chunk;
}

char* i_data_add_string (char *data, char *dataptr, int datasize, char *str)
{
  int str_len;

  if (!data || !dataptr || datasize < 1) return NULL;

  if (str) str_len = strlen(str) +1;
  else str_len = 0;

  return i_data_add_chunk (data, dataptr, datasize, str, str_len);
}

char* i_data_get_string (char *data, char *dataptr, int datasize, int *offsetptr)
{
  int str_len;

  if (!data || !dataptr || datasize < 1 || !offsetptr) return NULL;

  return i_data_get_chunk (data, dataptr, datasize, &str_len, offsetptr);  
}
