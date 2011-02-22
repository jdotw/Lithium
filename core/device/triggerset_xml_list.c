#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/triggerset_xml.h>
#include <induction/xml.h>

int xml_triggerset_list (i_resource *self, i_xml_request *xmlreq)
{
  i_object *obj;
  i_triggerset *tset;
  xmlNodePtr root_node = NULL;

  /* Find entity */
  obj = (i_object *) i_entity_local_get (self, xmlreq->entaddr);
  if (!obj || obj->ent_type != ENT_OBJECT) return -1;
  
  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_list");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  /* Set the tset list to use. If there's an object
   * tset_list present, use it. Otherwise, use the 
   * containers tset_list
   */
  i_list *tset_list;
  if (obj->tset_list)
  { tset_list = obj->tset_list; }
  else
  { tset_list = obj->cnt->tset_list; }

  /* Loop through each triggerset */
  for (i_list_move_head(tset_list); (tset=i_list_restore(tset_list))!=NULL; i_list_move_next(tset_list))
  {
    i_metric *met;
    i_trigger *trg;
    xmlNodePtr tset_node;
    
    /* Create tset node */
    tset_node = xmlNewNode(NULL, BAD_CAST "triggerset");

    /* Tset info */
    xmlNewChild (tset_node, NULL, BAD_CAST "name", BAD_CAST tset->name_str);
    xmlNewChild (tset_node, NULL, BAD_CAST "desc", BAD_CAST tset->desc_str);

    /* Metric info */
    met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
    if (!met) continue;
    if (met)
    {
      xmlNewChild (tset_node, NULL, BAD_CAST "metric_name", BAD_CAST met->name_str);
      xmlNewChild (tset_node, NULL, BAD_CAST "metric_desc", BAD_CAST met->desc_str);
    }
    else
    {
      xmlNewChild (tset_node, NULL, BAD_CAST "metric_name", BAD_CAST tset->metname_str);
    }

    /* Current application */
    if (i_list_search(tset->obj_list, obj) == 0)
    { 
      xmlNewChild (tset_node, NULL, BAD_CAST "applied_flag", BAD_CAST "1"); 
      xmlNewChild (tset_node, NULL, BAD_CAST "applied", BAD_CAST "Yes"); 
    }
    else
    { 
      xmlNewChild (tset_node, NULL, BAD_CAST "applied_flag", BAD_CAST "0"); 
      xmlNewChild (tset_node, NULL, BAD_CAST "applied", BAD_CAST "No"); 
    }

    /* Loop through triggers */
    for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
    {
      char *str;
      i_list *valrules;
      i_triggerset_valrule *valrule;
      xmlNodePtr trg_node;

      /* Create trigger node */
      trg_node = xmlNewNode(NULL, BAD_CAST "trigger");

      /* Get value rules */
      valrules = i_triggerset_valrule_sql_load_sync (self, tset, obj, trg);
      i_list_move_head (valrules);
      valrule = i_list_restore (valrules);    

      /* Trigger info */
      xmlNewChild (trg_node, NULL, BAD_CAST "name", BAD_CAST trg->name_str);
      xmlNewChild (trg_node, NULL, BAD_CAST "desc", BAD_CAST trg->desc_str);

      /* Value type */
      asprintf (&str, "%i", trg->val_type);
      xmlNewChild (trg_node, NULL, BAD_CAST "valtype_num", BAD_CAST str);
      free (str);
      xmlNewChild (trg_node, NULL, BAD_CAST "valtype", BAD_CAST i_value_typestr (trg->val_type));

      /* Effect */
      asprintf (&str, "%i", trg->effect);
      xmlNewChild (trg_node, NULL, BAD_CAST "effect_num", BAD_CAST str);
      free (str);
      xmlNewChild (trg_node, NULL, BAD_CAST "effect", BAD_CAST i_entity_opstatestr (trg->effect));

      /* Trigger type */
      if (valrule)
      {
        asprintf (&str, "%i", valrule->trg_type);
        xmlNewChild (trg_node, NULL, BAD_CAST "trgtype_num", BAD_CAST str);
        free (str);
        xmlNewChild (trg_node, NULL, BAD_CAST "trgtype", BAD_CAST i_trigger_typestr (valrule->trg_type));
      }
      else
      {
        asprintf (&str, "%i", trg->trg_type);
        xmlNewChild (trg_node, NULL, BAD_CAST "trgtype_num", BAD_CAST str);
        free (str);
        xmlNewChild (trg_node, NULL, BAD_CAST "trgtype", BAD_CAST i_trigger_typestr (trg->trg_type));
      }

      /* Units */
      if (met->unit_str)
      { xmlNewChild (trg_node, NULL, BAD_CAST "units", BAD_CAST met->unit_str);  }

      /* X-Value */
      if (valrule)
      { 
        asprintf (&str, "%s", valrule->xval_str);
        xmlNewChild (trg_node, NULL, BAD_CAST "xval", BAD_CAST str); 
        free (str);
      }
      else
      { 
        if (met->enumstr_list && met->enumstr_list->size > 0)
        { 
          xmlNewChild (trg_node, NULL, BAD_CAST "xval", BAD_CAST i_value_valstr (trg->val_type, trg->val, met->unit_str, met->enumstr_list)); 
        }
        else
        { 
          xmlNewChild (trg_node, NULL, BAD_CAST "xval", BAD_CAST i_value_valstr_raw (trg->val_type, trg->val)); 
        }
      }

      /* Y-Value */
      if (valrule)
      {
        if (valrule->yval_str)
        {
          asprintf (&str, "%s", valrule->yval_str);
          xmlNewChild (trg_node, NULL, BAD_CAST "yval", BAD_CAST str); 
          free (str);
        }
        else
        { 
          xmlNewChild (trg_node, NULL, BAD_CAST "yval", NULL); 
        }
      }
      else
      {
        if (trg->yval)
        { 
          if (met->enumstr_list && met->enumstr_list->size > 0)
          { 
            xmlNewChild (trg_node, NULL, BAD_CAST "yval", BAD_CAST i_value_valstr (trg->val_type, trg->yval, met->unit_str, met->enumstr_list)); 
          }
          else
          { 
            xmlNewChild (trg_node, NULL, BAD_CAST "yval", BAD_CAST i_value_valstr_raw (trg->val_type, trg->yval)); 
          }
        }
        else
        { 
          xmlNewChild (trg_node, NULL, BAD_CAST "yval", NULL); 
        }
      }

      /* Duration */
      if (valrule)
      {
        asprintf (&str, "%li", valrule->duration_sec);
        xmlNewChild (trg_node, NULL, BAD_CAST "duration", BAD_CAST str);
        free (str);
      }
      else
      {
        asprintf (&str, "%li", trg->duration_sec);
        xmlNewChild (trg_node, NULL, BAD_CAST "duration", BAD_CAST str);
        free (str);
      }

      /* Admin State */
      if (valrule)
      {
        xmlNewChild (trg_node, NULL, BAD_CAST "adminstate", BAD_CAST i_entity_adminstatestr (valrule->adminstate));
        asprintf (&str, "%i", valrule->adminstate);
        xmlNewChild (trg_node, NULL, BAD_CAST "adminstate_num", BAD_CAST str);
        free (str);
      }
      else
      {
        xmlNewChild (trg_node, NULL, BAD_CAST "adminstate", BAD_CAST "Enabled");
        xmlNewChild (trg_node, NULL, BAD_CAST "adminstate_num", BAD_CAST "0");
      }

      /* Load trigger ValRules -- Although this uses the _sync method, the
       * data is in fact cached in a local sqlite table so it's OK to use
       */

      i_list *val_rules = i_triggerset_valrule_sql_load_sync(self, tset, obj, trg);
      i_triggerset_valrule *val_rule = NULL;
      for (i_list_move_head(val_rules); (val_rule=i_list_restore(val_rules))!=NULL; i_list_move_next(val_rules))
      {
        xmlAddChild(trg_node, i_triggerset_valrule_xml(val_rule));
      }
      
      /* Add Trigger node to tset node */
      xmlAddChild (tset_node, trg_node);
    }

    /* Load Triggerset Apprules -- Although this uses the _sync method, the
     * data is infact cached in a local sqlite table so it's OK to use 
     */

    i_list *app_rules = i_triggerset_apprule_sql_load_sync(self, tset, obj);
    i_triggerset_apprule *app_rule = NULL;
    for (i_list_move_head(app_rules); (app_rule=i_list_restore(app_rules))!=NULL; i_list_move_next(app_rules))
    {
      xmlAddChild(tset_node, i_triggerset_apprule_xml(app_rule));
    }

    /* Add Tset node */
    xmlAddChild (root_node, tset_node);
  }
  
  return 1;
}

