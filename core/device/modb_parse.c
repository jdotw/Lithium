#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/callback.h>
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
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/name.h>

#include "avail.h"
#include "snmp.h"
#include "triggerset.h"
#include "record.h"
#include "objform.h"
#include "cntform.h"
#include "modb.h"

/* modb - Module Builder Sub-System */

/* Container parsing */

int l_modb_parse_container (i_resource *self, xmlDocPtr doc, xmlNodePtr container_node)
{
  int mode = 0;
  char *oid_str = NULL;
  i_container *cnt = NULL;
  
  /* Parse XML */
  xmlNodePtr node;
  for (node = container_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);

    /* 
     * Get basics 
     */

    if (!strcmp((char *)node->name, "desc") && str)
    { 
      /* Set name and desc */
      char *desc_str = strdup (str);
      char *name_str = strdup (str);
      i_name_parse (name_str);
      
      /* 
       * Create and register the container
       */
      cnt = i_container_create (name_str, desc_str);
      free (name_str);
      free (desc_str);
      i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
      i_entity_refresh_config defrefconfig;
      memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
      defrefconfig.refresh_method = REFMETHOD_PARENT;
      defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
      defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
      i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
      cnt->item_list = i_list_create ();
      cnt->item_list_state = ITEMLIST_STATE_POPULATE;
      cnt->mainform_func = l_cntform_generic;
      cnt->sumform_func = l_cntform_generic;
      cnt->modb = 1;
    }

    /* 
     * Parse Options 
     */

    else if (!strcmp((char *)node->name, "mode") && str) 
    { mode = atoi (str); }

    else if (!strcmp((char *)node->name, "oid") && str)
    { oid_str = strdup (str); }
    
    else if (!strcmp((char *)node->name, "cview1") && str)
    { 
      cnt->cview1_str = strdup (str); 
      i_name_parse (cnt->cview1_str);
    }
    
    else if (!strcmp((char *)node->name, "cview2") && str)
    { 
      cnt->cview2_str = strdup (str); 
      i_name_parse (cnt->cview2_str);
    }
    
    else if (!strcmp((char *)node->name, "cview3") && str)
    { 
      cnt->cview3_str = strdup (str); 
      i_name_parse (cnt->cview3_str);
    }
    
    else if (!strcmp((char *)node->name, "cview4") && str)
    { 
      cnt->cview4_str = strdup (str); 
      i_name_parse (cnt->cview4_str);
    }

    else if (!strcmp((char *)node->name, "wview") && str)
    { 
      char *wview_str = strdup (str);
      i_name_parse (wview_str);
      i_list_enqueue (cnt->wview_metrics, wview_str); 
    }
    
    else if (!strcmp((char *)node->name, "wview_mode") && str)
    { cnt->wview_mode = atoi (str); }

    /* 
     * Metric Processing
     */

    else if (!strcmp((char *)node->name, "metric"))
    {
      /* Process metric */
      l_modb_parse_metric (self, doc, node, cnt);
    }
    
    else if (!strcmp((char *)node->name, "triggerset"))
    {
      /* Process triggerset  */
      l_modb_parse_triggerset (self, doc, node, cnt);
    }
      
    xmlFree (str);
  }

  /* Check ccontainer mode */
  if (mode == 1 && oid_str)
  {
    /* Dynamic container, create the Object Factory */
    if (oid_str)
    {
      l_snmp_objfact *objfact;
      objfact = l_snmp_objfact_create (self, cnt->name_str, cnt->desc_str);
      objfact->dev = self->hierarchy->dev;
      objfact->cnt = cnt;
      objfact->name_oid_str = strdup (oid_str);
      objfact->refresh_int_sec = self->hierarchy->dev->refresh_interval;
      objfact->fabfunc = l_modb_objfact_fab;
      objfact->ctrlfunc = l_modb_objfact_ctrl;
      objfact->cleanfunc = l_modb_objfact_clean;
      cnt->objfactptr = objfact;

      /* Start the object factory */
      int num = l_snmp_objfact_start (self, objfact);
      if (num != 0)
      {
        i_printf (1, "l_modb_parse_container failed to call l_snmp_objfact_start to start the object factory");
        return -1;
      }
    }
    else
    { i_printf (1, "l_modb_parse_container no SNMP OID specified for %s, polling will not occur", cnt->desc_str); }
  }
  else
  {
    /* 
     * Static container, create obj and metrics 
     */

    /* Configure container */
    cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

    /* Create master object */
    i_object *obj = i_object_create ("master", "Master");
    obj->cnt = cnt;
    i_entity_register (self, ENTITY(cnt), ENTITY(obj));
    i_entity_refresh_config refconfig;
    memset (&refconfig, 0, sizeof(i_entity_refresh_config));
    refconfig.refresh_method = REFMETHOD_PARENT;
    refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
    i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);
    obj->mainform_func = l_objform_generic;

    /* Create metrics from template */
    l_modb_metric *met_template;
    for (i_list_move_head(cnt->met_templates); (met_template=i_list_restore(cnt->met_templates))!=NULL; i_list_move_next(cnt->met_templates))
    {
      i_metric *met;
      met = l_modb_objfact_fab_met (self, cnt, obj, met_template, NULL);
    }

    /* Evaluate apprules for all triggersets */
    i_triggerset_evalapprules_allsets (self, obj);

    /* Evaluate recrules for all metrics */
    l_record_eval_recrules_obj (self, obj);
  }

  if (oid_str) free (oid_str);

  return 0;  
}

/* Triggerset Parsing */

