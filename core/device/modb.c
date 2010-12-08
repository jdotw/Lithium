#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/callback.h>
#include <induction/construct.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/name.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/path.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "avail.h"
#include "snmp.h"
#include "triggerset.h"
#include "record.h"
#include "modb.h"

/* modb - Module Builder Sub-System */

/* Sub-System Init */

int l_modb_init (i_resource *self)
{
  /* See if we're using a modbuilder module */
  if (!strstr(self->hierarchy->dev->vendor_str, ".xml"))
  { return 0; }
  
  /* Open the file, read into memoryy */
  char *file;
  self->hierarchy->dev->modb_xml_str = strdup (self->hierarchy->dev->vendor_str);
  asprintf (&file, "module_builder/%s", self->hierarchy->dev->vendor_str);
  char *fullpath = i_path_glue (self->construct->config_path, file);
  free (file);
  int fd = open (fullpath, O_RDONLY);
  if (fd == -1)
  { i_printf (1, "l_modb_enable failed to open file %s (%s)", fullpath, strerror(errno)); free (fullpath); return -1; }
  free (fullpath);
  lseek (fd, 0, SEEK_SET);
  size_t len = lseek (fd, 0, SEEK_END);
  lseek (fd, 0, SEEK_SET);
  char *buf = (char *) malloc (len+1);
  size_t num_read = read (fd, buf, len);
  close (fd);
  if (num_read < len)
  { i_printf (1, "l_modb_enable failed to read file (%i bytes read -- len was %i)", num_read, len); free (buf); return -1; }
  buf[num_read] = '\0';  

  /* Enable SNMP module builder */
  l_snmp_objfact_init (self, self->hierarchy->dev);

  /* Parse XML */
  xmlDocPtr doc = xmlReadMemory (buf, len, "modulebuilder", "UTF-8", 0);
  free (buf);
  if (!doc)
  { i_printf (1, "l_modb_enable failed to parse xml file"); return -1; }
  xmlNodePtr root_node = xmlDocGetRootElement (doc);
  if (!root_node)
  { i_printf (1, "l_modb_enable failed to find root node"); xmlFreeDoc (doc); return -1; }
  xmlNodePtr node;
  for (node = root_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);

    /* 
     * Options 
     */
    
    if (!strcmp((char *)node->name, "vendor") && str)
    {
      /* Set vendor module */
      if (self->hierarchy->dev->vendor_str) free (self->hierarchy->dev->vendor_str);
      self->hierarchy->dev->vendor_str = strdup (str);
    }
    
    /* 
     * Child Processing
     */

    else if (!strcmp((char *)node->name, "container"))
    {
      /* Process container */
      l_modb_parse_container (self, doc, node);
    }
      
    xmlFree (str);
  }

  return 0;  
}

