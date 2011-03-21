typedef struct v_client_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *type;
  struct i_metric_s *time_assoc;
  struct i_metric_s *strength;
  struct i_metric_s *noise;
  struct i_metric_s *rate;
  struct i_metric_s *rx_count;
  struct i_metric_s *tx_count;
  struct i_metric_s *rx_rate;
  struct i_metric_s *tx_rate;
  struct i_metric_s *tx_errors;
  struct i_metric_s *rx_errors;
  struct i_metric_s *tx_eps;
  struct i_metric_s *rx_eps;

} v_client_item;

/* client.c */
struct i_container_s* v_client_cnt ();
int v_client_enable (i_resource *self);
int v_client_disable (i_resource *self);

/* client_item.c */
v_client_item* v_client_item_create ();
void v_client_item_free (void *itemptr);

/* client_objfact.c */
int v_client_objfact_fab ();
int v_client_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_client_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* client_cntform.c */
int v_client_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* client_objform.c */
int v_client_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_client_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

