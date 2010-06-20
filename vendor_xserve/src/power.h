typedef struct v_power_item_s
{
  struct i_object_s *obj;

  /* Xserve */
  struct i_metric_s *sc_vcore;
  struct i_metric_s *ddr_io;
  struct i_metric_s *ddr_io_sleep;
  struct i_metric_s *io_vdd;
  struct i_metric_s *v_1_2v;
  struct i_metric_s *v_1_2v_sleep;
  struct i_metric_s *v_1_5v;
  struct i_metric_s *v_1_5v_sleep;
  struct i_metric_s *v_1_8v;
  struct i_metric_s *v_3_3v;
  struct i_metric_s *v_3_3v_sleep;
  struct i_metric_s *v_3_3v_trickle;
  struct i_metric_s *v_5v;
  struct i_metric_s *v_5v_sleep;
  struct i_metric_s *v_12v;
  struct i_metric_s *v_12v_trickle;

} v_power_item;

/* power.c */
struct i_container_s* v_power_cnt ();
struct v_power_item_s* v_power_static_item ();
int v_power_enable (i_resource *self);
int v_power_disable (i_resource *self);

/* power_item.c */
v_power_item* v_power_item_create ();
void v_power_item_free (void *itemptr);

/* power_cntform.c */
int v_power_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* power_objform.c */
int v_power_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

