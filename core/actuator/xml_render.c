#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/entity.h>
#include <induction/xml.h>

#include "xml_get.h"
#include "shutdown.h"

/* xml_render functions */

int a_xml_render (i_resource *self, i_xml *xml)
{
  int datasize;
  char *data;

  if (xml->raw_data)
  {
    printf ("%s", xml->raw_data);
  }
  else
  {
    data = i_xml_data (xml, &datasize);
    printf ("%s", data);
    free (data);
  }

  return 0;
}
