typedef struct v_supplies_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  int tset_applied;   /* 0=Not Yet / 1=Applied */
  
  struct i_metric_s *class;
  struct i_metric_s *type;
  struct i_metric_s *units;
  struct i_metric_s *max_capacity;
  struct i_metric_s *current_level;
  struct i_metric_s *remaining_pc;
  struct i_metric_s *fill_pc;

} v_supplies_item;

/* supplies.c */
struct i_container_s* v_supplies_cnt ();
int v_supplies_enable (i_resource *self);
int v_supplies_disable (i_resource *self);

/* supplies_item.c */
v_supplies_item* v_supplies_item_create ();
void v_supplies_item_free (void *itemptr);

/* supplies_objfact.c */
int v_supplies_objfact_fab ();
int v_supplies_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_supplies_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* supplies_cntform.c */
int v_supplies_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* supplies_objform.c */
int v_supplies_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_supplies_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* supplies_refcb.c */
int v_supplies_level_refcb (i_resource *self, struct i_metric_s *met, void *passdata);
int v_supplies_class_refcb (i_resource *self, struct i_metric_s *met, void *passdata);
  
