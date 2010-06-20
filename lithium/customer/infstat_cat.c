#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/colour.h>

#include "infstat.h"

static int static_load_inprogress = 0;
static i_list *static_cat_list = NULL;

/* Pointer/Variable fetching */

i_list* l_infstat_cat_list ()
{ 
  if (static_load_inprogress == 1)
  { return NULL; }
  return static_cat_list; 
}

int l_infstat_cat_load_inprogress ()
{ return static_load_inprogress; }

/* Struct manipulation */

l_infstat_cat* l_infstat_cat_create ()
{
  l_infstat_cat *cat;

  cat = (l_infstat_cat *) malloc (sizeof(l_infstat_cat));
  if (!cat)
  { i_printf (1, "l_infstat_cat_create failed to malloc cat struct"); return NULL; }
  memset (cat, 0, sizeof(l_infstat_cat));
  cat->issue_list = i_list_create ();
  if (!cat->issue_list)
  { i_printf (1, "l_infstat_cat_create failed to create issue_list"); l_infstat_cat_free (cat); return NULL; }
  i_list_set_destructor (cat->issue_list, l_infstat_issue_free);

  return cat;
}

void l_infstat_cat_free (void *catptr)
{
  l_infstat_cat *cat = catptr;

  if (!cat) return;
  if (cat->name_str) free (cat->name_str);
  if (cat->desc_str) free (cat->desc_str);
  if (cat->issue_list) i_list_free (cat->issue_list);

  free (cat);
}

/* Load */

int l_infstat_cat_load (i_resource *self)
{
  int num;

  static_load_inprogress = 1;

  /* Check/Create list */
  static_cat_list = i_list_create ();
  if (!static_cat_list)
  { i_printf (1, "l_infstat_cat_load failed to create static_cat_list when loading categories"); static_load_inprogress = 0;  return -1; }
  i_list_set_destructor (static_cat_list, l_infstat_cat_free);
  
  /* Load list */
  num = l_infstat_cat_sql_load (self, static_cat_list, l_infstat_cat_load_callback, NULL);
  if (num != 0)
  { 
    i_printf (1, "l_infstat_cat_load failed to call l_infstat_cat_sql_load"); 
    i_list_free (static_cat_list); 
    static_cat_list = NULL;
    static_load_inprogress = 0;
    return -1; 
  }

  return 0;
}

int l_infstat_cat_load_callback (i_resource *self, int result, void *passdata)
{
  i_list_sort (static_cat_list, l_infstat_cat_listsort_severity);
  static_load_inprogress = 0;
 
  return 0;
}

/* Add */

int l_infstat_cat_add (i_resource *self, l_infstat_cat *cat)
{
  int num;
  
  /* Load check */
  if (static_load_inprogress == 1) 
  { i_printf (1, "l_infstat_cat_add failed to add category %s due to a cat_list_load in progress", cat->name_str); return -1; }

  /* Enqueue and sort */
  num = i_list_enqueue (static_cat_list, cat);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_add failed to enqueue category %s into static_cat_list", cat->name_str); return -1; }
  i_list_sort (static_cat_list, l_infstat_cat_listsort_severity);

  /* Add to SQL */
  num = l_infstat_cat_sql_add (self, cat);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_add failed to request addition of category %s to SQL database", cat->name_str); return -1; }

  return 0;
}

/* Upate */

int l_infstat_cat_update (i_resource *self, l_infstat_cat *cat)
{
  /* Called when a cat has been updated */
  int num;

  /* Sort */
  i_list_sort (static_cat_list, l_infstat_cat_listsort_severity);

  /* Update SQL */
  num = l_infstat_cat_sql_update (self, cat);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_update failed to request update of category %s in SQL database", cat->name_str); return -1; }

  return 0;  
}

/* Remove */

int l_infstat_cat_remove (i_resource *self, l_infstat_cat *cat)
{
  int num;
  l_infstat_issue *issue;
  i_list *close_list;

  /* Close Issues */
  close_list = i_list_create ();
  if (!close_list)
  { i_printf (1, "l_infstat_cat_remove failed to create close_list"); return -1; }
  for (i_list_move_head(cat->issue_list); (issue=i_list_restore(cat->issue_list))!=NULL; i_list_move_next(cat->issue_list))
  { i_list_enqueue (close_list, issue); }
  for (i_list_move_head(close_list); (issue=i_list_restore(close_list))!=NULL; i_list_move_next(close_list))
  {
    issue->closenote_str = strdup ("Category removed"); 
    num = l_infstat_issue_close (self, cat, issue); 
    if (num != 0)
    { i_printf (1, "l_infstat_cat_remove failed to called l_infstat_issue_close"); }
  }
  i_list_free (close_list);
  
  /* Update SQL */
  num = l_infstat_cat_sql_delete (self, cat);
  if (num != 0)
  { i_printf (1, "l_infstat_cat_remove failed to delete category %s in SQL database", cat->name_str); }

  /* Remove from list */
  num = i_list_search (static_cat_list, cat);
  if (num == 0)
  { i_list_delete (static_cat_list); }
  else
  { i_printf (1, "l_infstat_cat_remove failed to remove category from list"); }

  /* Sort */
  i_list_sort (static_cat_list, l_infstat_cat_listsort_severity);

  return 0;
}

