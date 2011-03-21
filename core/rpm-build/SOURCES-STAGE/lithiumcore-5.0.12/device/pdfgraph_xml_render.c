#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/timeutil.h>
#include <induction/path.h>
#include <induction/rrd.h>
#include <induction/rrdtool.h>
#include <induction/xml.h>

#include "pdfgraph.h"
#include "xmlgraph.h"

int xml_pdfgraph_render (i_resource *self, i_xml_request *xmlreq)
{
  return xml_xmlgraph_render(self, xmlreq);
}
