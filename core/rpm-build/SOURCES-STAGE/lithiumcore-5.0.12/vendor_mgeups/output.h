typedef struct v_output_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;

  struct i_metric_s *onbattery;
  struct i_metric_s *onbypass;
  struct i_metric_s *bypassunavail;
  struct i_metric_s *utilityoff;
  struct i_metric_s *nobypass;
  struct i_metric_s *onboost;
  struct i_metric_s *inverteroff;
  struct i_metric_s *overload;
  struct i_metric_s *overtemp;
  struct i_metric_s *ontrim;
  
} v_output_item;

/* output.c */
struct i_container_s* v_output_cnt ();
int v_output_enable (i_resource *self);
int v_output_disable (i_resource *self);

/* output_refcb.c */
int v_output_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

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

