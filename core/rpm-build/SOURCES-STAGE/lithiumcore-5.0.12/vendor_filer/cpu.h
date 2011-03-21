/* CPU Item */

typedef struct v_cpu_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* cpu variables */
  struct i_metric_s *busy_pc;
  struct i_metric_s *cpu_count;

} v_cpu_item;

/* cpu.c */
struct i_container_s* v_cpu_cnt ();
int v_cpu_enable (i_resource *self);
int v_cpu_disable (i_resource *self);

/* cpu_item.c */
v_cpu_item* v_cpu_item_create ();
void v_cpu_item_free (void *itemptr);

/* cpu_cntform.c */
int v_cpu_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* cpu_objform.c */
int v_cpu_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_cpu_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata);

