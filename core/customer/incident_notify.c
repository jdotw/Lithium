#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/site.h>
#include <induction/contact.h>
#include <induction/user.h>
#include <induction/notification.h>
#include <induction/timeutil.h>
#include <induction/incident.h>

#include "email.h"
#include "case.h"
#include "incident.h"

int l_incident_notify (i_resource *self, i_incident *inc)
{
  /* This function is the timer callback for the
   * notification timer associated with the incident.
   * Here an email is formulated and send to report
   * the incident.
   *
   * Non-persistent timer, always return -1
   */

  char *subject;
  char *message;
  char *state_str;
  char *start_str;
  char *end_str;
  i_notification *note;
  i_site *site;
  i_list *userlist;
  i_user *user;

  /* 
   * Create subject 
   */

  if (inc->state == INCSTATE_ACTIVE)
  { state_str = i_entity_opstatestr (inc->ent->opstate); }
  else
  { state_str = "Resolved"; }
  
  asprintf (&subject, "[%s] %s Incident %li Report - %s (%s) %s %s %s %s", 
    self->ident_str, state_str, inc->id, 
    inc->ent->dev_desc, inc->ent->dev_name,
    inc->ent->cnt_desc, inc->ent->obj_desc, 
    inc->ent->met_desc, inc->ent->trg_desc);

  /*
   * Create body
   */  

  /* Get Site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), inc->ent->site_name);

  /* Times */
  if (inc->start_tv.tv_sec > 0)
  { start_str = i_time_ctime (inc->start_tv.tv_sec); }
  else
  { start_str = strdup ("N/A"); }
  if (inc->end_tv.tv_sec > 0)
  { end_str = i_time_ctime (inc->end_tv.tv_sec); }
  else
  { end_str = strdup ("N/A"); }

  /* Create Incident String */
  asprintf (&message, "Lithium %s Incident Report - %s\n\n%s %li\n%s %s\n%s %s\n%s %s (%s)\n%s %s\n%s %s\n%s %s\n%s %s\n%s %s\n%s %s\n%s %s\n",
    state_str, inc->ent->cust_desc, 
    "ID               ", inc->id,
    "Site             ", inc->ent->site_desc, 
    "Suburb           ", site->suburb_str, 
    "Device           ", inc->ent->dev_desc, inc->ent->dev_name,
    "Container        ", inc->ent->cnt_desc,
    "Object           ", inc->ent->obj_desc,
    "Metric           ", inc->ent->met_desc,
    "Trigger          ", inc->ent->trg_desc,
    "Op. State        ", i_entity_opstatestr (inc->ent->opstate),
    "Start Timestamp  ", start_str,
    "End Timestamp    ", end_str);
  free (start_str);
  free (end_str);

  /* 
   * Send email 
   */
  
  /* Create note */
  note = i_notification_create (subject, message, RES_ADDR(self));
  free (subject);
  free (message);
  if (!note)
  { i_printf (1, "l_incident_notify failed to create note struct"); return -1; }

  /* Get Userlist */
  userlist = i_user_sql_list(self);
  if (!userlist)
  {
    i_printf (1, "l_incident_notify failed to get user list whilst processing a notification");
    return -1;
  }

  /* Send */
  for (i_list_move_head(userlist); (user=i_list_restore(userlist))!=NULL; i_list_move_next(userlist))
  {
    /* Loop through each user and send notification
     * if the user has an email address contact
     */
    if (user->contact && user->contact->email)
    { l_email_send (self, user, note); }
  }

  /* 
   * Free Notification
   */
  i_notification_free (note);

  return 0;
}  

