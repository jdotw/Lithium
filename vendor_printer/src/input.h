typedef struct v_input_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *type;
  struct i_metric_s *media;
  struct i_metric_s *units;
  struct i_metric_s *max_capacity;
  struct i_metric_s *current_level;
  struct i_metric_s *remaining_pc;

} v_input_item;

/* input.c */
struct i_container_s* v_input_cnt ();
int v_input_enable (i_resource *self);
int v_input_disable (i_resource *self);

/* input_item.c */
v_input_item* v_input_item_create ();
void v_input_item_free (void *itemptr);

/* input_objfact.c */
int v_input_objfact_fab ();
int v_input_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_input_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* input_cntform.c */
int v_input_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* input_objform.c */
int v_input_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_input_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* input_refcb.c */
int v_input_level_refcb (i_resource *self, struct i_metric_s *met, void *passdata);

