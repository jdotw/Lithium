#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "path.h"
#include "cement.h"
#include "hierarchy.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "entity.h"
#include "timeutil.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Entity Filesystem Path Utilities
 */

char* i_entity_path (i_resource *self, i_entity *ent, time_t ref_sec, int flags)
{
  /* Returns the path to data files for the given 
   * given reference time and time period. If 
   * ENTPATH_ROOT is set then the root directory for
   * the entity is returned.
   */

  int i;
  int num;
  char *entroot;      /* Filesystem root directory of entity */
  struct tm ref_tm;

  /* Check if an entity was specified */
  if (!ent) 
  {
    /* No entity supplied, return a dup
     * string of the self->root
     */
    if (self->root)
    { return strdup (self->root); }
    else
    { return NULL; }
  }

  /* Obtain the entities root filesystem path 
   * 
   * If the ent->fsroot already exists, use it. Otherwise,
   * recursively call i_entity_path for the parent entity to 
   * establish the entities filesystem root
   */

  if (ent->fsroot_str)
  {
    /* Entities fsroot is already known */
    entroot = strdup (ent->fsroot_str);
  }
  else
  {
    /* Entities fsroot is not known, attempt to 
     * use this func, i_entity_path to retrieve
     * the parents root, then glue the entities
     * directory name to the parent root.
     */
    char *entdir;
    char *parentroot;

    /* Get parent's root */
    parentroot = i_entity_path (self, ent->parent, 0, ENTPATH_ROOT|(flags & ENTPATH_CREATE));
    if (!parentroot)
    { 
      i_printf (1, "i_entity_path failed to create parent filesystem root path for %s %s",
        i_entity_typestr(ent->ent_type), ent->name_str); 
      return NULL; 
    }

    /* Create entity directory string */
    switch (ent->ent_type)
    {
      case ENT_CUSTOMER:
        asprintf (&entdir, "customer-%s", ent->name_str);
        break;
      case ENT_SITE:
        asprintf (&entdir, "site-%s", ent->name_str);
        break;
      case ENT_DEVICE:
        asprintf (&entdir, "device-%s", ent->name_str);
        break;
      default:
        entdir = strdup (ent->name_str);
    }

    /* Glue parent's root and entdir */
    entroot = i_path_glue (parentroot, entdir);
    free (parentroot);
    free (entdir);
    if (!entroot)
    {
      i_printf (1, "i_entity_path failed to glue %s %s filesystem root path string",
        i_entity_typestr(ent->ent_type), ent->name_str);
      return NULL;
    }
  }

  /* Check/Create root path if applicable */
  if (flags & ENTPATH_CREATE && ent->fsroot_valid == 0)
  {
    num = i_path_mkdir (entroot, 0750);       /* drwxr-x--- */
    if (num != 0)
    { 
      i_printf (1, "i_entity_path failed to mkdir %s for %s %s", entroot, i_entity_typestr(ent->ent_type), ent->name_str);
      free (entroot);
      return NULL;
    }

    ent->fsroot_valid = 1;
  }

  /* Return early if TIMEPATH_ROOT is set */
  if ((flags & ENTPATH_PERIOD) == ENTPATH_ROOT)
  { return entroot; }

  /* Append the time-based portion of the path 
   *
   * First the year will be appended if the year 
   * bit in the flag is set. Then, if the month bit is also
   * set, the month portion will be set.
   *
   * Note that the ENTPATH_MONTH def sets both the year and
   * month bits of the flags. 
   *
   * The ref_sec reference epoch time is used to determine the actual
   * values of the year, month and day subdirectories
   */

  localtime_r (&ref_sec, &ref_tm);

  for (i=0; i < 2; i++)
  {
    /* Loop twice for year/month processing */
    char *subdir = NULL;
    unsigned short mkdir_flag = 0;                /* 0=Do not call mkdir   1=Call mkdir */

    /* Create the subdir str if appropriate */
    switch (i)
    {
      case 0:
        /* Year Handling */
        if ((flags & ENTPATH_YEAR) == ENTPATH_YEAR)
        {
          /* Append year directory */ 
          asprintf (&subdir, "%i", ref_tm.tm_year + 1900); 

          /* Check path validity */
          if (flags & ENTPATH_CREATE && ref_sec >= ent->fsroot_year_vsec)
          {
            /* Path is not valid, mkdir required */
            mkdir_flag = 1;
          }
        }
        break;
      case 1: 
        /* Month Handling */
        if ((flags & ENTPATH_MONTH) == ENTPATH_MONTH)
        { 
          /* Append month directory */
          asprintf (&subdir, "%i", ref_tm.tm_mon + 1); 
          
          /* Check path validity */
          if (flags & ENTPATH_CREATE && ref_sec >= ent->fsroot_month_vsec)
          {
            /* Path is not valid, mkdir required */
            mkdir_flag = 1;
          }
        }
        break;
    }

    if (subdir)
    {
      char *tempstr;

      /* Append subdir to path */
      tempstr = i_path_glue (entroot, subdir);
      if (!tempstr)
      {
        i_printf (1, "i_entity_path failed to glue year subdir %s to entroot %s for %s %s",
          subdir, entroot, i_entity_typestr(ent->ent_type), ent->name_str);
        free (subdir);
        free (entroot);
        return NULL;
      }
      free (subdir);
      free (entroot);
      entroot = tempstr;
    
      /* Check/Create portion of path if applicable */
      if (flags & ENTPATH_CREATE && mkdir_flag == 1)
      { 
        struct tm end_tm;

        /* Call mkdir */        
        num = i_path_mkdir (entroot, 0750);       /* drwxrwx--- */
        if (num != 0)
        { 
          i_printf (1, "i_entity_path failed to mkdir %s for %s %s", entroot, i_entity_typestr(ent->ent_type), ent->name_str);
          free (entroot);
          return NULL;
        } 

        /* Set path validity sec value */
        switch (i)
        {
          case 0: 
            /* Year Path Valid Sec Handling */            
            ent->fsroot_year_vsec = i_time_yearend (ref_sec, &end_tm);
            break;
          case 1:
            /* Month Path Valid Sec Handling */            
            ent->fsroot_month_vsec = i_time_monthend (ref_sec, &end_tm);
            break;
        }
      }
      /* End of subdir processing */
    }
    /* End of time-path handling */
  }

  return entroot;
}

