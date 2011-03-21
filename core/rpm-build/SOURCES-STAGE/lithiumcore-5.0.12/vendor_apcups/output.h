typedef struct v_output_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  /* Basic Data */
  struct i_metric_s *status;
  struct i_metric_s *phase;
  
  /* Advanced Data */
  struct i_metric_s *voltage;
  struct i_metric_s *frequency;
  struct i_metric_s *load;
  struct i_metric_s *current;

} v_output_item;

/* output.c */
struct i_container_s* v_output_cnt ();
int v_output_enable (i_resource *self);
int v_output_disable (i_resource *self);

/* output_item.c */
v_output_item* v_output_item_create ();
void v_output_item_free (void *itemptr);

/* output_objfact.c */
int v_output_objfact_fab ();
int v_output_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_output_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* output_cntform.c */
int v_output_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* output_objform.c */
int v_output_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_output_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

