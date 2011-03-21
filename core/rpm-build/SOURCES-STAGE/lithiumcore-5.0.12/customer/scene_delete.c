#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/callback.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "scene.h"

/* Scene Delete
 *
 * Deletes all document-specific data 
 */

int l_scene_delete (i_resource *self, int doc_id)
{
  /* Delete the old data */
  l_scene_sql_delete (self, doc_id);
  l_scene_sql_overlay_delete (self, doc_id);

  return 0;
}
