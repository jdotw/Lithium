#include <stdlib.h>

#include <induction.h>

int l_sms_send (i_resource *self, i_user *user, i_notification *note)
{
  long msgid;

  if (!self || !user || !note) return -1;
  if (!user->contact || !user->contact->mobile || !user->contact->mobile->phone || strlen(user->contact->mobile->phone) < 1)
  { i_printf (1, "l_sms_send called for a user with no mobile phone contact details"); return -1; }    
  
  msgid = i_sms_send (self, user, note);
  if (msgid == -1)
  {
    i_printf (1, "l_sms_send failed to send SMS message");
    return -1;
  }

  return 0;
}
