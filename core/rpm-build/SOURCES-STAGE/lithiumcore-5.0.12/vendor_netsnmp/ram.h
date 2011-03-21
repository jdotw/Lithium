/* CPU Item */

typedef struct l_snmp_nsram_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* Quantities */
  struct i_metric_s *swap_total;
  struct i_metric_s *swap_avail;
  struct i_metric_s *swap_used;
  struct i_metric_s *swap_usedpc;
  struct i_metric_s *real_total;
  struct i_metric_s *real_avail;
  struct i_metric_s *real_used;
  struct i_metric_s *real_usedpc;
  struct i_metric_s *shared;
  struct i_metric_s *cached;
  struct i_metric_s *buffers;

  /* Swap */
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

