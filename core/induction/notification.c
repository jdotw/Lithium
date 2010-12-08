#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "notification.h"
#include "customer.h"
#include "data.h"
#include "message.h"

void i_notification_free (void *noteptr)
{
  i_notification *note = noteptr;

  if (!note) return;

  if (note->subject) free (note->subject);
  if (note->message) free (note->message);
  if (note->owner) i_resource_free_address (note->owner);

  free (note);
}

i_notification* i_notification_create (char *subject, char *message, i_resource_address *owner)
{
  i_notification *note;

  note = (i_notification *) malloc (sizeof(i_notification));
  if (!note)
  { i_printf (1, "i_notification_create failed to malloc note"); return NULL; }
  memset (note, 0, sizeof(i_notification));

  gettimeofday (&note->timestamp, NULL);
  
  if (subject) note->subject = strdup (subject);
  if (message) note->message = strdup (message);
  if (owner) note->owner = i_resource_address_duplicate (owner);

  return note;
}

long i_notification_send (i_resource *self, i_notification *note)
{
  int datasize;
  char *data;
  long msgid;
  i_resource_address *customer_addr;

  data = i_notification_struct_to_data (note, &datasize);
  if (!data || datasize < 1)
  { i_printf (1, "i_notification_send failed to convert notification to data"); return -1; }

  customer_addr = i_customer_get_resaddr_by_id (self, self->customer_id);
  if (!customer_addr)
  { i_printf (1, "i_notification_send failed to get customer_addr from i_customer_get_resaddr_by_id"); free (data); return -1; }
  
  msgid = i_message_send (self, MSG_NOTIFICATION, data, datasize, customer_addr, MSG_FLAG_REQ, 0);
  i_resource_address_free (customer_addr);
  free (data);
  if (msgid == -1)
  { i_printf (1, "i_notification_send failed to send notification to customer resource"); return -1; }

  return msgid;
}

char* i_notification_struct_to_data (i_notification *note, int *datasizeptr)
{
  /* Data format is as follows :
   *
   * int owner_str_size
   * char *owner_str
   * int subject_size
   * char *subject
   * int message_size
   * char *message
   *
   * long timestamp_sec
   * long timestamp_usec
   *
   */

  int datasize;
  char *data;
  char *dataptr;
  char *owner_str;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  if (!note) return NULL;

  if (note->owner) owner_str = i_resource_address_struct_to_string (note->owner);
  else owner_str = NULL;
  
  datasize = (3*sizeof(int)) + (2*sizeof(long));
  if (note->subject) datasize += strlen(note->subject)+1;
  if (note->message) datasize += strlen(note->message)+1;
  if (owner_str) datasize += strlen(owner_str)+1;

  data = (char *) malloc (datasize);
  if (!data)
  { 
    i_printf (1, "i_notification_struct_to_data failed to malloc data"); 
    if (owner_str) free (owner_str);
    return NULL;
  }
  memset (data, 0, datasize);
  dataptr = data;
  
  dataptr = i_data_add_string (data, dataptr, datasize, owner_str);
  if (owner_str) free (owner_str);
  if (!dataptr)
  { i_printf (1, "i_notification_struct_to_data failed to add owner_str to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, note->subject);
  if (!dataptr)
  { i_printf (1, "i_notification_struct_to_data failed to add subject to data"); free (data); return NULL; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, note->message);
  if (!dataptr)
  { i_printf (1, "i_notification_struct_to_data failed to add message to data"); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &note->timestamp.tv_sec);
  if (!dataptr)
  { i_printf (1, "i_notification_struct_to_data failed to add timestamp.tv_sec to data"); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &note->timestamp.tv_usec);
  if (!dataptr)
  { i_printf (1, "i_notification_struct_to_data failed to add timestamp.tv_usec to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));
  
  return data;
}

i_notification* i_notification_data_to_struct (char *data, int datasize)
{
  int offset;
  char *dataptr = data;
  char *owner_str;
  i_notification *note;

  if (!data || datasize < 1) return NULL;

  note = i_notification_create (NULL, NULL, NULL);
  if (!note)
  { i_printf (1, "i_notification_data_to_struct failed to create note struct"); return NULL; }

  owner_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_notification_data_to_struct failed to get owner_str from data"); i_notification_free (note); return NULL; }
  dataptr += offset;

  if (owner_str) 
  { note->owner = i_resource_address_string_to_struct (owner_str); free (owner_str); }

  note->subject = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_notification_data_to_struct failed to get note->subject from data"); i_notification_free (note); return NULL; }
  dataptr += offset;

  note->message = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_notification_data_to_struct failed to get note->message from data"); i_notification_free (note); return NULL; }
  dataptr += offset;

  note->timestamp.tv_sec = i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_notification_data_to_struct failed to get note->timestamp.tv_sec from data"); i_notification_free (note); return NULL; }
  dataptr += offset;

  note->timestamp.tv_usec = i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_notification_data_to_struct failed to get note->timestamp.tv_usec from data"); i_notification_free (note); return NULL; }
  dataptr += offset;

  return note;
}