int l_modb_parse_triggerset (i_resource *self, xmlDocPtr doc, xmlNodePtr container_node, i_container *cnt)
{
  /* Parse XML */
  i_triggerset *tset = NULL;
  xmlNodePtr node;
  for (node = container_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "met_desc") && str) 
    { 
      char *met_name = strdup (str);
      char *met_desc = strdup (str);
      i_name_parse (met_name);
      tset = i_triggerset_create (met_name, met_desc, met_name);
    }
    else if (!strcmp((char *)node->name, "trigger"))
    {
      /* Parse trigger XML */
      int condition = 0;
      int severity = 0;
      char *xval_str = NULL;
      char *yval_str = NULL;
      char *trg_desc = NULL;
      int val_type = 0;
      time_t duration = 0;
      xmlNodePtr trg_node;
      for (trg_node = node->children; trg_node; trg_node = trg_node->next)
      {
        if (str) xmlFree (str);
        str = (char *) xmlNodeListGetString (doc, trg_node->xmlChildrenNode, 1);
        
        if (!strcmp((char *)trg_node->name, "xval") && str) xval_str = strdup (str);
        else if (!strcmp((char *)trg_node->name, "yval") && str) yval_str = strdup (str);
        else if (!strcmp((char *)trg_node->name, "condition") && str) condition = atoi (str);
        else if (!strcmp((char *)trg_node->name, "severity") && str) severity = atoi (str);
        else if (!strcmp((char *)trg_node->name, "val_type") && str) val_type = atoi (str);
        else if (!strcmp((char *)trg_node->name, "duration") && str) duration = atol (str);
        else if (!strcmp((char *)trg_node->name, "trg_desc") && str) trg_desc = strdup (str);

        xmlFree (str);
        str = NULL;
      }

      /* Create the trigger */

      char *trg_name = NULL;
      if (trg_desc)
      {
        trg_name = strdup (trg_desc);
        i_name_parse (trg_name);
      }
      else
      {
        switch (severity)
        {
          case 1:
            trg_name = strdup ("warning");
            trg_desc = strdup ("Warning");
            break;
          case 2:
            trg_name = strdup ("impaired");
            trg_desc = strdup ("Impaired");
            break;
          case 3:
            trg_name = strdup ("critical");
            trg_desc = strdup ("Critical");
            break;
        }
      }

      float xval_flt = 0.0f;
      if (xval_str) xval_flt = atof (xval_str);
      float yval_flt = 0.0f; 
      if (yval_str) yval_flt = atof (yval_str);
      i_triggerset_addtrg (self, tset, trg_name, trg_desc, val_type, condition, 
        xval_flt, xval_str, yval_flt, yval_str, 0, severity, TSET_FLAG_VALAPPLY);
      free (xval_str);
      free (yval_str);
      free (trg_name);
      free (trg_desc);
    }

    xmlFree (str);
  }

  if (tset)
  { i_triggerset_assign (self, cnt, tset); }

  return 0;
}

/* Metric Parsing */

int l_modb_parse_metric (i_resource *self, xmlDocPtr doc, xmlNodePtr metric_node, i_container *cnt)
{
  /* Create metric template */
  l_modb_metric *met = l_modb_metric_create ();
  i_list_enqueue (cnt->met_templates, met);
  
  /* Parse XML */
  xmlNodePtr node;
  for (node = metric_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "desc") && str) 
    {
      met->desc_str = strdup (str);
      met->name_str = strdup (str);
      i_name_parse (met->name_str);
    }
    else if (!strcmp((char *)node->name, "oid") && str) met->oid_str = strdup (str);
    else if (!strcmp((char *)node->name, "met_type") && str) met->type = atoi (str);
    else if (!strcmp((char *)node->name, "val_type") && str) met->val_type = atoi (str);
    else if (!strcmp((char *)node->name, "record_method") && str) met->record_method = atoi (str);
    else if (!strcmp((char *)node->name, "summary_flag") && str) met->summary_flag = atoi (str);
    else if (!strcmp((char *)node->name, "xmet_desc") && str) met->xmet_desc = strdup (str);
    else if (!strcmp((char *)node->name, "ymet_desc") && str) met->ymet_desc = strdup (str);
    else if (!strcmp((char *)node->name, "units") && str) 
    {
      met->unit_str = strdup (str);
      if (strstr(met->unit_str, "byte") || strstr(met->unit_str, "Byte")) met->kbase = 1024;
    }
    else if (!strcmp((char *)node->name, "countermet_desc") && str) met->countermet_desc = strdup (str);
    else if (!strcmp((char *)node->name, "gaugemet_desc") && str) met->gaugemet_desc = strdup (str);
    else if (!strcmp((char *)node->name, "maxmet_desc") && str) met->maxmet_desc = strdup (str);
    else if (!strcmp((char *)node->name, "multiply_by") && str) met->multiply_by = atof (str);
    else if (!strcmp((char *)node->name, "enumerator")) l_modb_parse_metric_enum (self, doc, node, met);

    xmlFree (str);
  }

  return 0;
}

int l_modb_parse_metric_enum (i_resource *self, xmlDocPtr doc, xmlNodePtr enum_node, l_modb_metric *met)
{
  i_metric_enumstr *met_enum = i_metric_enumstr_create();
  xmlNodePtr node;
  for (node = enum_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "value") && str) met_enum->val_int = atoi(str);
    else if (!strcmp((char *)node->name, "label") && str) met_enum->str = strdup(str);
    xmlFree (str);
  }
  i_list_enqueue (met->enum_list, met_enum);

  return 0;
}
