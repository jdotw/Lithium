typedef struct l_snmp_nsdiskio_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *write_bytes;
  struct i_metric_s *read_bytes;
  struct i_metric_s *write_count;
  struct i_metric_s *read_count;
  struct i_metric_s *write_rate;
  struct i_metric_s *read_rate;
  struct i_metric_s *write_tput;
  struct i_metric_s *read_tput;
  struct i_metric_s *write_avg;
  struct i_metric_s *read_avg;

} l_snmp_nsdiskio_item;

/* diskio.c */
struct i_container_s* l_snmp_nsdiskio_cnt ();
int l_snmp_nsdiskio_enable (i_resource *self);
int l_snmp_nsdiskio_disable (i_resource *self);

/* diskio_item.c */
l_snmp_nsdiskio_item* l_snmp_nsdiskio_item_create ();
void l_snmp_nsdiskio_item_free (void *itemptr);

/* diskio_objfact.c */
int l_snmp_nsdiskio_objfact_fab ();
int l_snmp_nsdiskio_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_nsdiskio_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* diskio_cntform.c */
int l_snmp_nsdiskio_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* diskio_objform.c */
int l_snmp_nsdiskio_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_snmp_nsdiskio_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

