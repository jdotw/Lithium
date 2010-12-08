#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>

#include "infstat.h"

static long static_nextid = -1;
static unsigned long static_normal_count = 0;
static unsigned long static_trivial_count = 0;
static unsigned long static_impaired_count = 0;
static unsigned long static_offline_count = 0;

/* Variable Retrieval */

unsigned long l_infstat_issue_count_normal ()
{ return static_normal_count; }
int l_infstat_issue_count_normal_dec ()
{ static_normal_count--; return 0; }
int l_infstat_issue_count_normal_inc ()
{ static_normal_count++; return 0; }

unsigned long l_infstat_issue_count_trivial ()
{ return static_trivial_count; }
int l_infstat_issue_count_trivial_dec ()
{ static_trivial_count--; return 0; }
int l_infstat_issue_count_trivial_inc ()
{ static_trivial_count++; return 0; }

unsigned long l_infstat_issue_count_impaired ()
{ return static_impaired_count; }
int l_infstat_issue_count_impaired_dec ()
{ static_impaired_count--; return 0; }
int l_infstat_issue_count_impaired_inc ()
{ static_impaired_count++; return 0; }

unsigned long l_infstat_issue_count_offline ()
{ return static_offline_count; }
int l_infstat_issue_count_offline_dec ()
{ static_offline_count--; return 0; }
int l_infstat_issue_count_offline_inc ()
{ static_offline_count++; return 0; }

/* Struct Manipulation */

l_infstat_issue* l_infstat_issue_create ()
{
  l_infstat_issue *issue;

  issue = (l_infstat_issue *) malloc (sizeof(l_infstat_issue));
  if (!issue)
  { i_printf (1, "l_infstat_issue_create failed to malloc issue struct"); return NULL; }
  memset (issue, 0, sizeof(l_infstat_issue));
  issue->id = -1;   /* -1 == Not assigned */

  issue->adminusername_list = i_list_create ();
  if (!issue->adminusername_list)
  { i_printf (1, "l_infstat_issue_create failed to create adminusername_list"); l_infstat_issue_free (issue); return NULL; }
  i_list_set_destructor (issue->adminusername_list, free);

  issue->techusername_list = i_list_create ();
  if (!issue->techusername_list)
  { i_printf (1, "l_infstat_issue_create failed to create techusername_list"); l_infstat_issue_free (issue); return NULL; }
  i_list_set_destructor (issue->techusername_list, free);
  

  return issue;
}

void l_infstat_issue_free (void *issueptr)
{
  l_infstat_issue *issue = issueptr;

  if (!issue) return;

  if (issue->headline_str) free (issue->headline_str);
  if (issue->desc_str) free (issue->desc_str);
  if (issue->affres_list) i_list_free (issue->affres_list);
  if (issue->symptom_list) i_list_free (issue->symptom_list);
  if (issue->event_list) i_list_free (issue->event_list);
  if (issue->log_list) i_list_free (issue->log_list);
  if (issue->adminusername_list) i_list_free (issue->adminusername_list);
  if (issue->techusername_list) i_list_free (issue->techusername_list);

  free (issue);
}

/* Add */

int l_infstat_issue_add (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue)
{
  int num;

  /* Enqueue and sort */
  num = i_list_enqueue (cat->issue_list, issue);
  if (num != 0)
  { i_printf (1, "l_infstat_issue_add failed to enqueue new issue into category %s", cat->name_str); return -1; }
  issue->cat = cat;
  issue->start_sec = time (NULL);
  issue->state = INFSTAT_STATE_OPEN;
  i_list_sort (cat->issue_list, l_infstat_issue_listsort_severity);

  /* Sort category list */
  i_list_sort (l_infstat_cat_list(), l_infstat_cat_listsort_severity);

  /* Add to SQL */
  num = l_infstat_issue_sql_add (self, issue);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_add failed to request addition of new issue to SQL database for category %s", cat->name_str); return -1; }

  /* Set sequence */
  issue->id = l_infstat_issue_nextid ();
  l_infstat_issue_nextid_increment ();

  /* Update Counters */
  switch (issue->severity)
  {
    case INFSTAT_SEV_NORMAL: static_normal_count++;
                             break;
    case INFSTAT_SEV_TRIVIAL: static_trivial_count++;
                              break;
    case INFSTAT_SEV_IMPAIRED: static_impaired_count++;
                               break;
    case INFSTAT_SEV_OFFLINE: static_offline_count++;
                              break;
  }

  return 0;
}

