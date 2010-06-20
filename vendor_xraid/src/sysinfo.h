typedef struct v_sysinfo_item_s
{
  struct i_object_s *obj;

//  struct i_metric_s *data;
  struct i_metric_s *name;
  struct i_metric_s *DN;
  struct i_metric_s *PN;
  struct i_metric_s *contact;
  struct i_metric_s *DS;
  struct i_metric_s *location;
  struct i_metric_s *version;
  struct i_metric_s *uptime;
  struct i_metric_s *Fl;
  struct i_metric_s *mac;
  struct i_metric_s *ip;
  struct i_metric_s *subnet;

  struct i_metric_s *firmware_top;
  struct i_metric_s *firmware_bottom;

  struct i_metric_s *serial;

} v_sysinfo_item;

/* sysinfo.c */
struct i_container_s* v_sysinfo_cnt ();
struct v_sysinfo_item_s* v_sysinfo_static_item ();
int v_sysinfo_enable (i_resource *self);
int v_sysinfo_disable (i_resource *self);

/* sysinfo_item.c */
v_sysinfo_item* v_sysinfo_item_create ();
void v_sysinfo_item_free (void *itemptr);

/* sysinfo_cntform.c */
int v_sysinfo_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* sysinfo_objform.c */
int v_sysinfo_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

