#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/user.h>

#include "infstat.h"

/* form_infstat_issue_edit - Add/Edit issues */

int form_infstat_issue_edit (i_resource *self, i_form_reqdata *reqdata)
{
  int severity;
  int affects;
  char *time_str;
  char *date_str;
  char *form_title;
  char *adminlead_username = NULL;
  char *techlead_username = NULL;
  struct timeval now;
  struct tm now_tm;
  i_form_item *item;
  i_form_item *admin_item;
  i_form_item *tech_item;
  i_form_item *(*strent_func) ();
  i_list *user_list;
  i_user *user;
  l_infstat_cat *cat = NULL;
  l_infstat_issue *issue = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  /* Form setup */
  if (reqdata->auth->level < AUTH_LEVEL_STAFF)
  {
    /* Non-priv */
    reqdata->form_out = i_form_create (reqdata, 0);
    if (!reqdata->form_out)
    { i_printf (1, "form_infstat_issue_edit failed to create form"); return -1; }
    strent_func = i_form_string_add;
  }
  else
  {
    /* Priv */
    reqdata->form_out = i_form_create (reqdata, 1);
    if (!reqdata->form_out)
    { i_printf (1, "form_infstat_issue_edit failed to create form"); return -1; }
    strent_func = i_form_entry_add;
  }

  /* State check */
  if (l_infstat_cat_load_inprogress() == 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "New issues can not be added whilst the category list is being loaded. Please try again when the category list load has complete"); return 1; }

  /* Check for cat_name_str or id */
  if (reqdata->form_passdata && reqdata->form_passdata_size > 0)
  {
    char *name_str = (char *) reqdata->form_passdata;
    char *id_str;

    /* Interpret ':' separated values */
    id_str = strchr (name_str, ':');
    if (id_str)
    { *id_str = '\0'; id_str++; }

    /* Find specified cat */
    cat = l_infstat_cat_get (self, name_str);
    if (!cat)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }

    /* Check if an ID was specified */
    if (id_str)
    { 
      /* ID Present, editing existing */
      issue = l_infstat_issue_get (self, cat, atol(id_str));
      if (!issue)
      { i_form_string_add (reqdata->form_out, "error", "Error", "Specified issue not found"); return 1; }

      asprintf (&form_title, "Infrastructure Status - Edit Existing Issue in %s Category", cat->desc_str);
      i_form_frame_start (reqdata->form_out, "issueedit", form_title);
      free (form_title);
    }
    else
    {
      /* No ID specified, adding new */
      /* Check authority */
      if (reqdata->auth->level < AUTH_LEVEL_STAFF)
      { return i_form_deliver_denied (self, reqdata); }

      /* Form string */
      asprintf (&form_title, "Infrastructure Status - Add New Issue to %s Category", cat->desc_str);
      i_form_frame_start (reqdata->form_out, "issueedit", form_title);
      free (form_title);
    }
  }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "No passdata supplier (Issue ID or Category Name)"); return 1; }

  /* Category */
  i_form_string_add (reqdata->form_out, "cat_label", "Category", cat->desc_str);
  i_form_hidden_add (reqdata->form_out, "cat", cat->name_str);
  if (issue)
  { 
    char *id_str;
    asprintf (&id_str, "%li", issue->id); 
    i_form_hidden_add (reqdata->form_out, "id", id_str);
    free (id_str);
  }

  /* Headline / Desc */
  if (issue)
  { 
    strent_func (reqdata->form_out, "headline", "Headline", issue->headline_str);
    if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
    { i_form_textarea_add (reqdata->form_out, "desc", "Description", issue->desc_str); }
    else
    { i_form_string_add (reqdata->form_out, "desc", "Description", issue->desc_str); }
  }
  else
  { 
    strent_func (reqdata->form_out, "headline", "Headline", NULL); 
    i_form_textarea_add (reqdata->form_out, "desc", "Description", NULL);
  }

  /* Severity */
  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    if (issue) severity = issue->severity;
    else severity = -1;
    item = i_form_dropdown_create ("severity", "Severity");
    i_form_dropdown_add_option (item, "0", l_infstat_issue_severity_str(0), (severity == INFSTAT_SEV_NORMAL));
    i_form_dropdown_add_option (item, "1", l_infstat_issue_severity_str(1), (severity == INFSTAT_SEV_TRIVIAL));
    i_form_dropdown_add_option (item, "2", l_infstat_issue_severity_str(2), (severity == INFSTAT_SEV_IMPAIRED));
    i_form_dropdown_add_option (item, "3", l_infstat_issue_severity_str(3), (severity == INFSTAT_SEV_OFFLINE));
    i_form_add_item (reqdata->form_out, item);
  }
  else
  { i_form_string_add (reqdata->form_out, "severity", "Severity", l_infstat_issue_severity_str(issue->severity)); }

  /* Affects */
  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    if (issue) affects = issue->affects;
    else affects = -1;
    item = i_form_dropdown_create ("affects", "Affects");
    i_form_dropdown_add_option (item, "0", l_infstat_issue_affects_str(0), (affects == INFSTAT_AFFECTS_NONE));
    i_form_dropdown_add_option (item, "1", l_infstat_issue_affects_str(1), (affects == INFSTAT_AFFECTS_SOME));
    i_form_dropdown_add_option (item, "2", l_infstat_issue_affects_str(2), (affects == INFSTAT_AFFECTS_MANY));
    i_form_dropdown_add_option (item, "3", l_infstat_issue_affects_str(3), (affects == INFSTAT_AFFECTS_ALL));
    i_form_add_item (reqdata->form_out, item);
  }
  else
  { i_form_string_add (reqdata->form_out, "affects", "Affects", l_infstat_issue_affects_str(issue->severity)); }

  /* Restore Time */
  if (issue)
  {
    struct tm restore_tm;
    localtime_r (&issue->etr_sec, &restore_tm);
    asprintf (&time_str, "%02i:%02i", restore_tm.tm_hour, restore_tm.tm_min);
    asprintf (&date_str, "%2i/%2i/%4i", restore_tm.tm_mday, restore_tm.tm_mon+1, restore_tm.tm_year+1900);
  }
  else
  {
    gettimeofday (&now, NULL);
    localtime_r ((time_t *)&now.tv_sec, &now_tm);
    if (now_tm.tm_min < 30)
    { asprintf (&time_str, "%02i:00", (now_tm.tm_hour+4) % 24); }
    else
    { asprintf (&time_str, "%02i:00", (now_tm.tm_hour+5) % 24); }
    asprintf (&date_str, "%2i/%2i/%4i", now_tm.tm_mday, now_tm.tm_mon+1, now_tm.tm_year+1900);
  }

  strent_func (reqdata->form_out, "restore_time", "Est. Restore Time", time_str);
  free (time_str);
  i_form_string_add (reqdata->form_out, "restore_time_note", "", "(24 Hour time - E.g 23:45)");

  strent_func (reqdata->form_out, "restore_date", "Est. Restore Date", date_str);
  free (date_str);
  i_form_string_add (reqdata->form_out, "restore_date_note", "", "(DD/MM/YYYY - E.g 16/10/2003)");
  
  /* Admin/Tech Lead */
  if (issue)
  {
    i_list_move_head (issue->adminusername_list);
    adminlead_username = i_list_restore (issue->adminusername_list);
    i_list_move_head (issue->techusername_list);
    techlead_username = i_list_restore (issue->techusername_list);
  }

  if (reqdata->auth->level >= AUTH_LEVEL_STAFF)
  {
    user_list = i_user_sql_list (self);
    admin_item = i_form_dropdown_create ("adminuser", "Administrative Lead");
    tech_item = i_form_dropdown_create ("techuser", "Technical Lead");
    for (i_list_move_head(user_list); (user=i_list_restore(user_list))!=NULL; i_list_move_next(user_list))
    {
      i_form_dropdown_add_option (admin_item, user->auth->username, user->fullname, (adminlead_username && !strcmp(adminlead_username, user->auth->username)));
      i_form_dropdown_add_option (tech_item, user->auth->username, user->fullname, (techlead_username && !strcmp(techlead_username, user->auth->username)));
    }
    i_form_add_item (reqdata->form_out, admin_item);
    i_form_add_item (reqdata->form_out, tech_item);
  }
  else
  { 
    user = i_user_sql_get (self, adminlead_username);
    if (user)
    { i_form_string_add (reqdata->form_out, "adminuser", "Administrative Lead", user->fullname); i_user_free (user); }
    else
    { i_form_string_add (reqdata->form_out, "adminuser", "Administrative Lead", "None Set"); }

    user = i_user_sql_get (self, techlead_username);
    if (user)
    { i_form_string_add (reqdata->form_out, "techuser", "Technical Lead", user->fullname); i_user_free (user); }
    else
    { i_form_string_add (reqdata->form_out, "techuser", "Technical Lead", "None Set"); }
  }

  i_form_frame_end (reqdata->form_out, "issueedit");

  /* Finished */
  return 1;
}

