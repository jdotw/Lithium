/* CPU Item */

typedef struct l_snmp_nsload_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* load variables */
  struct i_metric_s *one_min;
  struct i_metric_s *one_min_raw;
  struct i_metric_s *five_min;
  struct i_metric_s *five_min_raw;
  struct i_metric_s *fifteen_min;
  struct i_metric_s *fifteen_min_raw;

  /* misc */
  struct i_metric_s *cswitches;
  struct i_metric_s *interrupts;
  struct i_metric_s *io_sent;
  struct i_metric_s *io_recv;

  /* cgraph */
  struct i_metric_cgraph_s *load_cg;

} l_snmp_nsload_item;

/* load.c */
struct i_container_s* l_snmp_nsload_cnt ();
int l_snmp_nsload_enable (i_resource *self);
int l_snmp_nsload_disable (i_resource *self);

/* load_refcb.c */
int l_snmp_nsload_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

/* load_item.c */
l_snmp_nsload_item* l_snmp_nsload_item_create ();
void l_snmp_nsload_item_free (void *itemptr);

/* load_cntform.c */
int l_snmp_nsload_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* load_objform.c */
int l_snmp_nsload_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_snmp_nsload_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

