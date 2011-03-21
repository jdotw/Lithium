#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/path.h>
#include <induction/files.h>
#include <induction/configfile.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>

#include "autoconf.h"

/* 
 * Console autoconf functions
 */

/* Single-customer autoconf */

int l_autoconf_generate_customer (i_resource *self, i_customer *cust)
{
  int fd;
  char *webroot;
  char *custroot;
  char *file;

  /* Create path */
  webroot = i_configfile_get (self, NODECONF_FILE, "httpd", "imageroot", 0);
  if (!webroot)
  { i_printf (1, "l_autoconf_generate_customer failed to create autoconf xml for %s because the http webroot is not specified in the config", cust->name_str); return -1; }
  custroot = i_path_glue (webroot, cust->name_str);
  free (webroot);
  file = i_path_glue (custroot, "autoconf.xml");
  free (custroot);
  
  /* Open the file */
  fd = open (file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (fd == -1)
  { i_printf (1, "l_autoconf_generate_customer failed to open/create file %s", file); free (file); return -1; }
  free (file);

  /* Write XML */
  l_autoconf_writestart (fd);
  l_autoconf_writecust (fd, cust);
  l_autoconf_writeend (fd);

  /* Close file */
  close (fd);

  return 0;
}

/* All-Customers autoconf */

int l_autoconf_generate_deployment (i_resource *self, i_list *cust_list)
{
  int fd;
  char *webroot;
  char *file;
  i_customer *cust;

  /* Create path */
  webroot = i_configfile_get (self, NODECONF_FILE, "httpd", "imageroot", 0);
  if (!webroot)
  { i_printf (1, "l_autoconf_generate_customer failed to create deployment autoconf xml because the http webroot is not specified in the config"); return -1; }
  file = i_path_glue (webroot, "autoconf.xml");
  free (webroot);
  
  /* Open the file */
  fd = open (file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
  if (fd == -1)
  { i_printf (1, "l_autoconf_generate_deployment failed to open/create file %s", file); free (file); return -1; }
  free (file);

  /* Write XML */
  l_autoconf_writestart (fd);
  for (i_list_move_head(cust_list); (cust=i_list_restore(cust_list))!=NULL; i_list_move_next(cust_list))
  {
    l_autoconf_writecust (fd, cust);
  }
  l_autoconf_writeend (fd);

  /* Close file */
  close (fd);

  return 0;

}

/* Util Functions */

void l_autoconf_writestart (int fd)
{
  char *str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<customers>\n";
  int num = write (fd, str, strlen(str));
  if (num < 1) i_printf(1, "l_autoconf_writestart failed to write");
}

void l_autoconf_writeend (int fd)
{
  char *str = "</customers>\n\n";
  int num = write (fd, str, strlen(str));
  if (num < 1) i_printf(1, "l_autoconf_writeend failed to write");
}

void l_autoconf_writecust (int fd, i_customer *cust)
{
  char *str;
  if (cust && cust->resaddr)
  {
    asprintf (&str, "<customer>\n<name>%s</name>\n<baseurl>%s</baseurl>\n<cluster>%s</cluster>\n<node>%s</node>\n</customer>\n",
      cust->name_str, cust->baseurl_str, cust->resaddr->plexus, cust->resaddr->node);
    int num = write (fd, str, strlen(str));
    if (num < 1) i_printf(1, "l_autoconf_writecust failed to write");
    free (str);
  }
}
