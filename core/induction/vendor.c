#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <libxml/parser.h>

#include "induction.h"
#include "list.h"
#include "path.h"
#include "construct.h"
#include "vendor.h"
#include "module.h"

/* Vendor-specific sub-system */

i_vendor *static_vendor = NULL;

/* Init */

int i_vendor_init ()
{ static_vendor = NULL; return 0; }

/* Struct Manipulation */

i_vendor* i_vendor_create ()
{
  i_vendor *vendor;

  vendor = (i_vendor *) malloc (sizeof(i_vendor));
  if (!vendor)
  { i_printf (1, "i_vendor_create failed to malloc vendor struct"); return NULL; }
  memset (vendor, 0, sizeof(i_vendor));

  return vendor;
}

void i_vendor_free (void *vendorptr)
{
  i_vendor *vendor = vendorptr;

  if (!vendor) return;

  if (vendor->name_str) free (vendor->name_str);
  if (vendor->desc_str) free (vendor->desc_str);
  if (vendor->module_str) free (vendor->module_str);
  if (vendor->detect_str) free (vendor->detect_str);
  if (vendor->module) i_module_free (vendor->module);

  free (vendor);
}

/* Set */

void i_vendor_set (i_vendor *vendor)
{ static_vendor = vendor; }

/* Get */

i_vendor* i_vendor_get ()
{ return static_vendor; }

/* List */

i_list* i_vendor_list (i_resource *self)
{
  /* Returns a list of i_vendor structs for the 
   * modules present on this system
   */

  i_list *list;

  /* Create list */
  list = i_list_create ();
  i_list_set_destructor (list, i_vendor_free);

  /* Open dir */
  char *dirname = i_path_glue (self->construct->module_path, "vendor_modules");
  DIR *dir = opendir (dirname);
  if (!dir)
  { i_printf (1, "i_vendor_list failed to open vendor_module directory %s", dirname); free(dirname); return list; }

  /* Traverse */
  struct dirent *dirent;
  while ((dirent=readdir(dir))!=NULL)
  {
    /* Handle directory entry */
    if (strstr(dirent->d_name, ".so"))
    {
      /* Module found */
      i_module *module = i_module_open (dirname, dirent->d_name);
      if (module)
      {
        char *err;
        char *name_str = NULL;
        char *desc_str = NULL;
        char *(*vendor_name) ();
        char *(*vendor_desc) ();

        /* Get name function */
        vendor_name = dlsym (module->handle, "vendor_name");
        err = dlerror();
        if (!err)
        { 
          /* Get name */
          name_str = vendor_name(); 

          /* Get desc */
          vendor_desc = dlsym (module->handle, "vendor_desc");
          err = dlerror();
          if (err)
          { i_printf (1, "i_vendor_list failed to get vendor_desc from %s", dirent->d_name); }
          else
          { desc_str = vendor_desc(); }
        }
        else
        {
          /* Use module name */
          char *ptr;
          char *temp;
          temp = strdup (dirent->d_name);
          ptr = strchr (temp, '.');
          *ptr = '\0';
          name_str = strdup (temp);
        }

        /* Check result */
        if (name_str)
        {
          /* Add vendor to list */
          i_vendor *vendor;
          vendor = i_vendor_create ();
          vendor->name_str = strdup (name_str);
          if (desc_str)
          { vendor->desc_str = strdup (desc_str); }
          else
          { vendor->desc_str = strdup (name_str); }
          vendor->module_str = strdup (dirent->d_name);
          vendor->module = module;
          i_list_enqueue (list, vendor);
        }
        else
        { i_module_free (module); }
      }
      else
      {
        /* Module failed to load...
         * FIX this is due to the uselessness of linux.
         * On OSX, RTLD_LAZY is observed.
         * In debian.. what a surprise... it's not.
         * The modules refuse to load due to unresolved symbols
         * BUT HEY, that's not what RTLD_LAZY is for is it? Turds
         */

        /* Use filename */
        char *name_str;
        char *ptr;
        char *temp;
        temp = strdup (dirent->d_name);
        ptr = strchr (temp, '.');
        *ptr = '\0';
        name_str = strdup (temp);

        /* Add vendor to list */
        i_vendor *vendor;
        vendor = i_vendor_create ();
        vendor->name_str = strdup (name_str);
        vendor->desc_str = strdup (name_str);
        vendor->module_str = strdup (dirent->d_name);
        vendor->module = NULL;
        i_list_enqueue (list, vendor);
      } 
    }
  }
  free (dirname);
  closedir (dir);

  /*
   * Find Mobule-builder modules
   */

  /* Open dir */
  dirname = i_path_glue (self->construct->config_path, "module_builder");
  dir = opendir (dirname);
  if (!dir)
  { i_printf (1, "i_vendor_list failed to open vendor_module directory %s", dirname); free(dirname); return list; }
          
  /* Traverse */
  while ((dirent=readdir(dir))!=NULL) 
  {  
    /* Check for .xml */
    if (!strstr(dirent->d_name, ".xml"))
    {
      if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, ".."))
      { i_printf (1, "i_vendor_list skipped file with incorrect extension in %s (%s)", dirname, dirent->d_name); }
      continue; 
    }

    /* Load file into memory */
    int fd = open (dirent->d_name, O_RDONLY);
    lseek (fd, 0, SEEK_SET);
    size_t len = lseek (fd, 0, SEEK_END);
    char *buf = (char *) malloc (sizeof(len)+1);
    read (fd, buf, len);
    close (fd);
    buf[len] = '\0';

    /* Create vendor */
    i_vendor *vendor = i_vendor_create ();
    vendor->module = NULL;
    vendor->module_str = strdup (dirent->d_name);
    vendor->name_str = strdup (dirent->d_name);
    vendor->desc_str = strdup (dirent->d_name);
    i_list_enqueue (list, vendor);
    
    /* Parse */
    xmlDocPtr doc = xmlReadMemory (buf, len+1, "module_builder", "UTF-8", 0);
    free (buf);
    if (!doc) continue;
    xmlNodePtr node;
    xmlNodePtr root_node = xmlDocGetRootElement (doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);

      if (strcmp(str, "description")==0)
      { 
        free (vendor->desc_str); 
        vendor->desc_str = strdup (str); 
        break;
      }

      xmlFree (str);
    }
  }
  free (dirname);
  closedir (dir);

  return list;
}