/* Update */

int l_infstat_issue_update (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue)
{
  /* Called when an issue has been updated */
  int num;

  /* Sort issue list */
  i_list_sort (cat->issue_list, l_infstat_issue_listsort_severity);

  /* Sort category list */
  i_list_sort (l_infstat_cat_list(), l_infstat_cat_listsort_severity);

  /* Update SQL */
  num = l_infstat_issue_sql_update (self, issue);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_update failed to request update issue in SQL database for category %s", cat->name_str); return -1; }

  return 0;
}

/* Close */

int l_infstat_issue_close (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue)
{
  /* Called when an issue it to be closed */
  int num;

  /* Set closing values */
  issue->state = INFSTAT_STATE_CLOSED;
  issue->end_sec = time (NULL);

  /* Update SQL */
  num = l_infstat_issue_sql_update (self, issue);
  if (num != 0)
  { i_printf (1, "l_infstat_issue_close failed to remove issue in category %s from the SQL database", cat->name_str); }

  /* Remove issue */
  num = i_list_search (cat->issue_list, issue);
  if (num == 0)
  { i_list_delete (cat->issue_list); }
  else
  { i_printf (1, "l_infstat_issue_close failed to find issue in cat->issue_list for category %s", cat->name_str); }
  
  /* Sort issue list */
  i_list_sort (cat->issue_list, l_infstat_issue_listsort_severity);

  /* Sort category list */
  i_list_sort (l_infstat_cat_list(), l_infstat_cat_listsort_severity);

  /* Update Counters */
  switch (issue->severity)
  {
    case INFSTAT_SEV_NORMAL: static_normal_count--;
                             break;
    case INFSTAT_SEV_TRIVIAL: static_trivial_count--;                         
                              break;
    case INFSTAT_SEV_IMPAIRED: static_impaired_count--;                           
                               break;
    case INFSTAT_SEV_OFFLINE: static_offline_count--;
                              break;
  }                           

  return 0;
}

/* Get */

l_infstat_issue* l_infstat_issue_get (i_resource *self, l_infstat_cat *cat, long id)
{
  l_infstat_issue *issue;

  for (i_list_move_head (cat->issue_list); (issue=i_list_restore(cat->issue_list))!=NULL; i_list_move_next(cat->issue_list))
  { if (issue->id == id) return issue; }

  return NULL;
}

/* List sort */

int l_infstat_issue_listsort_severity (void *curptr, void *nextptr)
{
  l_infstat_issue *cur = curptr;
  l_infstat_issue *next = nextptr;

  if (next->severity > cur->severity) return 1;

  return 0;
}

/* String util functions */

char* l_infstat_issue_severity_str (int severity)
{
  switch (severity)
  {
    case INFSTAT_SEV_NORMAL: return "Normal Operation";
    case INFSTAT_SEV_TRIVIAL: return "Trivial Issue";
    case INFSTAT_SEV_IMPAIRED: return "System Impaired";
    case INFSTAT_SEV_OFFLINE: return "System Offline";
    default: return "Other";
  }

  return NULL;
}

char* l_infstat_issue_affects_str (int affects)
{
  switch (affects)
  {
    case INFSTAT_AFFECTS_NONE: return "No Users Affected";
    case INFSTAT_AFFECTS_SOME: return "Some Users Affected";
    case INFSTAT_AFFECTS_MANY: return "Many Users Affected";
    case INFSTAT_AFFECTS_ALL: return "All Users Affected";
    default: return "Other";
  }

  return NULL;
}

/* Next sequence number setting */

long l_infstat_issue_nextid ()
{ return static_nextid; }

int l_infstat_issue_nextid_set (long id)
{ static_nextid = id; return 0; }

int l_infstat_issue_nextid_increment ()
{ static_nextid++; return 0; }

