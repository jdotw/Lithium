#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/user.h>
#include <induction/contact.h>
#include <induction/userdb.h>

#include "user.h"

int form_user_list (i_resource *self, i_form_reqdata *reqdata)
{
  i_form_item *item;
  i_list *userlist;

  if (!self || !reqdata) return -1;
  
  /* Authentication checking */

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied (self, reqdata); }

  /* Form Creation */
  
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out) 
  { i_printf (1, "form_user_list failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "userlist", "User List");

  if (reqdata->auth->level >= AUTH_LEVEL_ADMIN)
  {
    item = i_form_string_add (reqdata->form_out, "add", "Add New User", NULL);
    i_form_item_add_link (item, 0, 0, 0, RES_ADDR(self), NULL, "user_add", 0, NULL, 0);
  
    i_form_spacer_add (reqdata->form_out);
  }

  userlist = i_userdb_get_all (self);
  if (!userlist)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to get user records from database"); return 1; }

  if (userlist->size > 0)
  {
    /* User list is not empty */
    int row;
    i_user *user;
    char *notconfstr = "Not configured";
    char *labels[8];

    if (reqdata->auth->level >= AUTH_LEVEL_ADMIN) item = i_form_table_create (reqdata->form_out, "site_list", NULL, 8); /* Admin and above get full table */
    else item = i_form_table_create (reqdata->form_out, "site_list", NULL, 5); /* Anything below gets cut-down version */
    if (!item) 
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create site list table"); i_list_free (userlist); return 1; }

    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width */

    labels[0] = "Username";
    labels[1] = "Full Name";
    labels[2] = "Title";
    labels[3] = "Office Phone";
    labels[4] = "Mobile";
    labels[5] = "Page";
    labels[6] = "Edit";
    labels[7] = "Remove";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(userlist); (user=i_list_restore(userlist))!=NULL; i_list_move_next(userlist))
    { 
      if (user->auth)
      {
        if (user->auth->username) labels[0] = strdup (user->auth->username);
        else labels[0] = strdup (notconfstr);
      }
      else
      {
        labels[0] = strdup (notconfstr);
      }

      if (user->fullname) labels[1] = strdup (user->fullname);
      else labels[1] = strdup (notconfstr);

      if (user->title) labels[2] = strdup (user->title);
      else labels[2] = strdup (notconfstr);

      if (user->contact)
      {
        if (user->contact->office)
        {          
          if (user->contact->office->phone) labels[3] = strdup (user->contact->office->phone);
          else labels[3] = strdup (notconfstr);
        } 
        else
        {
          labels[3] = strdup (notconfstr);
        }

        if (user->contact->mobile)
        {
          if (user->contact->mobile->phone) labels[4] = strdup (user->contact->mobile->phone);
          else labels[4] = strdup (notconfstr);
        }
        else
        {
          labels[4] = strdup (notconfstr);
        }
      }
      else
      {
        labels[3] = strdup (notconfstr);
        labels[4] = strdup (notconfstr);
      }

      row = i_form_table_add_row (item, labels);

      if (user->auth && user->auth->username && user->contact && user->contact->mobile && user->contact->mobile->phone)
      { i_form_table_add_link (item, 5, row, RES_ADDR(self), NULL, "user_page", 0, user->auth->username, strlen(user->auth->username)+1); }

      if (user->auth && user->auth->username)
      { i_form_table_add_link (item, 6, row, RES_ADDR(self), NULL, "user_edit", 0, user->auth->username, strlen(user->auth->username)+1); }

      if (user->auth && user->auth->username)
      { i_form_table_add_link (item, 7, row, RES_ADDR(self), NULL, "user_remove", 0, user->auth->username, strlen(user->auth->username)+1); }

      free (labels[0]);
      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);

    }
  }
  else
  {
    /* User list is empty */
    i_form_string_add (reqdata->form_out, "message", "Database", "User Database is empty");
  }

  i_list_free (userlist);

  i_form_frame_end (reqdata->form_out, "userlist");

  return 1;
}

