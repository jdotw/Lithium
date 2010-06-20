typedef struct v_physical_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *desc;
  struct i_metric_s *speed;
  struct i_metric_s *state;
  struct i_metric_s *duplex;
  struct i_metric_s *rx_count;
  struct i_metric_s *tx_count;
  struct i_metric_s *rx_rate;
  struct i_metric_s *tx_rate;
  struct i_metric_s *tx_errors;
  struct i_metric_s *rx_errors;
  struct i_metric_s *tx_eps;
  struct i_metric_s *rx_eps;

  struct i_metric_cgraph_s *tput_cg;

} v_physical_item;

/* physical.c */
struct i_container_s* v_physical_cnt ();
int v_physical_enable (i_resource *self);
int v_physical_disable (i_resource *self);

/* physical_item.c */
v_physical_item* v_physical_item_create ();
void v_physical_item_free (void *itemptr);

/* physical_objfact.c */
int v_physical_objfact_fab ();
int v_physical_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_physical_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* physical_cntform.c */
int v_physical_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* physical_objform.c */
int v_physical_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_physical_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

