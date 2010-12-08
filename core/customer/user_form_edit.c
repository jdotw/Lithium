#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include <induction.h>
#include <induction/hierarchy.h>
#include <induction/user.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/contact.h>
#include <induction/userdb.h>

#include "user.h"

int form_user_edit (i_resource *self, i_form_reqdata *reqdata)
{
  char *str;
  i_user *user;
  i_form_item *item;

  if (!self || !reqdata) return -1;
  
  /* Auth checking */
  
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_ADMIN)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form creation */

  reqdata->form_out = i_form_create (reqdata, 1);
  if (!reqdata->form_out) 
  { i_printf (1, "form_user_edit failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "useredit", "Edit User Profile");

  /* Get user record */

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No user specified"); return 1; }
  
  user = i_userdb_get (self, (char *) reqdata->form_passdata);
  if (!user)
  { i_form_string_add (reqdata->form_out, "error", "Error", "The specified username was not found"); return 1; }

  i_form_string_add (reqdata->form_out, "username", "Username", user->auth->username);
  i_form_hidden_add (reqdata->form_out, "username", user->auth->username);
  i_form_password_add (reqdata->form_out, "password", "Password", user->auth->password);
  item = i_form_dropdown_create ("auth_level", "Security Level");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Security Level Dropdown", "Failed to create security level dropdown item"); return 1; }

//  asprintf (&str, "%i", AUTH_LEVEL_INFSTAT);  
//  i_form_dropdown_add_option (item, str, "Status Only", (user->auth->level == AUTH_LEVEL_INFSTAT));
//  free (str);

  asprintf (&str, "%i", AUTH_LEVEL_RO);  
  i_form_dropdown_add_option (item, str, "Read-Only", (user->auth->level <= AUTH_LEVEL_RO));
  free (str);

  asprintf (&str, "%i", AUTH_LEVEL_USER);  
  i_form_dropdown_add_option (item, str, "Normal User", (user->auth->level == AUTH_LEVEL_USER));
  free (str);

  asprintf (&str, "%i", AUTH_LEVEL_ADMIN);  
  i_form_dropdown_add_option (item, str, "Administrator", (user->auth->level >= AUTH_LEVEL_ADMIN));
  free (str);

  i_form_add_item (reqdata->form_out, item);
  i_form_spacer_add (reqdata->form_out);

  i_form_entry_add (reqdata->form_out, "fullname", "Full Name", user->fullname);
  i_form_entry_add (reqdata->form_out, "title", "Title", user->title);
  i_form_spacer_add (reqdata->form_out);

  i_form_entry_add (reqdata->form_out, "email", "Email Address", user->contact->email);
  i_form_entry_add (reqdata->form_out, "office_phone", "Office Phone", user->contact->office->phone);
  i_form_entry_add (reqdata->form_out, "mobile_phone", "Mobile Phone", user->contact->mobile->phone);

  item = i_form_dropdown_create ("hours", "Contact Hours");
  if (!item)
  { i_form_string_add (reqdata->form_out, "error", "Hours Dropdown", "Failed to create hours dropdown item"); return 1; }
  i_form_dropdown_add_option (item, "8_6x5", "8am - 6pm / Monday - Friday", 0);
  i_form_dropdown_add_option (item, "24x7", "24 Hours / 7 Days", 0);
  switch (user->contact->hours)
  {
    case HOURS_8_6x5: 
      i_form_dropdown_set_selected (item, "8_6x5");
      break;
    case HOURS_24x7:
      i_form_dropdown_set_selected (item, "24x7");
      break;
  }
  i_form_add_item (reqdata->form_out, item);

  i_form_frame_end (reqdata->form_out, "useredit");

  return 1;
}

int form_user_edit_submit (i_resource *self, i_form_reqdata *reqdata)
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
  { i_form_string_add (reqdata->form_out, "error", "Error", "Fullname value not found in form"); i_user_free (user); return 1; }
  user->fullname = strdup (opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "title");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Title value not found in form"); i_user_free (user); return 1; }
  user->title = strdup (opt->data);

  /* Contact Details*/

  user->contact = i_contact_profile_create ();
  if (!user->contact)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create user->contact"); i_user_free (user); return 1; }

  /* Hours */

  item = i_form_find_item (reqdata->form_in, "hours");
  if (!item) 
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find contact hours info in form"); i_user_free (user); return 1; }

  opt = i_form_dropdown_get_selected_value (item);
  if (opt)
  {
    if (!strcmp((char *)opt->data, "8_6x5"))
      user->contact->hours = HOURS_8_6x5;
    if (!strcmp((char *)opt->data, "24x7"))
      user->contact->hours = HOURS_24x7;
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find selected value for hours"); i_user_free (user); return 1; }

  /* Email */

  opt = i_form_get_value_for_item (reqdata->form_in, "email");
  if (opt)
  { user->contact->email = strdup ((char *) opt->data); }
  
  /* Office phone */
  
  user->contact->office = i_contact_info_create ();
  if (!user->contact->office)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create user->contact->office"); i_user_free (user); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "office_phone");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find Office contact details in form"); i_user_free (user); return 1; }
  user->contact->office->phone = strdup (opt->data);

  /* Mobile phone */
  
  user->contact->mobile = i_contact_info_create ();
  if (!user->contact->mobile)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create user->contact->mobile"); i_user_free (user); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "mobile_phone");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to find Mobile contact details in form"); i_user_free (user); return 1; }
  user->contact->mobile->phone = strdup (opt->data);

  /* Remove old user record */

  num = i_userdb_del (self, user->auth->username);
  
  /* Add to db */

  num = i_userdb_put (self, user);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to store updated user record"); i_user_free (user); return 1; }
              
  /* Free the error form and return the user_edit form */

  i_form_free (reqdata->form_out);
  reqdata->form_out = NULL;
  num = form_user_list (self, reqdata);
  i_user_free (user);

  return num;
}

