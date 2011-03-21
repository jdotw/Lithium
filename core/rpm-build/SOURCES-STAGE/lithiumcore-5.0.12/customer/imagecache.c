#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <sys/stat.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/path.h>

#include "imagecache.h"
#include "config.h"

/*
 * Client Daemon Image Cache
 */

/* Periodic cache clean-up */

int l_imagecache_timercb (i_resource *self, i_timer *timer, void *passdata);
int l_imagecache_purge_specific (i_resource *self, time_t max_age);

int l_imagecache_enable (i_resource *self)
{
  /* Remove all images in cahce, and then add
   * a timer to remove images older than an hour
   * every 60 minutes 
   */

  l_imagecache_purge_specific (self, 0);
  i_timer_add (self, 60 * 60, 0, l_imagecache_timercb, NULL);

  return 0;
}

int l_imagecache_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Remove all images over an hour old */
  l_imagecache_purge_specific (self, 60 * 60);
  return 1;
}

int l_imagecache_purge_specific (i_resource *self, time_t max_age)
{
  /* Remove images older than specific time */

  /* Open dir */
  char *dirname;
#ifdef OS_DARWIN
  asprintf (&dirname, "/Library/Application Support/Lithium/ClientService/Resources/htdocs/%s/image_cache", self->hierarchy->cust->name_str);
#else
  asprintf (&dirname, "/var/www/%s/image_cache", self->hierarchy->cust->name_str);
#endif
  DIR *dir = opendir (dirname);
  if (!dir)
  { i_printf (1, "l_imagecache_purge failed to open vendor_module directory %s", dirname); free(dirname); return -1; }

  /* Traverse */
  struct dirent *dirent;
  struct timeval now;
  gettimeofday (&now, NULL);
  while ((dirent=readdir(dir))!=NULL)
  {
    /* Skip . and .. */
    if (strcmp(dirent->d_name, ".")==0 || strcmp(dirent->d_name, "..")==0) continue;

    /* Stat file */
    struct stat stat_buf;
    char *fullpath_str = i_path_glue (dirname, dirent->d_name);
    if (stat(fullpath_str, &stat_buf) == 0)
    {
      /* Check age */
#ifdef OS_DARWIN
      if ((now.tv_sec - stat_buf.st_mtimespec.tv_sec) > max_age)
#else
      if ((now.tv_sec - stat_buf.st_mtime) > max_age)
#endif
      {
        /* Delete */
        unlink (fullpath_str);
      }
    }
    else
    {
      i_printf (1, "l_imagecache_purge_specific failed to stat file %s", fullpath_str);
    }

    free (fullpath_str);
  }
  free (dirname);
  closedir (dir);

  return 0;
}
