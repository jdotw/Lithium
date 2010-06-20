#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/user.h>
#include <induction/data.h>
#include <induction/notification.h>
#include <induction/contact.h>

#include "gsmdevice.h"
#include "sms.h"

int l_sms_send_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* This handler is called when a MSG_SMS_SEND message is received.
   *
   * This is a persistent handler, always return 0
   */

  int num;
  int offset;
  int user_datasize;
  int note_datasize;
  char *dataptr;
  char *user_data;
  char *note_data;
  char *message = NULL;
  i_user *user;
  i_notification *note;
  l_gsmdevice *device = passdata;
  
  if (!self || !msg) return -1;

  dataptr = msg->data;
  
  user_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &user_datasize, &offset);
  if (offset < 1 || !user_data)
  { i_printf (1, "l_sms_send_handler failed to get user data from msg data"); return 0; }
  dataptr += offset;

  note_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &note_datasize, &offset);
  if (offset < 1 || !note_data)
  { i_printf (1, "l_sms_send_handler failed to get note data from msg data"); if (user_data) free (user_data); return 0; }
  dataptr += offset;

  user = i_user_data_to_struct (user_data, user_datasize);
  if (user_data) free (user_data);
  if (!user)
  { i_printf (1, "l_sms_send_handler failed to convert userdata to user"); if (note_data) free (note_data); return 0; }

  note = i_notification_data_to_struct (note_data, note_datasize);
  if (note_data) free (note_data);
  if (!user)
  { i_printf (1, "l_sms_send_handler failed to convert userdata to user"); return 0; }

  if (!user->contact || !user->contact->mobile || !user->contact->mobile->phone || strlen(user->contact->mobile->phone) < 1)
  {
    i_printf (1, "l_sms_send_handler called with a NULL mobile phone entry in the contact profile");
    i_user_free (user);
    i_notification_free (note);
  }

  if (note->subject)
    message = strdup (note->subject);

  if (!note->subject && note->message)
    message = strdup (note->message);

  if (!message)
    message = strdup ("Blank");

  i_notification_free (note);

  num = l_sms_send (self, device, user->contact->mobile->phone, message);
  if (num != 0)
  { 
    i_printf (1, "l_sms_send_handler failed to send SMS (message: %s) (number: %s)", message, user->contact->mobile->phone); 
    i_user_free (user);
    free (message);
    return -1; 
  }
  i_user_free (user);
  free (message);
  
  return 0;
}
