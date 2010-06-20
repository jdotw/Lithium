typedef struct v_template_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *opstate;

} v_template_item;

/* template.c */
struct i_container_s* v_template_cnt ();
int v_template_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_template_disable (i_resource *self);

/* template_item.c */
v_template_item* v_template_item_create ();
void v_template_item_free (void *itemptr);

/* template_objfact.c */
int v_template_objfact_fab ();
int v_template_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_template_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

