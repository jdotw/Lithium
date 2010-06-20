#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xmlsync.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/loop.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "snmpagent.h"
#include "mib2c/lcCustomerTable.h"
#include "mib2c/lcSiteTable.h"
#include "mib2c/lcDeviceTable.h"
#include "mib2c/lcContainerTable.h"
#include "mib2c/lcObjectTable.h"
#include "mib2c/lcMetricTable.h"
#include "mib2c/lcTriggerTable.h"

/* 
 * SNMP Agent Related Functions
 */

static int static_snmpagent_enabled = 0;

int l_snmpagent_is_enabled ()
{ return static_snmpagent_enabled; }

int l_snmpagent_enable (i_resource *self)
{
  /* Agent Setup */
  netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
  init_agent ("lithium-admin");
  init_snmp("lithium-admin");

  /* Add i_loop_fdset pre/post processor functions */
  i_loop_fdset_preprocessor_add (self, l_snmpagent_fdset_preprocessor, NULL);
  i_loop_fdset_postprocessor_add (self, l_snmpagent_fdset_postprocessor, NULL);

  /* Device Table */
  init_lcCustomerTable ();
  init_lcSiteTable ();
  init_lcDeviceTable ();
  init_lcContainerTable ();
  init_lcObjectTable ();
  init_lcMetricTable ();
  init_lcTriggerTable ();

  /* Set flag */
  static_snmpagent_enabled = 1;

  return 0;
}

