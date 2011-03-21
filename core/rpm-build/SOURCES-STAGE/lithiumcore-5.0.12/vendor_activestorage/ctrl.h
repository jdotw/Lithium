typedef struct v_ctrl_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *ramsize;
  struct i_metric_s *ramtype;
  struct i_metric_s *serial;
  struct i_metric_s *model;
  struct i_metric_s *firmware;
  struct i_metric_s *bootcode;
  struct i_metric_s *ifacetype;
  struct i_metric_s *vendor;
  struct i_metric_s *product;
  struct i_metric_s *role;

} v_ctrl_item;

/* ctrl.c */
struct i_container_s* v_ctrl_cnt ();
int v_ctrl_enable (i_resource *self);

/* ctrl_item.c */
v_ctrl_item* v_ctrl_item_create ();
void v_ctrl_item_free (void *itemptr);

/* ctrl_objfact.c */
int v_ctrl_objfact_fab ();
int v_ctrl_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_ctrl_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

