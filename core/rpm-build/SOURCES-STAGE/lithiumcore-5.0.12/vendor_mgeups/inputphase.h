typedef struct v_inputphase_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  /* Advanced Data */
  struct i_metric_s *voltage;
  struct i_metric_s *max_voltage;
  struct i_metric_s *min_voltage;
  struct i_metric_s *frequency;

} v_inputphase_item;

/* input.c */
struct i_container_s* v_inputphase_cnt ();
int v_inputphase_enable (i_resource *self);
int v_inputphase_disable (i_resource *self);

/* input_item.c */
v_inputphase_item* v_inputphase_item_create ();
void v_inputphase_item_free (void *itemptr);

/* input_objfact.c */
int v_inputphase_objfact_fab ();
int v_inputphase_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_inputphase_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* input_cntform.c */
int v_inputphase_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* input_objform.c */
int v_inputphase_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_inputphase_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

