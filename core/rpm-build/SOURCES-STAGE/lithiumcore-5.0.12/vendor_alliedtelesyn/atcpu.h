typedef struct v_atcpu_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *onesec_avg;
  struct i_metric_s *tensec_avg;
  struct i_metric_s *onemin_avg;
  struct i_metric_s *fivemin_avg;
  struct i_metric_s *fivemin_max;

} v_atcpu_item;

/* snmp_sysinfo.c */

int v_atcpu_enabled ();
struct i_container_s* v_atcpu_cnt ();
struct i_object_s* v_atcpu_obj ();
int v_atcpu_enable (i_resource *self);
int v_atcpu_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_atcpu_item* v_atcpu_item_create ();
void v_atcpu_item_free (void *itemptr);

/* snmp_sysinfo_cntform.c */

int v_atcpu_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_atcpu_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

