/* CPU Item */

typedef struct v_load_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* load variables */
  struct i_metric_s *one_min;
  struct i_metric_s *one_min_raw;
  struct i_metric_s *five_min;
  struct i_metric_s *five_min_raw;
  struct i_metric_s *fifteen_min;
  struct i_metric_s *fifteen_min_raw;

  /* cgraph */
  struct i_metric_cgraph_s *load_cg;

} v_load_item;

/* load.c */
struct i_container_s* v_load_cnt ();
int v_load_enable (i_resource *self);
int v_load_disable (i_resource *self);

/* load_refcb.c */
int v_load_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

/* load_item.c */
v_load_item* v_load_item_create ();
void v_load_item_free (void *itemptr);

/* load_cntform.c */
int v_load_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* load_objform.c */
int v_load_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_load_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

