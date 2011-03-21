typedef struct v_state_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *system;
  struct i_metric_s *humidifier;
  struct i_metric_s *dehumidifier;
  struct i_metric_s *systemfan;
  struct i_metric_s *systemalarm;
  struct i_metric_s *cooling_pc;
  struct i_metric_s *heating_pc;
  struct i_metric_s *audible_alarm;
  struct i_metric_s *cooling_unit_1;
  struct i_metric_s *cooling_unit_2;
  struct i_metric_s *heating_unit_1;
  struct i_metric_s *heating_unit_2;

  struct i_metric_cgraph_s *state_cg;
  
} v_state_item;

/* snmp_sysinfo.c */

int v_state_enabled ();
struct i_container_s* v_state_cnt ();
struct i_object_s* v_state_obj ();
int v_state_enable (i_resource *self);
int v_state_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_state_item* v_state_item_create ();
void v_state_item_free (void *istatetr);

/* snmp_sysinfo_cntform.c */

int v_state_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_sumform.c */

int v_state_sumform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_state_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

