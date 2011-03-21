typedef struct v_mainboard_item_s
{
  struct i_object_s *obj;

  /* Xserve - Intel */
  struct i_metric_s *fbdimm_vrm_12v;
  struct i_metric_s *standby;
  struct i_metric_s *main_3_3v;
  struct i_metric_s *main_12v;
  struct i_metric_s *ns_bridge_power;
  struct i_metric_s *nbridge_temp;

} v_mainboard_item;

/* mainboard.c */
struct i_container_s* v_mainboard_cnt ();
struct v_mainboard_item_s* v_mainboard_static_item ();
int v_mainboard_enable (i_resource *self);
int v_mainboard_disable (i_resource *self);

/* mainboard_item.c */
v_mainboard_item* v_mainboard_item_create ();
void v_mainboard_item_free (void *itemptr);

/* mainboard_cntform.c */
int v_mainboard_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* mainboard_objform.c */
int v_mainboard_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_mainboard_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

