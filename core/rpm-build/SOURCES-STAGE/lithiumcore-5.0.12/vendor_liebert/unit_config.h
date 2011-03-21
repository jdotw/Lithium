typedef struct v_config_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *restart_delay;
  struct i_metric_s *remote_shutdown;
  struct i_metric_s *cooling_maint;
  struct i_metric_s *humidifier_maint;
  struct i_metric_s *filter_maint;

} v_config_item;

/* snmp_sysinfo.c */

int v_config_enabled ();
struct i_container_s* v_config_cnt ();
struct i_object_s* v_config_obj ();
int v_config_enable (i_resource *self);
int v_config_disable (i_resource *self);

/* snmp_sysinfo_item.c */

v_config_item* v_config_item_create ();
void v_config_item_free (void *iconfigtr);

/* snmp_sysinfo_cntform.c */

int v_config_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_sysinfo_objform.c */

int v_config_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

