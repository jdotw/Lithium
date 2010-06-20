#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <induction.h>

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
  { i_printf (1, "l_email_send called for a user with no email contact details"); return -1; }

  if (!note->message) 
  { asprintf (&text, "No message text"); }
  else
  { asprintf (&text, "%s", note->message); }
  
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
 
  asprintf (&command, "/usr/bin/mail -s \"%s\" \"%s\" < %s", note->subject, user->contact->email, file);
  num = system (command);
  if (num == -1)
  { i_printf (1, "l_email_send failed to send email, system() returned -1"); }
  free (command);
  unlink (file);
  free (file);

  return 0;
}

