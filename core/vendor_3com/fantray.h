typedef struct v_fantray_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  
  struct i_metric_s *ent_type;
  struct i_metric_s *hw_ver;
  struct i_metric_s *sw_ver;
  struct i_metric_s *state;
  struct i_metric_s *fault;

} v_fantray_item;

/* fantray.c */
struct i_container_s* v_fantray_cnt ();
int v_fantray_enable (i_resource *self);
int v_fantray_disable (i_resource *self);

/* fantray_item.c */
v_fantray_item* v_fantray_item_create ();
void v_fantray_item_free (void *itemptr);

/* fantray_objfact.c */
int v_fantray_objfact_fab ();
int v_fantray_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_fantray_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* fantray_cntform.c */
int v_fantray_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* fantray_objform.c */
int v_fantray_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_fantray_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

