typedef struct v_device_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *rating;

} v_device_item;

/* device.c */
struct i_container_s* v_device_cnt ();
int v_device_enable (i_resource *self);
int v_device_disable (i_resource *self);

/* device_item.c */
v_device_item* v_device_item_create ();
void v_device_item_free (void *itemptr);

/* device_objfact.c */
int v_device_objfact_fab ();
int v_device_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_device_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* device_cntform.c */
int v_device_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* device_objform.c */
int v_device_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_device_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