char* i_entity_path_extdev (i_resource *self, i_entity_descriptor *entdesc, time_t ref_sec, int flags)
{
  /* Obtains the entity root, with appropriate time subdirs
   * for an entity descriptor that is potentially not local 
   * to this device
   *
   * Note: This is only intended to be used by device resource
   */

  /* Get base file system path */
  char *dev_fsroot = i_entity_path (self, ENTITY(self->hierarchy->dev), 0, ENTPATH_ROOT|ENTPATH_CREATE);
  char *fsroot;
  asprintf (&fsroot, "%s/../..", dev_fsroot);
  free (dev_fsroot);
  dev_fsroot = NULL;

  /* Append cnt/obj/met/trig as necessary */
  int i;
  for (i=2; i <= entdesc->type; i++)
  {
    char *name_str = NULL;
    switch (i)
    {
      case 2:
        name_str = entdesc->site_name;
        break;
      case 3:
        name_str = entdesc->dev_name;
        break;
      case 4:
        name_str = entdesc->cnt_name;
        break;
      case 5:
        name_str = entdesc->obj_name;
        break;
      case 6:
        name_str = entdesc->met_name;
        break;
      case 7:
        name_str = entdesc->trg_name;
        break;
    }
    char *temp;
    if (i == 2)
    { asprintf (&temp, "%s/site-%s", fsroot, name_str); }
    else if (i == 3)
    { asprintf (&temp, "%s/device-%s", fsroot, name_str); }
    else
    { asprintf (&temp, "%s/%s", fsroot, name_str); }
    free (fsroot);
    fsroot = temp;
  }

  /* Append time subdir */
  struct tm ref_tm;
  localtime_r (&ref_sec, &ref_tm);
  for (i=0; i < 2; i++)
  {
    /* Loop twice for year/month processing */
    char *subdir = NULL;
    
    /* Create the subdir str if appropriate */
    switch (i)
    {
      case 0:
        /* Year Handling */
        if ((flags & ENTPATH_YEAR) == ENTPATH_YEAR)
        {
          /* Append year directory */
          asprintf (&subdir, "%i", ref_tm.tm_year + 1900);
        } 
        break;
      case 1: 
        /* Month Handling */
        if ((flags & ENTPATH_MONTH) == ENTPATH_MONTH)
        {  
          /* Append month directory */
          asprintf (&subdir, "%i", ref_tm.tm_mon + 1);
        }
        break;
    }

    /* Append */
    if (subdir)
    {
      char *temp;
      asprintf (&temp, "%s/%s", fsroot, subdir);
      free (fsroot);
      free (subdir);
      fsroot = temp;
    }
  }

  return fsroot;  
}

/* @} */
