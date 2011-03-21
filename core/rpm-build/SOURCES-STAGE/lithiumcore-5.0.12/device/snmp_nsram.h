/* CPU Item */

typedef struct l_snmp_nsram_item_s
{
  /* Objects */
  struct i_object_s *real_obj;
  struct i_object_s *swap_obj;

  /* Real */
  struct i_metric_s *real_alloc;        /* Allocation units for real/physical memory */
  struct i_metric_s *real_total;        /* Total amount of RAM */
  struct i_metric_s *real_used;         /* Total Amount of RAM Used in any way (Wired + Active + Inactive + Buffers + Cache, etc) */
  struct i_metric_s *real_free;         /* Completed unused RAM */

  struct i_metric_s *real_wired;        /* Wired RAM (used by kernel, can not be swapped) */
  struct i_metric_s *real_active;       /* Active RAM (used by apps, can be swapped) */
  struct i_metric_s *real_totalactive;  /* Total of Active RAM (used by apps and kernel ) */
  struct i_metric_s *real_inactive;     /* Inactive RAM (can be freed) */
  
  struct i_metric_s *shared;            /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *shared_alloc;      /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *cached;            /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *cached_alloc;      /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *buffers;           /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *buffers_alloc;     /* As reported by SNMP -- Not on OS X */

  struct i_metric_s *real_usedpc;       /* Calculated on free/total */
  struct i_metric_s *real_activepc;     /* Calculated on totalactive/total */

  /* Swap */
  struct i_metric_s *swap_total;
  struct i_metric_s *swap_avail;
  struct i_metric_s *swap_used;
  struct i_metric_s *swap_usedpc;
  struct i_metric_s *swap_alloc;
  struct i_metric_s *swap_in;
  struct i_metric_s *swap_out;

} l_snmp_nsram_item;

/* ram.c */
struct i_container_s* l_snmp_nsram_cnt ();
int l_snmp_nsram_enable (i_resource *self);
int l_snmp_nsram_disable (i_resource *self);

/* ram_refcb.c */
int l_snmp_nsram_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

/* ram_item.c */
l_snmp_nsram_item* l_snmp_nsram_item_create ();
void l_snmp_nsram_item_free (void *itemptr);

/* ram_cntform.c */
int l_snmp_nsram_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* ram_objform.c */
int l_snmp_nsram_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_snmp_nsram_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

