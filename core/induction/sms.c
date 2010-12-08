#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "user.h"
#include "notification.h"
#include "data.h"
#include "message.h"
#include "sms.h"

long i_sms_send (i_resource *self, i_user *user, i_notification *note)
{
  int datasize;
  int user_datasize;
  int note_datasize;
  long msgid;
  char *data;
  char *dataptr;
  char *user_data;
  char *note_data;
  char *res_addr_str;
  i_resource_address *sms_addr;

  if (!self || !user || !note) return -1;

  user_data = i_user_struct_to_data (user, &user_datasize);
  if (!user_data || user_datasize < 1)
  { i_printf (1, "i_sms_send failed to convert user struct to data"); return -1; }

  note_data = i_notification_struct_to_data (note, &note_datasize);
  if (!note_data || note_datasize < 1)
  { i_printf(1, "i_sms_send failed to convert notification struct to data"); free (user_data); return -1; }

  datasize = (2*sizeof(int)) + user_datasize + note_datasize;
  data = (char *) malloc (datasize);
  if (!data)
  { 
    i_printf (1, "i_sms_send failed to malloc data");
    free (user_data);
    free (note_data);
    return -1;
  }
  memset (data, 0, datasize);
  dataptr = data;

  dataptr = i_data_add_chunk (data, dataptr, datasize, user_data, user_datasize);
  free (user_data);
  if (!dataptr)
  { i_printf (1, "i_sms_send failed to add user_data to data"); return -1; }
  
  dataptr = i_data_add_chunk (data, dataptr, datasize, note_data, note_datasize);
  free (note_data);
  if (!dataptr)
  { i_printf (1, "i_sms_send failed to add note_data to data"); return -1; }

  asprintf (&res_addr_str, "::%i::", RES_SMS);
  sms_addr = i_resource_address_string_to_struct (res_addr_str);
  if (!sms_addr)
  { i_printf (1, "i_sms_send failed to convert res_addr_str to sms_addr"); return -1; }
  
  msgid = i_message_send (self, MSG_SMS_SEND, data, datasize, sms_addr, MSG_FLAG_REQ, 0);
  free (data);
  i_resource_free_address (sms_addr);
  if (msgid == -1)
  { i_printf (1, "i_sms_send failed to send MSG_SMS_SEND message"); return -1; }

  return msgid;
}
