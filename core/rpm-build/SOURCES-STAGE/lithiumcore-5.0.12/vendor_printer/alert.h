typedef struct v_alert_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *severity;
  struct i_metric_s *group;
  struct i_metric_s *code;

} v_alert_item;

/* alert.c */
struct i_container_s* v_alert_cnt ();
int v_alert_enable (i_resource *self);
int v_alert_disable (i_resource *self);

/* alert_item.c */
v_alert_item* v_alert_item_create ();
void v_alert_item_free (void *itemptr);

/* alert_objfact.c */
int v_alert_objfact_fab ();
int v_alert_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_alert_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* alert_cntform.c */
int v_alert_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* alert_objform.c */
int v_alert_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
