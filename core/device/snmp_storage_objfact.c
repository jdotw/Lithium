#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/adminstate.h>
#include <induction/triggerset.h>
#include <induction/str.h>

#include "snmp.h"
#include "osx.h"
#include "snmp_storage.h"
#include "snmp_hrfilesys.h"
#include "snmp_nsram.h"
#include "record.h"

/* 
 * SNMP Storage Resources - Object Factory Functions 
 */

int static_ram_cnt_invalidated = 0;

/* Object Factory Fabrication */

int l_snmp_storage_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_storage_item *store;
  i_entity_refresh_config refconfig;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = l_snmp_storage_objform;
  obj->histform_func = l_snmp_storage_objform_hist;
  obj->tset_list = i_list_create ();
  obj->tset_ht = i_hashtable_create (30);
  i_entity_refreshcb_add (ENTITY(obj), l_snmp_storage_obj_refcb, NULL);

  /* Check name */
  if ( 
    strstr(obj->desc_str, "/sys") || 
    strstr(obj->desc_str, "/proc/") ||
    strstr(obj->desc_str, "/private/tmp") ||
    strcmp(obj->desc_str, "/dev") == 0 || 
    strcmp(obj->desc_str, "/net") == 0 || 
    strcmp(obj->desc_str, "/home") == 0 || 
    strcmp(obj->desc_str, "/.vol") == 0 || 
    strstr(obj->desc_str, "/Network/") || 
    strstr(obj->desc_str, "/automount/") ||
    strstr(obj->desc_str, "Swap Space") ||
    strstr(obj->desc_str, "Memory") || 
    strstr(obj->desc_str, "emory") || 
    strstr(obj->desc_str, "A:\\") || 
    strcmp(obj->desc_str, "Swap space") == 0 || 
    strcmp(obj->desc_str, "Physical memory") == 0
    )
  { obj->hidden = 1; }

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_storage_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  store = l_snmp_storage_item_create ();
  if (!store)
  { i_printf (1, "l_snmp_storage_objfact_fab failed to create storage item for object %s", obj->name_str); return -1; }
  obj->itemptr = store;
  store->obj = obj;
  store->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  if (!obj->hidden)
  {
    if (l_snmp_xsnmp_enabled())
    {
      /* Use Xsnmp OIDs */
      
      i_triggerset *tset = i_triggerset_create ("smart_status", "SMART Status", "smart_status");
      i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
      i_triggerset_assign_obj (self, obj, tset);

      store->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
      store->size->alloc_unit = 1024 * 1024;
      store->size->valstr_func = i_string_volume_metric;
      store->size->unit_str = strdup ("byte");
      store->size->kbase = 1024;
      store->size->summary_flag = 1;

      store->used = l_snmp_metric_create (self, obj, "used", "Used", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.5", index_oidstr, RECMETHOD_RRD, 0);
      store->used->record_defaultflag = 1;
      store->used->alloc_unit = 1024 * 1024;
      store->used->valstr_func = i_string_volume_metric;
      store->used->unit_str = strdup ("byte");
      store->used->kbase = 1024;

      store->free = l_snmp_metric_create (self, obj, "free", "Free", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
      store->free->alloc_unit = 1024 * 1024;
      store->free->valstr_func = i_string_volume_metric;
      store->free->unit_str = strdup ("byte");
      store->free->kbase = 1024;

      store->used_pc = l_snmp_metric_create (self, obj, "used_pc", "Used Percent", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.4.1.1.7", index_oidstr, RECMETHOD_RRD, 0);
      store->used_pc->record_defaultflag = 1;
      store->used_pc->unit_str = strdup ("%");

      store->writeable = l_snmp_metric_create (self, obj, "writeable", "Writeable", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.4.1.1.8", index_oidstr, RECMETHOD_NONE, 0);
      i_metric_enumstr_add (store->writeable, 0, "No");
      i_metric_enumstr_add (store->writeable, 1, "Yes");
      store->removable = l_snmp_metric_create (self, obj, "removable", "Removable", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.4.1.1.9", index_oidstr, RECMETHOD_NONE, 0);
      i_metric_enumstr_add (store->removable, 0, "No");
      i_metric_enumstr_add (store->removable, 1, "Yes");
      store->bootable = l_snmp_metric_create (self, obj, "bootable", "Bootable", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.4.1.1.10", index_oidstr, RECMETHOD_NONE, 0);
      i_metric_enumstr_add (store->bootable, 0, "No");
      i_metric_enumstr_add (store->bootable, 1, "Yes");

      store->smart_status = l_snmp_metric_create (self, obj, "smart_status", "SMART Status", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.4.1.1.11", index_oidstr, RECMETHOD_NONE, 0);
      i_metric_enumstr_add (store->smart_status, -1, "Unknown");
      i_metric_enumstr_add (store->smart_status, 0, "Not Supported");
      i_metric_enumstr_add (store->smart_status, 1, "Verified");
      i_metric_enumstr_add (store->smart_status, 2, "Warning");
      store->smart_message = l_snmp_metric_create (self, obj, "smart_message", "SMART Message", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.4.1.1.12", index_oidstr, RECMETHOD_NONE, 0);
    }
    else
    {
      /* Use standard hrStorage OIDs */

      /* Type */
      store->type = i_metric_create ("type", "Type", METRIC_INTEGER);
      i_entity_register (self, ENTITY(obj), ENTITY(store->type));
      memset (&refconfig, 0, sizeof(i_entity_refresh_config));
      refconfig.refresh_method = REFMETHOD_EXTERNAL;
      refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
      refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
      num = i_entity_refresh_config_apply (self, ENTITY(store->type), &refconfig);
      i_metric_enumstr_add (store->type, 1, "Unknown");
      i_metric_enumstr_add (store->type, 2, "RAM");
      i_metric_enumstr_add (store->type, 3, "Virtual");
      i_metric_enumstr_add (store->type, 4, "Fixed");
      i_metric_enumstr_add (store->type, 5, "Removable");
      i_metric_enumstr_add (store->type, 6, "Floppy");
      i_metric_enumstr_add (store->type, 7, "Compact Disc");
      i_metric_enumstr_add (store->type, 8, "RAM Disk");
      i_metric_enumstr_add (store->type, 9, "Flash");
      i_metric_enumstr_add (store->type, 10, "Network");

      store->typeoid = l_snmp_metric_create (self, obj, "typeoid", "Type OID", METRIC_OID, ".1.3.6.1.2.1.25.2.3.1.2", index_oidstr, RECMETHOD_NONE, 0);

      store->alloc_units = l_snmp_metric_create (self, obj, "alloc_units", "Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
      store->alloc_units->prio--;

      store->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_NONE, 0);
      store->size->alloc_unit_met = store->alloc_units;
      store->size->valstr_func = i_string_volume_metric;
      store->size->unit_str = strdup ("byte");
      store->size->kbase = 1024;

      store->used = l_snmp_metric_create (self, obj, "used", "Used", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.6", index_oidstr, RECMETHOD_NONE, 0);
      store->used->alloc_unit_met = store->alloc_units;
      store->used->valstr_func = i_string_volume_metric;
      store->used->unit_str = strdup ("byte");
      store->used->kbase = 1024;

      store->used_pc = i_metric_acpcent_create (self, obj, "used_pc", "Used Percent", RECMETHOD_RRD, store->used, store->size, ACPCENT_REFCB_GAUGE);
      store->used_pc->record_defaultflag = 1;

      store->free = i_metric_acdiff_create (self, obj, "free", "Free", METRIC_FLOAT, RECMETHOD_RRD, store->size, store->used, ACDIFF_REFCB_YMET);
      store->free->valstr_func = i_string_volume_metric;
      store->free->unit_str = strdup ("byte");
      store->free->kbase = 1024;
      store->free->record_defaultflag = 1;

      store->free_pc = i_metric_acpcent_create (self, obj, "free_pc", "Free Percent", RECMETHOD_RRD, store->free, store->size, ACPCENT_REFCB_GAUGE);
    }
  }
  
  /*
   * End Metric Creation
   */

  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, store);
  if (num != 0)
  { i_printf (1, "l_snmp_storage_objfact_fab failed to enqueue storage item for object %s", obj->name_str); l_snmp_storage_item_free (store); return -1; }

  /* Check for Xsnmp 
   *
   * If Xsnmp is not used, memory and swap information is grabbed from
   * hrStorageResource
   */

  if (!l_snmp_xsnmp_enabled())
  {
    /* Get RAM objects */
    if (strstr(obj->desc_str, "emory") || strstr(obj->desc_str, "Swap"))
    {
      if (!l_snmp_nsram_cnt()) l_snmp_nsram_enable (self);
      i_container *ram_cnt = l_snmp_nsram_cnt();
      i_object *real_obj = (i_object *) i_entity_child_get(ENTITY(ram_cnt), "master");
      i_object *swap_obj = (i_object *) i_entity_child_get(ENTITY(ram_cnt), "swap");
      l_snmp_nsram_item *ram = real_obj->itemptr;

      /* Check for memory resource */
      if (strcmp(obj->desc_str, "Physical memory") == 0 || strcmp(obj->desc_str, "Real Memory") == 0 || strcmp(obj->desc_str, "Physical Memory") == 0)
      {
        /* Log */
        i_printf(0, "l_snmp_storage_objfact_fab encountered '%s' object, using it for Physical Memory data", obj->desc_str);

        /* Clean */
        if (ram->real_alloc) { i_entity_deregister (self, ENTITY(ram->real_alloc)); i_entity_free (ENTITY(ram->real_alloc)); ram->real_alloc = NULL; }
        if (ram->real_total) { i_entity_deregister (self, ENTITY(ram->real_total)); i_entity_free (ENTITY(ram->real_total)); ram->real_total = NULL;}
        if (ram->real_used) { i_entity_deregister (self, ENTITY(ram->real_used)); i_entity_free (ENTITY(ram->real_used)); ram->real_used = NULL; }
        if (ram->real_free) { i_entity_deregister (self, ENTITY(ram->real_free)); i_entity_free (ENTITY(ram->real_free)); ram->real_free = NULL; }
    
        /* Physical Alloc units */
        ram->real_alloc = l_snmp_metric_create (self, real_obj, "real_alloc", "Physical Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
        ram->real_alloc->hidden = 1;

        /* Physical Total */
        ram->real_total = l_snmp_metric_create (self, real_obj, "real_total", "Total", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_NONE, 0);
        ram->real_total->alloc_unit_met = ram->real_alloc;
        ram->real_total->valstr_func = i_string_volume_metric;
        ram->real_total->unit_str = strdup ("byte");
        ram->real_total->kbase = 1024;
        ram->real_total->summary_flag = 1;

        /* Physical Used */
        ram->real_used = l_snmp_metric_create (self, real_obj, "real_used", "Used", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.6", index_oidstr, RECMETHOD_RRD, 0);
        ram->real_used->alloc_unit_met = ram->real_alloc;
        ram->real_used->valstr_func = i_string_volume_metric;
        ram->real_used->unit_str = strdup ("byte");
        ram->real_used->kbase = 1024;
        ram->real_used->record_defaultflag = 1;
    
        /* Physical Free (Total-Used) */
        ram->real_free = i_metric_acdiff_create (self, real_obj, "real_free", "Free", METRIC_FLOAT, RECMETHOD_RRD, ram->real_total, ram->real_used, ACDIFF_REFCB_YMET);
        ram->real_free->valstr_func = i_string_volume_metric;
        ram->real_free->unit_str = strdup ("byte");
        ram->real_free->kbase = 1024;
        ram->real_free->summary_flag = 1;

        /* Mark the ram container as needing a rebuild */
        static_ram_cnt_invalidated = 1;
      }
      else if (strcmp(obj->desc_str, "Swap space") == 0 || strcmp(obj->desc_str, "Swap Space") == 0)
      {
        /* Log */
        i_printf(0, "l_snmp_storage_objfact_fab encountered '%s' object, using it for Swap data", obj->desc_str);

        /* Clean */
        if (ram->swap_alloc) { i_entity_deregister (self, ENTITY(ram->swap_alloc)); i_entity_free (ENTITY(ram->swap_alloc)); ram->swap_alloc = NULL; }
        if (ram->swap_total) { i_entity_deregister (self, ENTITY(ram->swap_total)); i_entity_free (ENTITY(ram->swap_total)); ram->swap_total = NULL; }
        if (ram->swap_used) { i_entity_deregister (self, ENTITY(ram->swap_used)); i_entity_free (ENTITY(ram->swap_used)); ram->swap_used = NULL; }
        if (ram->swap_avail) { i_entity_deregister (self, ENTITY(ram->swap_avail)); i_entity_free (ENTITY(ram->swap_avail)); ram->swap_avail = NULL; }
        if (ram->swap_usedpc) { i_entity_deregister (self, ENTITY(ram->swap_usedpc)); i_entity_free (ENTITY(ram->swap_usedpc)); ram->swap_usedpc = NULL; }

        /* Swap Alloc units */
        ram->swap_alloc = l_snmp_metric_create (self, swap_obj, "swap_alloc", "Swap Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
        ram->swap_alloc->hidden = 1;

        /* Swap Total */
        ram->swap_total = l_snmp_metric_create (self, swap_obj, "swap_total", "Total", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_NONE, 0);
        ram->swap_total->alloc_unit_met = ram->swap_alloc;
        ram->swap_total->valstr_func = i_string_volume_metric;
        ram->swap_total->unit_str = strdup ("byte");
        ram->swap_total->kbase = 1024;
        ram->swap_total->summary_flag = 1;

        /* Swap Used */
        ram->swap_used = l_snmp_metric_create (self, swap_obj, "swap_used", "Used", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.6", index_oidstr, RECMETHOD_RRD, 0);
        ram->swap_used->alloc_unit_met = ram->swap_alloc;
        ram->swap_used->valstr_func = i_string_volume_metric;
        ram->swap_used->unit_str = strdup ("byte");
        ram->swap_used->kbase = 1024;
        ram->swap_used->record_defaultflag = 1;
    
        /* Swap Available */
        ram->swap_avail = i_metric_acdiff_create (self, swap_obj, "swap_avail", "Available", METRIC_FLOAT, RECMETHOD_RRD, ram->swap_total, ram->swap_used, ACDIFF_REFCB_YMET);
        ram->swap_avail->valstr_func = i_string_volume_metric;
        ram->swap_avail->unit_str = strdup ("byte");
        ram->swap_avail->kbase = 1024;
        ram->swap_avail->summary_flag = 1;
   
        /* Used percent */
        ram->swap_usedpc = i_metric_acpcent_create (self, swap_obj, "swap_usedpc", "Swap Used Percent", RECMETHOD_RRD, ram->swap_used, ram->swap_total, ACPCENT_REFCB_GAUGE);
        ram->swap_usedpc->record_defaultflag = 1;

        /* Mark the ram container as needing a rebuild */
        static_ram_cnt_invalidated = 1;
      }
      else if (strcmp(obj->desc_str, "Cached memory") == 0)
      {
        /* Clean */
        if (ram->cached_alloc) { i_entity_deregister (self, ENTITY(ram->cached_alloc)); i_entity_free (ENTITY(ram->cached_alloc)); ram->cached_alloc = NULL;}
        if (ram->cached) { i_entity_deregister (self, ENTITY(ram->cached)); i_entity_free (ENTITY(ram->cached)); ram->cached = NULL; }

        /* Cached Alloc units */
        ram->cached_alloc = l_snmp_metric_create (self, real_obj, "cached_alloc", "Cached Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
        ram->cached_alloc->hidden = 1;

        /* Cached Total -- Uses 'size' OID, not 'Used' (Used is not populated) */
        ram->cached = l_snmp_metric_create (self, real_obj, "cached", "Cached", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_RRD, 0);
        ram->cached->alloc_unit_met = ram->cached_alloc;
        ram->cached->valstr_func = i_string_volume_metric;
        ram->cached->unit_str = strdup ("byte");
        ram->cached->kbase = 1024;
        ram->cached->record_defaultflag = 1;
        
        /* Mark the ram container as needing a rebuild */
        static_ram_cnt_invalidated = 1;
        
        /* Log */
        i_printf(0, "l_snmp_storage_objfact_fab encountered '%s' object, using it for Cached Memory data (%s.%s)", obj->desc_str, ".1.3.6.1.2.1.25.2.3.1.6", index_oidstr);
      }
      else if ((strcmp(obj->desc_str, "Memory buffers") == 0 || strcmp(obj->desc_str, "Memory Buffers") == 0))
      {
        /* Log */
        i_printf(0, "l_snmp_storage_objfact_fab encountered '%s' object, using it for Memory Buffers data", obj->desc_str);

        /* Clean */
        if (ram->buffers_alloc) { i_entity_deregister (self, ENTITY(ram->buffers_alloc)); i_entity_free (ENTITY(ram->buffers_alloc)); ram->buffers_alloc = NULL; }
        if (ram->buffers) { i_entity_deregister (self, ENTITY(ram->buffers)); i_entity_free (ENTITY(ram->buffers)); ram->buffers = NULL; }

        /* Buffered Alloc units */
        ram->buffers_alloc = l_snmp_metric_create (self, real_obj, "buffers_alloc", "Buffer Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
        ram->buffers_alloc->hidden = 1;

        /* Buffered Total -- Uses 'size' OID, not 'used' (Used is not populated) */
        ram->buffers = l_snmp_metric_create (self, real_obj, "buffers", "Buffers", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_RRD, 0);
        ram->buffers->alloc_unit_met = ram->buffers_alloc;
        ram->buffers->valstr_func = i_string_volume_metric;
        ram->buffers->unit_str = strdup ("byte");
        ram->buffers->kbase = 1024;
        ram->buffers->record_defaultflag = 1;
        
        /* Mark the ram container as needing a rebuild */
        static_ram_cnt_invalidated = 1;
      }
      else if (strcmp(obj->desc_str, "Shared memory") == 0)
      {
        /* Clean */
        if (ram->shared_alloc) { i_entity_deregister (self, ENTITY(ram->shared_alloc)); i_entity_free (ENTITY(ram->shared_alloc)); ram->shared_alloc = NULL; }
        if (ram->shared) { i_entity_deregister (self, ENTITY(ram->shared)); i_entity_free (ENTITY(ram->shared)); ram->shared = NULL; }

        /* Shared Alloc units */
        ram->shared_alloc = l_snmp_metric_create (self, real_obj, "shared_alloc", "Shared Allocation Units", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
        ram->shared_alloc->hidden = 1;

        /* Shared Total -- Uses 'size' OID, not 'used' (Used is not populated) */
        ram->shared = l_snmp_metric_create (self, real_obj, "shared", "Shared", METRIC_GAUGE, ".1.3.6.1.2.1.25.2.3.1.5", index_oidstr, RECMETHOD_RRD, 0);
        ram->shared->alloc_unit_met = ram->shared_alloc;
        ram->shared->valstr_func = i_string_volume_metric;
        ram->shared->unit_str = strdup ("byte");
        ram->shared->kbase = 1024;
        ram->shared->record_defaultflag = 1;
        
        /* Mark the ram container as needing a rebuild */
        static_ram_cnt_invalidated = 1;
        
        /* Log */
        i_printf(0, "l_snmp_storage_objfact_fab encountered '%s' object, using it for Shared Memory data (%s.%s)", obj->desc_str, ".1.3.6.1.2.1.25.2.3.1.6", index_oidstr);

      }
    }
  }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_storage_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;

    /* If Xsnmp is not used, enable hrFilesys to augment storage info */
    if (!l_snmp_xsnmp_enabled() && !l_snmp_hrfilesys_enabled())
    { 
      i_printf (0, "l_snmp_storage_enable automatically enabling hrfilesys to augment hrstorage info without Xsnmp"); 
      l_snmp_hrfilesys_enable (self); 
    }

    /* Xsnmp is not used, and the RAM container needs rebuilding */
    if (static_ram_cnt_invalidated == 1 && !l_snmp_xsnmp_enabled())
    {
      /* Log */
      i_printf(1, "l_snmp_storage_objfact_ctrl rebuilding the RAM container due to changes in storage resources");

      /* Update the memory monitoring container */    
      i_container *ram_cnt = l_snmp_nsram_cnt();
      if (ram_cnt)
      {
        i_object *real_obj = (i_object *) i_entity_child_get(ENTITY(ram_cnt), "master");
        i_object *swap_obj = (i_object *) i_entity_child_get(ENTITY(ram_cnt), "swap");
        l_snmp_nsram_item *ram = real_obj->itemptr;

        if (ram->real_inactive) 
        { 
          i_entity_deregister (self, ENTITY(ram->real_inactive)); 
          i_entity_free (ENTITY(ram->real_inactive)); 
        }
        if (ram->real_totalactive) 
        { 
          i_entity_deregister (self, ENTITY(ram->real_totalactive)); 
          i_entity_free (ENTITY(ram->real_totalactive)); 
        }
        if (ram->real_usedpc) 
        { 
          i_entity_deregister (self, ENTITY(ram->real_usedpc)); 
          i_entity_free (ENTITY(ram->real_usedpc)); 
        }
        if (ram->real_activepc) 
        { 
          i_entity_deregister (self, ENTITY(ram->real_activepc)); 
          i_entity_free (ENTITY(ram->real_activepc)); 
        }
        ram->real_inactive = i_metric_acsum_create (self, real_obj, "real_inactive", "Inactive", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
        if (ram->shared)  i_metric_acsum_addmet (ram->real_inactive, ram->shared, 0);
        if (ram->buffers)  i_metric_acsum_addmet (ram->real_inactive, ram->buffers, 0);
        if (ram->cached)  i_metric_acsum_addmet (ram->real_inactive, ram->cached, 0);
        ram->real_inactive->valstr_func = i_string_volume_metric;
        ram->real_inactive->kbase = 1024;
        ram->real_inactive->unit_str = strdup ("byte");
  
        ram->real_totalactive = i_metric_acdiff_create (self, real_obj, "real_totalactive", "Total Active", METRIC_FLOAT, RECMETHOD_RRD, ram->real_used, ram->real_inactive, ACDIFF_REFCB_YMET);
        ram->real_totalactive->valstr_func = i_string_volume_metric;
        ram->real_totalactive->unit_str = strdup ("byte");
        ram->real_totalactive->kbase = 1024;
        ram->real_totalactive->record_defaultflag = 1;

        ram->real_usedpc = i_metric_acpcent_create (self, real_obj, "real_usedpc", "Used Percent", RECMETHOD_RRD, ram->real_used, ram->real_total, ACPCENT_REFCB_GAUGE);
        ram->real_usedpc->record_defaultflag = 1;

        ram->real_activepc = i_metric_acpcent_create (self, real_obj, "real_activepc", "Active Percent", RECMETHOD_RRD, ram->real_totalactive, ram->real_total, ACPCENT_REFCB_GAUGE);
        ram->real_activepc->record_defaultflag = 1;

        i_triggerset_evalapprules_allsets (self, real_obj);
        l_record_eval_recrules_obj (self, real_obj);
        i_triggerset_evalapprules_allsets (self, swap_obj);
        l_record_eval_recrules_obj (self, swap_obj);
      }
      static_ram_cnt_invalidated = 0;
    }
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int l_snmp_storage_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_snmp_storage_item *store = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, store);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