int form_infstat_issue_edit_submit (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *hour_str;
  char *min_str;
  char *day_str;
  char *mon_str;
  char *year_str;
  struct tm restore_tm;
  i_user *user;
  i_form_item_option *opt;
  l_infstat_cat *cat;
  l_infstat_issue *issue;
  l_infstat_issue *existing_issue = NULL;

  /* Auth check */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_STAFF)
  { return i_form_deliver_denied (self, reqdata); }

  /* Create the form */
  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_infstat_issue_add_submit failed to create form"); return -1; }

  /* Find the category */
  opt = i_form_get_value_for_item (reqdata->form_in, "cat");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Category name not found in form"); return 1; }
  cat = l_infstat_cat_get (self, (char *) opt->data);
  if (!cat)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified category not found"); return 1; }

  /* Check for existing issue */
  opt = i_form_get_value_for_item (reqdata->form_in, "id");
  if (opt)
  {
    /* Existing issue specified */
    long id;

    /* Form title */
    i_form_frame_start (reqdata->form_out, "issueedit", "Infrastructure Status - Edit Existing Issue");

    /* Find issue */
    id = atol ((char *)opt->data);
    existing_issue = l_infstat_issue_get (self, cat, id); 
    if (!existing_issue)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Specified issue not found"); return 1; }
  }
  else
  {
    /* New issue form title */
    i_form_frame_start (reqdata->form_out, "issueedit", "Infrastructure Status - Edit Existing Issue");
  }
  
  /* Create the struct */
  issue = l_infstat_issue_create ();
  if (!issue)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create l_infstat_issue struct"); return 1; }

  /* Issue info */
  opt = i_form_get_value_for_item (reqdata->form_in, "headline");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue headline not found in form"); l_infstat_issue_free (issue); return 1; }
  issue->headline_str = strdup (opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "desc");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue description not found in form"); l_infstat_issue_free (issue); return 1; }
  issue->desc_str = strdup (opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "severity");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue severity not found in form"); l_infstat_issue_free (issue); return 1; }
  issue->severity = atoi ((char *)opt->data);

  opt = i_form_get_value_for_item (reqdata->form_in, "affects");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue affects not found in form"); l_infstat_issue_free (issue); return 1; }
  issue->affects = atoi ((char *)opt->data);

  /* Restore time */
  memset (&restore_tm, 0, sizeof(struct tm));

  opt = i_form_get_value_for_item (reqdata->form_in, "restore_time");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue restore time not found in form"); l_infstat_issue_free (issue); return 1; }
  hour_str = (char *)opt->data;
  min_str = strchr (hour_str, ':');
  if (!min_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue restore time not formatted correct"); l_infstat_issue_free (issue); return 1; }
  *min_str = '\0';
  min_str++;
  restore_tm.tm_hour = atoi (hour_str);
  restore_tm.tm_min = atoi (min_str);

  opt = i_form_get_value_for_item (reqdata->form_in, "restore_date");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue restore date not found in form"); l_infstat_issue_free (issue); return 1; }
  day_str = (char *)opt->data;
  mon_str = strchr (day_str, '/');
  if (!mon_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue restore date not formatted correctly near month"); l_infstat_issue_free (issue); return 1; }
  *mon_str = '\0';
  mon_str++;
  year_str = strchr (mon_str, '/');
  if (!year_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue restore date not formatted correctly near year"); l_infstat_issue_free (issue); return 1; }
  *year_str = '\0';
  year_str++;
  restore_tm.tm_mday = atoi (day_str);
  restore_tm.tm_mon = (atoi (mon_str)) - 1;
  restore_tm.tm_year = (atoi (year_str)) - 1900;

  issue->etr_sec = mktime (&restore_tm);

  /* Users */
  opt = i_form_get_value_for_item (reqdata->form_in, "adminuser");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue administrative lead user not found in form"); l_infstat_issue_free (issue); return 1; }
  user = i_user_sql_get (self, (char *)opt->data);
  if (user)
  { i_list_enqueue (issue->adminusername_list, strdup(user->auth->username)); i_user_free (user); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified administrative lead user not found in user database"); l_infstat_issue_free (issue); return 1; }

  opt = i_form_get_value_for_item (reqdata->form_in, "techuser");
  if (!opt)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Issue technical lead user not found in form"); l_infstat_issue_free (issue); return 1; }
  user = i_user_sql_get (self, (char *)opt->data);
  if (user)
  { i_list_enqueue (issue->techusername_list, strdup(user->auth->username)); i_user_free (user); }
  else
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified technical lead user not found in user database"); l_infstat_issue_free (issue); return 1; }

  /* Update/Add the issue */
  if (existing_issue)
  {
    /* Update the existing issue struct identifiers */
    if (existing_issue->headline_str) free (existing_issue->headline_str);
    existing_issue->headline_str = strdup (issue->headline_str);
    if (existing_issue->desc_str) free (existing_issue->desc_str);
    existing_issue->desc_str = strdup (issue->desc_str);

    /* Check for severity change */
    if (existing_issue->severity != issue->severity)
    {
      /* Decrement old counter */
      switch (existing_issue->severity)
      {
        case INFSTAT_SEV_NORMAL: l_infstat_issue_count_normal_dec();
                                 break;
        case INFSTAT_SEV_TRIVIAL: l_infstat_issue_count_trivial_dec();
                                 break;
        case INFSTAT_SEV_IMPAIRED: l_infstat_issue_count_impaired_dec();
                                 break;
        case INFSTAT_SEV_OFFLINE: l_infstat_issue_count_offline_dec();
                                  break;
      }
      /* Increment new counter */
      switch (issue->severity)
      {
        case INFSTAT_SEV_NORMAL: l_infstat_issue_count_normal_inc();
                                 break;
        case INFSTAT_SEV_TRIVIAL: l_infstat_issue_count_trivial_inc();
                                 break;
        case INFSTAT_SEV_IMPAIRED: l_infstat_issue_count_impaired_inc();
                                 break;
        case INFSTAT_SEV_OFFLINE: l_infstat_issue_count_offline_inc();
                                  break;
      }
    }

    /* Update issue variables */
    existing_issue->severity = issue->severity;
    existing_issue->affects = issue->affects;
    existing_issue->etr_sec = issue->etr_sec;

    if (existing_issue->adminusername_list) i_list_free (existing_issue->adminusername_list);
    existing_issue->adminusername_list = issue->adminusername_list;
    issue->adminusername_list = NULL;
    if (existing_issue->techusername_list) i_list_free (existing_issue->techusername_list);
    existing_issue->techusername_list = issue->techusername_list;
    issue->techusername_list = NULL;

    /* Update (SQL et. al.) */
    num = l_infstat_issue_update (self, cat, existing_issue);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to update issue"); return 1; }

    i_form_string_add (reqdata->form_out, "msg", "Success", "Existing issue updated successfully");

    /* Free issue, not needed now */
    l_infstat_issue_free (issue); 
  }
  else
  {
    /* Add the new issue */
    num = l_infstat_issue_add (self, cat, issue);
    if (num != 0)
    { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to add issue"); l_infstat_issue_free (cat); return 1; }

    i_form_string_add (reqdata->form_out, "msg", "Success", "New issue added successfully");
  }

  i_form_frame_end (reqdata->form_out, "issueedit");

  /* Success */

  return 1;
}
