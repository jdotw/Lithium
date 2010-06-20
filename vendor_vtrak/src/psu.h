typedef struct v_psu_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *opstate;

} v_psu_item;

/* psu.c */
struct i_container_s* v_psu_cnt ();
int v_psu_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_psu_disable (i_resource *self);

/* psu_item.c */
v_psu_item* v_psu_item_create ();
void v_psu_item_free (void *itemptr);

/* psu_objfact.c */
int v_psu_objfact_fab ();
int v_psu_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_psu_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

