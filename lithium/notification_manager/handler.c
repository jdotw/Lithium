#include <stdlib.h>

#include <induction.h>

#include "handler.h"
#include "email.h"
#include "sms.h"

int l_notification_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a MSG_NOTIFICATION type message is recieved 
   *
   * This is a persistent callback, always return 0
   */

  i_notification *note;
  i_list *userlist;
  i_user *user;
  char *res_addr_str;

  if (!msg || !msg->data || msg->datasize < 1) return 0;
  
  note = i_notification_data_to_struct (msg->data, msg->datasize);
  if (!note)
  {
    i_printf (1, "l_notification_handler failed to convert msg->data to note");
    return 0;
  }

  userlist = i_userdb_get_all (self);
  if (!userlist)
  {
    i_printf (1, "l_notification_handler failed to get user list whilst processing a notification");
    return 0;
  }

  for (i_list_move_head(userlist); (user=i_list_restore(userlist))!=NULL; i_list_move_next(userlist))
  {
    /* Loop through each user and send the appropriate notification */

    int send_sms = 0;
    struct timeval now;
    struct tm *now_tm;
    
    l_email_send (self, user, note);    /* Always send an email */
  
    /* Check if the users hours permit an SMS being sent at this time */

    if (!user->contact) 
    { i_printf (1, "l_notification_handler a user in the userlist did not have a contact profile, continuing"); continue; }

    if (user->contact->hours == HOURS_24x7)
    {
      send_sms = 1;
    }

    if (user->contact->hours == HOURS_8_6x5)
    {
      /* 8am-6pm, Mon-Fri */
      gettimeofday (&now, NULL);
      now_tm = localtime (&now.tv_sec);
      if (now_tm->tm_wday != 0 && now_tm->tm_wday != 6) 
      {
        /* It's not saturday and its not sunday */
        if (now_tm->tm_hour >= 8 && now_tm->tm_hour <= 18)
        {
          /* And it's between 8am and 6 (1800) pm */
          send_sms = 1;
        }
      }
    }

    if (send_sms == 1)
    { l_sms_send (self, user, note); }
  }

  i_notification_free (note);
  
  return 0;
}