int l_snmpagent_xmlsync_delegate (i_resource *self, int operation, i_entity *entity)
{
  /* DEBUG Performance Testing FIX */
  if (entity && entity->ent_type > ENT_DEVICE) return 0;

  /* Update the Entry */
  if (entity && (operation == XMLSYNC_SYNC || operation == XMLSYNC_NEW))
  {
    netsnmp_tdata_row *snmprow = entity->agent_rowptr;
    if (entity->ent_type == ENT_CUSTOMER)
    {
      i_customer *cust = (i_customer *)entity;
      if (!snmprow)
      {
        snmprow = lcCustomerTable_createEntry (lcCustomerTable(), cust->agent_rowindex);
        cust->agent_rowptr = snmprow;
      }
      struct lcCustomerTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcCustomerName, cust->name_str);
      snmpentry->lcCustomerName_len = strlen (cust->name_str);
      strcpy (snmpentry->lcCustomerDesc, cust->desc_str);
      snmpentry->lcCustomerDesc_len = strlen (cust->desc_str);
      snmpentry->lcCustomerAdminStatus = entity->adminstate;
      snmpentry->lcCustomerOperStatus = entity->opstate;
    }
    else if (entity->ent_type == ENT_SITE)
    {
      i_site *site = (i_site *)entity;
      if (!snmprow)
      {
        snmprow = lcSiteTable_createEntry (lcSiteTable(), site->cust->agent_rowindex, site->agent_rowindex);
        site->agent_rowptr = snmprow;
      }
      struct lcSiteTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcSiteName, site->name_str);
      snmpentry->lcSiteName_len = strlen (site->name_str);
      strcpy (snmpentry->lcSiteDesc, site->desc_str);
      snmpentry->lcSiteDesc_len = strlen (site->desc_str);
      snmpentry->lcSiteAdminStatus = entity->adminstate;
      snmpentry->lcSiteOperStatus = entity->opstate;
    }
    else if (entity->ent_type == ENT_DEVICE)
    {
      i_device *dev = (i_device *)entity;
      if (!snmprow)
      {
        snmprow = lcDeviceTable_createEntry (lcDeviceTable(), dev->site->cust->agent_rowindex, dev->site->agent_rowindex, dev->agent_rowindex);
        dev->agent_rowptr = snmprow;
      }
      struct lcDeviceTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcDeviceName, dev->name_str);
      snmpentry->lcDeviceName_len = strlen (dev->name_str);
      strcpy (snmpentry->lcDeviceDesc, dev->desc_str);
      snmpentry->lcDeviceDesc_len = strlen (dev->desc_str);
      snmpentry->lcDeviceAdminStatus = entity->adminstate;
      snmpentry->lcDeviceOperStatus = entity->opstate;
    }
    else if (entity->ent_type == ENT_CONTAINER)
    {
      i_container *cnt = (i_container *)entity;
      if (!snmprow)
      {
        snmprow = lcContainerTable_createEntry (lcContainerTable(), 
          cnt->dev->site->cust->agent_rowindex, cnt->dev->site->agent_rowindex, 
          cnt->dev->agent_rowindex, cnt->agent_rowindex);
        cnt->agent_rowptr = snmprow;
      }
      struct lcContainerTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcContainerName, cnt->name_str);
      snmpentry->lcContainerName_len = strlen (cnt->name_str);
      strcpy (snmpentry->lcContainerDesc, cnt->desc_str);
      snmpentry->lcContainerDesc_len = strlen (cnt->desc_str);
      snmpentry->lcContainerAdminStatus = entity->adminstate;
      snmpentry->lcContainerOperStatus = entity->opstate;
    }
    else if (entity->ent_type == ENT_OBJECT)
    {
      i_object *obj = (i_object *)entity;
      if (!snmprow)
      {
        snmprow = lcObjectTable_createEntry (lcObjectTable(), 
          obj->cnt->dev->site->cust->agent_rowindex, obj->cnt->dev->site->agent_rowindex, 
          obj->cnt->dev->agent_rowindex, obj->cnt->agent_rowindex, obj->agent_rowindex);
        obj->agent_rowptr = snmprow;
      }
      struct lcObjectTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcObjectName, obj->name_str);
      snmpentry->lcObjectName_len = strlen (obj->name_str);
      strcpy (snmpentry->lcObjectDesc, obj->desc_str);
      snmpentry->lcObjectDesc_len = strlen (obj->desc_str);
      snmpentry->lcObjectAdminStatus = entity->adminstate;
      snmpentry->lcObjectOperStatus = entity->opstate;
    }
    else if (entity->ent_type == ENT_METRIC)
    {
      i_metric *met = (i_metric *)entity;
      if (!snmprow)
      {
        snmprow = lcMetricTable_createEntry (lcMetricTable(), 
          met->obj->cnt->dev->site->cust->agent_rowindex, met->obj->cnt->dev->site->agent_rowindex, 
          met->obj->cnt->dev->agent_rowindex, met->obj->cnt->agent_rowindex, 
          met->obj->agent_rowindex, met->agent_rowindex);
        met->agent_rowptr = snmprow;
      }
      struct lcMetricTable_entry *snmpentry = snmprow->data;
      if (snmpentry->lcMetricName) free (snmpentry->lcMetricName);
      snmpentry->lcMetricName = strdup(met->name_str);
      snmpentry->lcMetricName_len = strlen (met->name_str);
      if (snmpentry->lcMetricDesc) free (snmpentry->lcMetricDesc);
      snmpentry->lcMetricDesc = strdup(met->desc_str);
      snmpentry->lcMetricDesc_len = strlen (met->desc_str);
      snmpentry->lcMetricAdminStatus = entity->adminstate;
      snmpentry->lcMetricOperStatus = entity->opstate;

      char *valstr = i_metric_valstr (met, NULL);
      if (valstr)
      {
        if (snmpentry->lcMetricCurValue) free (snmpentry->lcMetricCurValue);
        snmpentry->lcMetricCurValue = strdup(valstr);
        snmpentry->lcMetricCurValue_len = strlen (valstr);
        free (valstr);
      }
      else
      {
        if (snmpentry->lcMetricCurValue) free (snmpentry->lcMetricCurValue);
        snmpentry->lcMetricCurValue = NULL;
        snmpentry->lcMetricCurValue_len = 0;
      }

      char *valstr_raw = i_metric_valstr_raw (met, NULL);
      if (valstr_raw)
      {
        if (snmpentry->lcMetricCurValueRaw) free (snmpentry->lcMetricCurValueRaw);
        snmpentry->lcMetricCurValueRaw = strdup (valstr_raw);
        snmpentry->lcMetricCurValueRaw_len = strlen (valstr_raw);
        free (valstr_raw);
      }
      else
      {
        if (snmpentry->lcMetricCurValueRaw) free (snmpentry->lcMetricCurValueRaw);
        snmpentry->lcMetricCurValueRaw = NULL;
        snmpentry->lcMetricCurValueRaw_len = 0;
      }
    }
    else if (entity->ent_type == ENT_TRIGGER)
    {
      i_trigger *trg = (i_trigger *)entity;
      if (!snmprow)
      {
        snmprow = lcTriggerTable_createEntry (lcTriggerTable(), 
          trg->met->obj->cnt->dev->site->cust->agent_rowindex, trg->met->obj->cnt->dev->site->agent_rowindex, 
          trg->met->obj->cnt->dev->agent_rowindex, trg->met->obj->cnt->agent_rowindex, 
          trg->met->obj->agent_rowindex, trg->met->agent_rowindex, trg->agent_rowindex);
        trg->agent_rowptr = snmprow;
      }
      struct lcTriggerTable_entry *snmpentry = snmprow->data;
      strcpy (snmpentry->lcTriggerName, trg->name_str);
      snmpentry->lcTriggerName_len = strlen (trg->name_str);
      strcpy (snmpentry->lcTriggerDesc, trg->desc_str);
      snmpentry->lcTriggerDesc_len = strlen (trg->desc_str);
      snmpentry->lcTriggerAdminStatus = entity->adminstate;
      snmpentry->lcTriggerOperStatus = entity->opstate;
    }
  }
  else if (entity && operation == XMLSYNC_OBSOLETE)
  {
    /* Remove Row */

  }

  return 0;
}