/* Get */

l_infstat_cat* l_infstat_cat_get (i_resource *self, char *name_str)
{
  i_list *cat_list;
  l_infstat_cat *cat;

  cat_list = l_infstat_cat_list ();
  for (i_list_move_head(cat_list); (cat=i_list_restore(cat_list))!=NULL; i_list_move_next(cat_list))
  { if (!strcmp(cat->name_str, name_str)) return cat; }

  return NULL;
}

/* List Sort */

int l_infstat_cat_listsort_severity (void *curptr, void *nextptr)
{
  int cur_severity = -1;
  int next_severity = -1;
  l_infstat_cat *cur = curptr;
  l_infstat_cat *next = nextptr;
  l_infstat_issue *issue;

  for (i_list_move_head(cur->issue_list); (issue=i_list_restore(cur->issue_list))!=NULL; i_list_move_next(cur->issue_list))
  { if (issue->severity > cur_severity) cur_severity = issue->severity; }
  for (i_list_move_head(next->issue_list); (issue=i_list_restore(next->issue_list))!=NULL; i_list_move_next(next->issue_list))
  { if (issue->severity > next_severity) next_severity = issue->severity; }

  if (next_severity > cur_severity) return 1; 
  if (next_severity == cur_severity)
  { return l_infstat_cat_listsort_desc (curptr, nextptr); }

  return 0;
}

int l_infstat_cat_listsort_name (void *curptr, void *nextptr)
{
  l_infstat_cat *cur = curptr;
  l_infstat_cat *next = nextptr;

  if (strcmp(cur->name_str, next->name_str) > 0) return 1;

  return 0;
}

int l_infstat_cat_listsort_desc (void *curptr, void *nextptr)
{ 
  l_infstat_cat *cur = curptr;
  l_infstat_cat *next = nextptr;
  
  if (strcmp(cur->desc_str, next->desc_str) > 0) return 1;
  
  return 0;
}

/* Headline Utils */

char* l_infstat_cat_headline_str (l_infstat_cat *cat)
{
  int highest = -1;
  char *sev_str = NULL;
  char *aff_str = NULL;
  char *headline_str = NULL;;
  l_infstat_issue *issue;

  if (!cat->issue_list || cat->issue_list->size < 1)
  { return strdup ("No known issues."); }

  for (i_list_move_head(cat->issue_list); (issue=i_list_restore(cat->issue_list))!=NULL; i_list_move_next(cat->issue_list))
  {
    if (issue->severity > highest) 
    { 
      highest = issue->severity; 
      sev_str = l_infstat_issue_severity_str (issue->severity); 
      aff_str = l_infstat_issue_affects_str (issue->affects); 
    }
  }

  if (sev_str && aff_str)
  { asprintf (&headline_str, "%s - %s", sev_str, aff_str); }
  else if (sev_str)
  { asprintf (&headline_str, "%s", sev_str); }
  else if (aff_str)
  { asprintf (&headline_str, "%s", aff_str); }
  else
  { headline_str = strdup(""); }

  return headline_str;
}


char* l_infstat_cat_headline_colorstr (l_infstat_cat *cat)
{
  int highest = 0;
  l_infstat_issue *issue;

  for (i_list_move_head(cat->issue_list); (issue=i_list_restore(cat->issue_list))!=NULL; i_list_move_next(cat->issue_list))
  { if (issue->severity > highest) highest = issue->severity; }

  switch (highest)
  {
    case INFSTAT_SEV_NORMAL: return COLOUR_GREEN_BG;
    case INFSTAT_SEV_TRIVIAL: return COLOUR_YELLOW_BG;
    case INFSTAT_SEV_IMPAIRED: return COLOUR_ORANGE_BG;
    case INFSTAT_SEV_OFFLINE: return COLOUR_RED_BG;
    default: return "#FFFFFF";
  }

  return NULL;
}


