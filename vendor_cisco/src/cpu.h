typedef struct v_cpu_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *fivesec_pc;
  struct i_metric_s *onemin_pc;
  struct i_metric_s *fivemin_pc;

  struct i_metric_cgraph_s *load_cg;

} v_cpu_item;

/* cpu.c */
struct i_container_s* v_cpu_cnt ();
int v_cpu_enable (i_resource *self);
int v_cpu_disable (i_resource *self);

/* cpu_item.c */
v_cpu_item* v_cpu_item_create ();
void v_cpu_item_free (void *itemptr);

/* cpu_objfact.c */
int v_cpu_objfact_fab ();
int v_cpu_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_cpu_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* cpu_cntform.c */
int v_cpu_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* cpu_objform.c */
int v_cpu_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_cpu_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

