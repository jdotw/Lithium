typedef struct v_temp_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *control;
  struct i_metric_s *rtn;
  struct i_metric_s *supply;

  struct i_metric_s *control_high_thresh;
  struct i_metric_s *return_high_thresh;

  struct i_metric_s *control_low_thresh;
  struct i_metric_s *return_low_thresh;

  struct i_metric_cgraph_s *temp_cg;

} v_temp_item;

/* snmp_sysinfo.c */

int v_temp_enabled ();
struct i_container_s* v_temp_cnt ();
struct i_object_s* v_temp_obj ();
int v_temp_enable (i_resource *self);
int v_temp_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_temp_item* v_temp_item_create ();
void v_temp_item_free (void *itemptr);

/* snmp_sysinfo_cntform.c */

int v_temp_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_temp_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_sumform.c */

int v_temp_sumform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

