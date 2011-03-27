#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/user.h>

#include "infstat.h"

int l_infstat_cat_formsection (i_resource *self, i_form_reqdata *reqdata, l_infstat_cat *cat)
{
  /* Create the section of a form for a category */
  int row;
  char *table_name;
  char *labels[3] = { "", "", "" };
  char *admin_labels[4];
  char *headline_str;
  char *username;
  char *pass_str;
  i_user *adminlead;
  i_user *techlead;
  i_form_item *fstr;
  i_form_item *table;
  l_infstat_issue *issue;

  headline_str = l_infstat_cat_headline_str (cat);
  fstr = i_form_string_add (reqdata->form_out, cat->name_str, cat->desc_str, headline_str);
  free (headline_str);
  i_form_string_set_bgcolor (fstr, l_infstat_cat_headline_colorstr(cat), l_infstat_cat_headline_colorstr(cat));

  for (i_list_move_head(cat->issue_list); (issue=i_list_restore(cat->issue_list))!=NULL; i_list_move_next(cat->issue_list))
  {
    asprintf (&table_name, "infstat_%s_%li", cat->name_str, issue->id);
    table = i_form_table_create (reqdata->form_out, table_name, NULL, 3);
    free (table_name);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "142", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "15%", 4);

    /* Headline */
    labels[1] = "Headline:";
    labels[2] = issue->headline_str;
    row = i_form_table_add_row (table, labels);
    asprintf (&pass_str, "%s:%li", cat->name_str, issue->id);
    i_form_table_add_link (table, 2, row, RES_ADDR(self), NULL, "infstat_issue_edit", 0, pass_str, strlen(pass_str)+1);
    free (pass_str);

    /* Administration */
    if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
    {
      labels[1] = "Administration:";
      labels[2] = "Close Issue";
      row = i_form_table_add_row (table, labels);
      asprintf (&pass_str, "%s:%li", cat->name_str, issue->id);
      i_form_table_add_link (table, 2, row, RES_ADDR(self), NULL, "infstat_issue_close", 0, pass_str, strlen(pass_str)+1);
      free (pass_str);
    }

    /* Restore Time */
    labels[1] = "Restore Time:";
    if (issue->etr_sec > 0)
    { labels[2] = ctime (&issue->etr_sec); }
    else
    { labels[2] = "Unknown"; }
    row = i_form_table_add_row (table, labels);

    /* Contacts */
    labels[1] = "Contacts:";

    i_list_move_head (issue->adminusername_list);
    username = i_list_restore (issue->adminusername_list);
    if (username)
    { adminlead = i_user_sql_get (self, username); }
    else
    { adminlead = NULL; }

    i_list_move_head (issue->techusername_list);
    username = i_list_restore (issue->techusername_list);
    if (username)
    { techlead = i_user_sql_get (self, username); }
    else
    { techlead = NULL; }

    if (adminlead && techlead)
    { asprintf (&labels[2], "%s (Admin. Lead) / %s (Tech. Lead)", adminlead->fullname, techlead->fullname); }
    else if (adminlead)
    { asprintf (&labels[2], "%s (Admin. Lead)", adminlead->fullname); }
    else if (techlead)
    { asprintf (&labels[2], "%s (Tech. Lead)", techlead->fullname); }
    else
    { labels[2] = NULL; }
    if (labels[2]) 
    { row = i_form_table_add_row (table, labels); free (labels[2]); }

    if (adminlead) i_user_free (adminlead);
    if (techlead) i_user_free (techlead);

  }

  /* Edit/Remove */
  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    asprintf (&table_name, "infstat_%s_admin", cat->name_str);
    table = i_form_table_create (reqdata->form_out, table_name, NULL, 4);
    free (table_name);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "162", 4);
    i_form_item_add_option (table, ITEM_OPTION_NOHEADERS, 0, 0, 0, NULL, 0);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "15%", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 2, 0, "15%", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 3, 0, "15%", 4);
    admin_labels[0] = "";
    admin_labels[1] = "Add Issue";
    admin_labels[2] = "Edit Category";
    admin_labels[3] = "Remove Category";
    row = i_form_table_add_row (table, admin_labels);
    i_form_table_add_link (table, 1, row, RES_ADDR(self), NULL, "infstat_issue_edit", 0, cat->name_str, strlen(cat->name_str)+1);      
    i_form_table_add_link (table, 2, row, RES_ADDR(self), NULL, "infstat_cat_edit", 0, cat->name_str, strlen(cat->name_str)+1);      
    i_form_table_add_link (table, 3, row, RES_ADDR(self), NULL, "infstat_cat_remove", 0, cat->name_str, strlen(cat->name_str)+1);
  }

  return 0;
}
