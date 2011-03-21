typedef struct v_fanps_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *rpsconn_status;
  struct i_metric_s *mainpsu_status;
  struct i_metric_s *redundantpsu_status;
  struct i_metric_s *rpsmonitoring_status;
  struct i_metric_s *mainfan_status;
  struct i_metric_s *redundantfan_status;
  struct i_metric_s *temperature_status;
  struct i_metric_s *fantray_present;
  struct i_metric_s *fantray_status;
  struct i_metric_s *mainmonitoring_status;
  struct i_metric_s *accelfan_status;

} v_fanps_item;

/* snmp_sysinfo.c */

int v_fanps_enabled ();
struct i_container_s* v_fanps_cnt ();
struct i_object_s* v_fanps_obj ();
int v_fanps_enable (i_resource *self);
int v_fanps_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_fanps_item* v_fanps_item_create ();
void v_fanps_item_free (void *itemptr);

/* snmp_sysinfo_cntform.c */

int v_fanps_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_fanps_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

