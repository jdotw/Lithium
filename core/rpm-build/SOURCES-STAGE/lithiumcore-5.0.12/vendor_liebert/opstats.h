typedef struct v_opstats_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *comp1_rt;
  struct i_metric_s *comp2_rt;
  struct i_metric_s *fan_rt;
  struct i_metric_s *humidifier_rt;
  struct i_metric_s *reheat1_rt;
  struct i_metric_s *reheat2_rt;
  struct i_metric_s *cooling_rt;
  struct i_metric_s *heating_rt;
  struct i_metric_s *humidifying_rt;
  struct i_metric_s *dehumidifying_rt;

} v_opstats_item;

/* snmp_sysinfo.c */

int v_opstats_enabled ();
struct i_container_s* v_opstats_cnt ();
struct i_object_s* v_opstats_obj ();
int v_opstats_enable (i_resource *self);
int v_opstats_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_opstats_item* v_opstats_item_create ();
void v_opstats_item_free (void *iopstatstr);

/* snmp_sysinfo_cntform.c */

int v_opstats_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_opstats_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

