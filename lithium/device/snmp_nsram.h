/* CPU Item */

typedef struct l_snmp_nsram_item_s
{
  /* Objects */
  struct i_object_s *real_obj;
  struct i_object_s *swap_obj;

  /* Real */
  struct i_metric_s *real_total;        /* As reported by SNMP */
  struct i_metric_s *real_avail;        /* Free as reported by SNMP */
  struct i_metric_s *real_alloc;        /* Allocation for real/physical memory */
  struct i_metric_s *real_used;         /* Used as reported by SNMP */
  struct i_metric_s *real_free;         /* Available (calculated from free + cached + buffers) */
  struct i_metric_s *real_active;       /* Active Used (calculated from total - free) */
  struct i_metric_s *real_usedpc;       /* Calculated on free/total */
  struct i_metric_s *shared;            /* As reported by SNMP -- Not on OS X*/
  struct i_metric_s *shared_alloc;      /* As reported by SNMP -- Not on OS X*/
  struct i_metric_s *cached;            /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *cached_alloc;      /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *buffers;           /* As reported by SNMP -- Not on OS X */
  struct i_metric_s *buffers_alloc;     /* As reported by SNMP -- Not on OS X */

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

