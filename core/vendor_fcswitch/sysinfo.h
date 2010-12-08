typedef struct v_sysinfo_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *uptime;
  struct i_metric_s *descr;
  struct i_metric_s *contact;
  struct i_metric_s *name;
  struct i_metric_s *location;
  struct i_metric_s *vendor;
  struct i_metric_s *serial;
  struct i_metric_s *product;
  struct i_metric_s *type;

  struct i_metric_s *state;
  struct i_metric_s *status;

} v_sysinfo_item;

/* sysinfo.c */
struct i_container_s* v_sysinfo_cnt ();
struct v_sysinfo_item_s* v_sysinfo_static_item ();
int v_sysinfo_enable (i_resource *self);
int v_sysinfo_disable (i_resource *self);

/* sysinfo_item.c */
v_sysinfo_item* v_sysinfo_item_create ();
void v_sysinfo_item_free (void *itemptr);

/* sysinfo_objfact.c */
int v_sysinfo_objfact_fab ();
int v_sysinfo_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_sysinfo_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* sysinfo_cntform.c */
int v_sysinfo_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* sysinfo_objform.c */
int v_sysinfo_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_sysinfo_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

