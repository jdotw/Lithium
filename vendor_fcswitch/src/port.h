typedef struct v_port_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  int switch_domain;
  int switch_port;
  int desc_updated;

  /* Port info */
  struct i_metric_s *speed;
  struct i_metric_s *speed_kbyte;
  struct i_metric_s *state;
  struct i_metric_s *hwstate;
  struct i_metric_s *status;
  struct i_metric_s *wwn;

  /* Stats */
  struct i_metric_s *octets_out;
  struct i_metric_s *octets_in;
  struct i_metric_s *bps_out;
  struct i_metric_s *bps_in;
  struct i_metric_s *utilpc_in;
  struct i_metric_s *utilpc_out;
  struct i_metric_s *errors;
  struct i_metric_s *eps;

  struct i_metric_cgraph_s *tput_cg;

} v_port_item;

/* port.c */
struct i_container_s* v_port_cnt ();
int v_port_enable (i_resource *self);
int v_port_disable (i_resource *self);

/* port_item.c */
v_port_item* v_port_item_create ();
void v_port_item_free (void *itemptr);

/* port_objfact.c */
int v_port_objfact_fab ();
int v_port_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_port_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* port_cntform.c */
int v_port_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* port_objform.c */
int v_port_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_port_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* port_refcb.c */
int v_port_speed_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

