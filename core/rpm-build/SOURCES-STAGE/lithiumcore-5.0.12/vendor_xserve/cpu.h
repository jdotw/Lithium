typedef struct v_cpu_item_s
{
  struct i_object_s *obj;

  /* OSX */
  struct i_metric_s *usage;

  /* Xserve (Common) */
  struct i_metric_s *power_watts;
  struct i_metric_s *power_vcore;
  struct i_metric_s *temp_inlet;

  /* Xserve (PPC only) */
  struct i_metric_s *temp_ambient;
  struct i_metric_s *temp_internal;
  struct i_metric_s *current_core;
  struct i_metric_s *current_12v;

  /* Xserve (Intel only) */
  struct i_metric_s *temp_heatsink;
  struct i_metric_s *v_12v;
  struct i_metric_s *usage_1;
  struct i_metric_s *usage_2;
  
} v_cpu_item;

/* cpu.c */
struct i_container_s* v_cpu_cnt ();
v_cpu_item* v_cpu_master ();
v_cpu_item* v_cpu_x (int x);
int v_cpu_enable (i_resource *self);
v_cpu_item* v_cpu_create ();
int v_cpu_disable (i_resource *self);

/* cpu_item.c */
v_cpu_item* v_cpu_item_create ();
void v_cpu_item_free (void *itemptr);

/* cpu_cntform.c */
int v_cpu_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* cpu_objform.c */
int v_cpu_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_cpu_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

