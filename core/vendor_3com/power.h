typedef struct v_power_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  
  struct i_metric_s *ent_type;
  struct i_metric_s *hw_ver;
  struct i_metric_s *sw_ver;
  struct i_metric_s *state;
  struct i_metric_s *fault;

} v_power_item;

/* power.c */
struct i_container_s* v_power_cnt ();
int v_power_enable (i_resource *self);
int v_power_disable (i_resource *self);

/* power_item.c */
v_power_item* v_power_item_create ();
void v_power_item_free (void *itemptr);

/* power_objfact.c */
int v_power_objfact_fab ();
int v_power_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_power_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* power_cntform.c */
int v_power_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* power_objform.c */
int v_power_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_power_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

