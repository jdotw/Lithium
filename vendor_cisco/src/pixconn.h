typedef struct v_pixconn_item_s
{
  struct i_object_s *obj;
  unsigned long service;
  
  struct i_metric_s *curconn;
  struct i_metric_s *highconn;

} v_pixconn_item;

/* pixconn.c */
struct i_container_s* v_pixconn_cnt ();
int v_pixconn_enable (i_resource *self);
int v_pixconn_disable (i_resource *self);

/* pixconn_item.c */
v_pixconn_item* v_pixconn_item_create ();
void v_pixconn_item_free (void *itemptr);

/* pixconn_objfact.c */
int v_pixconn_objfact_fab ();
int v_pixconn_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_pixconn_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* pixconn_cntform.c */
int v_pixconn_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* pixconn_objform.c */
int v_pixconn_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_pixconn_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

