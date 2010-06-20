#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "list.h"

/* A threshold section is a group of thresholds
 * pertaining to a single sub-system. e.g hrstorage or iflist
 */

static int static_enabled = 0;
static i_list *static_section_list = NULL;

/* Enable / Disable */

int i_threshold_section_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "i_threshold_section_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  static_section_list = i_list_create ();
  if (!static_section_list)
  { i_printf (1, "i_threshold_section_enable failed to create static_section_list"); i_threshold_section_disable (self); return -1; }
  i_list_set_destructor (static_section_list, i_threshold_section_free);

  return 0;
}

int i_threshold_section_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_threshold_section_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_section_list)
  { i_list_free (static_section_list); static_section_list = NULL; }

  return 0;
}

/* Pointer fetching */

i_list *i_threshold_section_list ()
{ 
  if (static_enabled == 0)
  { return NULL; }

  return static_section_list;
}

/* Struct Manipulation */

i_threshold_section* i_threshold_section_create ()
{
  i_threshold_section *section;

  section = (i_threshold_section *) malloc (sizeof(i_threshold_section));
  if (!section)
  { i_printf (1, "i_threshold_section_create failed to malloc section"); return NULL; }
  memset (section, 0, sizeof(i_threshold_section));

  section->thresholds = i_list_create ();
  if (!section->thresholds)
  { i_printf (1, "i_threshold_section_create failed to create section->thresholds list"); i_threshold_section_free (section); return NULL; }

  return section;
}

void i_threshold_section_free (void *sectionptr)
{
  i_threshold_section *section = sectionptr;

  if (!section) return;

  if (section->name) free (section->name);
  if (section->desc) free (section->desc);
  if (section->thresholds) i_list_free (section->thresholds);

  free (section);
}

/* Section Register / Deregister */

i_threshold_section* i_threshold_section_register (i_resource *self, char *name, char *desc)
{
  int num;
  i_threshold_section *section;

  if (!self || !name || !desc) return NULL;

  /* Create the section struct */
  
  section = i_threshold_section_create ();
  if (!section)
  { i_printf (1, "i_threshold_section_register failed to create section struct for section %s", name); return NULL; }

  section->name = strdup (name);
  section->desc = strdup (desc);

  /* Enqueue the section */

  num = i_list_enqueue (static_section_list, section);
  if (num != 0)
  { 
    i_printf (1, "i_threshold_section_register failed to enqueue section %s", name);
    i_threshold_section_free (section);
    return NULL;
  }

  /* Finished */

  return section;
}

int i_threshold_section_deregister (char *name)
{
  int delete_count = 0;
  i_threshold_section *section;

  if (!name) return -1;

  for (i_list_move_head(static_section_list); (section=i_list_restore(static_section_list))!=NULL; i_list_move_next(static_section_list))
  {
    if (!strcmp(section->name, name))
    { 
      i_threshold *threshold;

      /* Deregister the sections thresholds */

      for (i_list_move_head(section->thresholds); (threshold=i_list_restore(section->thresholds))!=NULL; i_list_move_next(section->thresholds))
      { i_threshold_deregister (section, threshold->name); }

      /* Remove the section */
      
      i_list_delete (static_section_list); 
      delete_count++; 
    }
  }

  if (delete_count < 1) return -1;

  return 0;
}

/* Get */

i_threshold_section* i_threshold_section_get (i_resource *self, char *name)
{
  i_threshold_section *section;

  if (!self || !name) return NULL;

  for (i_list_move_head(static_section_list); (section=i_list_restore(static_section_list))!=NULL; i_list_move_next(static_section_list))
  {
    if (!strcmp(section->name, name))
    { return section; }
  }
  
  return NULL;
}
