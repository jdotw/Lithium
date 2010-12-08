typedef struct v_humid_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *control;
  struct i_metric_s *supply;
  struct i_metric_s *rtn;

  struct i_metric_s *control_high_thresh;
  struct i_metric_s *return_high_thresh;

  struct i_metric_s *control_low_thresh;
  struct i_metric_s *return_low_thresh;

  struct i_metric_cgraph_s *humid_cg;

} v_humid_item;

/* snmp_sysinfo.c */

int v_humid_enabled ();
struct i_container_s* v_humid_cnt ();
struct i_object_s* v_humid_obj ();
int v_humid_enable (i_resource *self);
int v_humid_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_humid_item* v_humid_item_create ();
void v_humid_item_free (void *ihumidtr);

/* snmp_sysinfo_cntform.c */

int v_humid_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_sumform.c */

int v_humid_sumform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_humid_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

