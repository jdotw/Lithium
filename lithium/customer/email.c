#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include <induction.h>
#include <induction/user.h>
#include <induction/contact.h>
#include <induction/notification.h>

int l_email_send (i_resource *self, i_user *user, i_notification *note)
{
  /* Send the notification to the user via email */

  int num;
  unsigned int written;
  int fd;
  char *file;
  char *command;
  char *text;

  if (!self || !user || !note) return -1;
  if (!user->contact || !user->contact->email || strlen(user->contact->email) < 1)
  { i_printf (2, "l_email_send called for a user with no email contact details"); return -1; }

  /* ORANGE PAGER HACK */
  if (strstr(user->contact->email, "orange"))
  {
    /* User is a pager user, check hours */
    if (user->contact->hours == HOURS_8_6x5)
    {
      int send_page = 0;
      struct timeval now;
      struct tm *now_tm;
 
      /* 8am-6pm, Mon-Fri */
      gettimeofday (&now, NULL);
      now_tm = localtime ((time_t *) &now.tv_sec);
      if (now_tm->tm_wday != 0 && now_tm->tm_wday != 6)
      {
        /* It's not saturday and its not sunday */
        if (now_tm->tm_hour >= 8 && now_tm->tm_hour < 18)
        {
          /* And it's between 8am and 6 (1800) pm */
          send_page = 1;
        }
      }
      
      if (send_page == 0) return 0; /* Dont send the page if it's out of hours */
    }
    asprintf (&text, "%s", note->subject);
  }
  else
  {
    if (!note->message) 
    { asprintf (&text, "No message text"); }
    else
    { asprintf (&text, "%s", note->message); }
  }
  /* END ORANGE PAGER HACK */
  
  asprintf (&file, "%s/email-%li", self->root, random());
  fd = open (file, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
  if (fd < 0)
  { i_printf (1, "l_email_send failed to create temporary email file (%s)"); free (file); return -1; }

  written = write (fd, text, strlen(text));
  close (fd);
  if (written < strlen(text))
  { 
    i_printf (1, "l_email_send failed to write message text to file"); 
    free (text);
    free (file); 
    return -1; 
  }
  free (text);

  /* ORANGE PAGER HACK */
  if (strstr(user->contact->email, "orange"))
  { asprintf (&command, "/usr/bin/mail -s \"%s\" \"%s\" < %s; rm %s &", "", user->contact->email, file, file); }
  else
  { asprintf (&command, "/usr/bin/mail -s \"%s\" \"%s\" < %s; rm %s &", note->subject, user->contact->email, file, file); }
  /* END ORANGE PAGER HACK */

  num = system (command);
  if (num == -1)
  { i_printf (1, "l_email_send failed to send email, system returned -1"); }
  free (command);
  free (file);

  return 0;
}

