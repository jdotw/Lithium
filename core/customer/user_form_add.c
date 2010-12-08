#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/user.h>
#include <induction/userdb.h>
#include <induction/contact.h>

#include "user.h"

int form_user_add (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_form_item *item;

  if (!self || !reqdata) return -1;
  
  /* Auth checking */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form creation */

  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out) 
  { i_printf (1, "form_user_add failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "useradd", "Add New User");

  i_form_entry_add (reqdata->form_out, "username", "Username", NULL);
  i_form_password_add (reqdata->form_out, "password", "Password", NULL);
  item = i_form_dropdown_create ("auth_level", "Security Level");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Security Level Dropdown", "Failed to create security level dropdown item"); return 1; }

//  asprintf (&str, "%i", AUTH_LEVEL_INFSTAT);  
//  i_form_dropdown_add_option (item, str, "Status Only", 0);
//  free (str);
  asprintf (&str, "%i", AUTH_LEVEL_RO);  
  i_form_dropdown_add_option (item, str, "Read-Only", 0);
  free (str);
  asprintf (&str, "%i", AUTH_LEVEL_USER);  
  i_form_dropdown_add_option (item, str, "Normal User", 1);
  free (str);
  asprintf (&str, "%i", AUTH_LEVEL_ADMIN);  
  i_form_dropdown_add_option (item, str, "Administrator", 0);
  free (str);

  i_form_add_item (reqdata->form_out, item);
  i_form_spacer_add (reqdata->form_out);

  i_form_entry_add (reqdata->form_out, "fullname", "Full Name", NULL);
  i_form_entry_add (reqdata->form_out, "title", "Title", NULL);
  i_form_spacer_add (reqdata->form_out);

  i_form_entry_add (reqdata->form_out, "email", "Email Address", NULL);
  i_form_entry_add (reqdata->form_out, "office_phone", "Office Phone", NULL);
  i_form_entry_add (reqdata->form_out, "mobile_phone", "Mobile Phone", NULL);

  item = i_form_dropdown_create ("hours", "Contact Hours");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Hours Dropdown", "Failed to create hours dropdown item"); return 1; }
  i_form_dropdown_add_option (item, "8_6x5", "8am - 6pm / Monday - Friday", 1);
  i_form_dropdown_add_option (item, "24x7", "24 Hours / 7 Days", 0);
  i_form_add_item (reqdata->form_out, item);

  i_form_frame_end (reqdata->form_out, "useradd");

  return 1;
}

int form_user_add_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  i_user *user;
  i_form_item *item;
  i_form_item_option *opt;

  if (!self || !reqdata) return -1;
  
  /* Auth check */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create the form */

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_user_add_submit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "useradd", "Adding New User");

  /* Create the user struct */

  user = i_user_create ();
  if (!user)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create user data structure"); return 1; }

  /* Auth info (user->auth is created by i_user_create) */

  if (!user->auth)
  { i_form_string_add (reqdata->form_out, "error", "Error", "user->auth is NULL"); i_user_free (user); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "username");
  if (!opt) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Username value not found in form"); i_user_free (user); return 1; }
  user->auth->username = strdup (opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "password");
  if (!opt) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Password value not found in form"); i_user_free (user); return 1; }
  user->auth->password = strdup (opt->data);

  user->auth->customer_id = strdup (self->hierarchy->cust_name);

  item = i_form_find_item (reqdata->form_in, "auth_level");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find security level info in form"); i_user_free (user); return 1; }
  opt = i_form_dropdown_get_selected_value (item);
  if (opt)
  { user->auth->level = atoi ((char *) opt->data); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find security level value in form"); i_user_free (user); return 1; }

  /* Fullname / Title */

  opt = i_form_get_value_for_item (reqdata->form_in, "fullname");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Fullname", "Fullname value not found in form"); i_user_free (user); return 1; }
  user->fullname = strdup (opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "title");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Title", "Title value not found in form"); i_user_free (user); return 1; }
  user->title = strdup (opt->data);

  /* Contact Details*/

  user->contact = i_contact_profile_create ();
  if (!user->contact)
  { i_form_string_add (reqdata->form_out, "error", "Contact Details", "Failed to create user->contact"); i_user_free (user); return 1; }

  /* Hours */

  item = i_form_find_item (reqdata->form_in, "hours");
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Contact Hours", "Failed to find contact hours info in form"); i_user_free (user); return 1; }

  opt = i_form_dropdown_get_selected_value (item);
  if (opt)
  {
    if (!strcmp((char *)opt->data, "8_6x5"))
      user->contact->hours = HOURS_8_6x5;
    if (!strcmp((char *)opt->data, "24x7"))
      user->contact->hours = HOURS_24x7;
  }

  /* Email */

  opt = i_form_get_value_for_item (reqdata->form_in, "email");
  if (opt)
  { user->contact->email = strdup ((char *) opt->data); }
  
  /* Office phone */
  
  user->contact->office = i_contact_info_create ();
  if (!user->contact->office)
  { i_form_string_add (reqdata->form_out, "error", "Office Contact", "Failed to create user->contact->office"); i_user_free (user); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "office_phone");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Office Phone", "Failed to find Office contact details in form"); i_user_free (user); return 1; }
  user->contact->office->phone = strdup (opt->data);

  /* Mobile phone */
  
  user->contact->mobile = i_contact_info_create ();
  if (!user->contact->mobile)
  { i_form_string_add (reqdata->form_out, "error", "Mobile Contact", "Failed to create user->contact->mobile"); i_user_free (user); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "mobile_phone");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Mobile Phone", "Failed to find Mobile contact details in form"); i_user_free (user); return 1; }
  user->contact->mobile->phone = strdup (opt->data);

  /* Add to db */

  num = i_userdb_put (self, user);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "DB Storage", "Failed to store user record"); i_user_free (user); return 1; }

  /* Finished */

  i_user_free (user);

  i_form_string_add (reqdata->form_out, "msg", "Success", "User successfully added");

  i_form_frame_end (reqdata->form_out, "useradd");
              
  return 1;
}

