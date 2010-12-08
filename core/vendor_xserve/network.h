typedef struct v_network_item_s
{
  struct i_object_s *obj;

  /* Standard */
  struct i_metric_s *fullname;
  struct i_metric_s *bps_in;
  struct i_metric_s *bps_out;
  struct i_metric_s *ip;
  struct i_metric_s *subnetmask;
  struct i_metric_s *broadcast;
  struct i_metric_s *router;

  /* Xserve */
  struct i_metric_s *bytes_in;
  struct i_metric_s *bytes_out;
  struct i_metric_s *pps_in;
  struct i_metric_s *pps_out;
  struct i_metric_s *packets_in;
  struct i_metric_s *packets_out;
  struct i_metric_s *mac;
  struct i_metric_s *kind;
  struct i_metric_s *duplex;
  struct i_metric_s *speed;
  struct i_metric_s *linkstate;

  /* CG */
  struct i_metric_cgraph_s *tput_cg;

} v_network_item;

/* network.c */
struct i_container_s* v_network_cnt ();
v_network_item* v_network_get (char *name_str);
int v_network_enable (i_resource *self);
v_network_item* v_network_create (i_resource *self, char *name_str);
int v_network_disable (i_resource *self);

/* network_item.c */
v_network_item* v_network_item_create ();
void v_network_item_free (void *itemptr);

/* network_cntform.c */
int v_network_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* network_objform.c */
int v_network_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_network_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);


