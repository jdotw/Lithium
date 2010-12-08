typedef struct v_enclosure_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *type;
  struct i_metric_s *opstate;
  struct i_metric_s *opstate_desc;

} v_enclosure_item;

/* enclosure.c */
struct i_container_s* v_enclosure_cnt ();
int v_enclosure_enable (i_resource *self);
int v_enclosure_disable (i_resource *self);

/* enclosure_item.c */
v_enclosure_item* v_enclosure_item_create ();
void v_enclosure_item_free (void *itemptr);

/* enclosure_objfact.c */
int v_enclosure_objfact_fab ();
int v_enclosure_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_enclosure_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

