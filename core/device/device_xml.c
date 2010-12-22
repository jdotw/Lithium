#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/auth.h>
#include <induction/log.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/metric.h>
#include <induction/opstate.h>
#include <induction/vendor.h>
#include <induction/navtree.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>

#include "snmp_swrun.h"
#include "procpro.h"
#include "service.h"

/*
 * Local Device XML
 */

xmlNodePtr l_procpro_xml (l_procpro *procpro);
extern i_resource *global_self;
int l_device_xml (i_entity *ent, xmlNodePtr ent_node, unsigned short flags)
{
  i_device *dev = (i_device *) ent;

  /* Add Process profiles */
  if (dev->swrun && !(flags & ENTXML_MOBILE))
  {
    l_procpro *procpro;
    i_list *procpro_list = l_procpro_list ();
    for (i_list_move_head(procpro_list); (procpro=i_list_restore(procpro_list))!=NULL; i_list_move_next(procpro_list))
    {
      xmlNodePtr proc_node = l_procpro_xml (procpro);
      if (proc_node) xmlAddChild (ent_node, proc_node);
    }
  }

  /* Add Services */
  if (!(flags & ENTXML_MOBILE))
  {
    i_list *service_list = l_service_list ();
    l_service *service;
    for (i_list_move_head(service_list); (service=i_list_restore(service_list))!=NULL; i_list_move_next(service_list))
    {
      xmlNodePtr serv_node = l_service_xml (service);
      if (serv_node) xmlAddChild (ent_node, serv_node);
    }
  }

  /* Add Unique process names */
  if (dev->swrun && l_snmp_swrun_cnt () && !(flags & ENTXML_MOBILE))
  {
    i_hashtable *proc_ht = i_hashtable_create (500);
    i_hashtable_set_destructor (proc_ht, free);
    xmlNodePtr procname_node = xmlNewNode(NULL, BAD_CAST "procnames");
    i_container *swrun_cnt = l_snmp_swrun_cnt ();
    l_snmp_swrun *swrun;
    for (i_list_move_head(swrun_cnt->item_list); (swrun=i_list_restore(swrun_cnt->item_list))!=NULL; i_list_move_next(swrun_cnt->item_list))
    {
      char *procname_str = i_metric_valstr (swrun->procname, NULL);
      if (procname_str) 
      {
        i_hashtable_key *key = i_hashtable_create_key_string (procname_str, proc_ht->size);
        if (!i_hashtable_get (proc_ht, key))
        {
          xmlNodePtr proc_node = xmlNewNode(NULL, BAD_CAST "proc");
          xmlNewChild (proc_node, NULL, BAD_CAST "name", BAD_CAST procname_str);
          i_hashtable_put (proc_ht, key, strdup(procname_str));
          xmlAddChild (procname_node, proc_node);
        }
        if (key) i_hashtable_key_free (key);
        free (procname_str);
      }
    }
    i_hashtable_free (proc_ht);
    xmlAddChild (ent_node, procname_node);
  }
  
  return 0;
}

