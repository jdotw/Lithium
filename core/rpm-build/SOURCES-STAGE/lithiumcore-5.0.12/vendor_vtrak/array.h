typedef struct v_array_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *wwn;
  
  struct i_metric_s *opstate;
  struct i_metric_s *condition;
  struct i_metric_s *operation;

  struct i_metric_s *mediapatrol;
  struct i_metric_s *pdmenabled;
  struct i_metric_s *phydrvcount;
  struct i_metric_s *logdrvcount;
  struct i_metric_s *dedspares;

  struct i_metric_s *phydrvs;
  struct i_metric_s *logdrvs;
  struct i_metric_s *sparedrvs;

} v_array_item;

/* array.c */
struct i_container_s* v_array_cnt ();
int v_array_enable (i_resource *self);
int v_array_disable (i_resource *self);

/* array_item.c */
v_array_item* v_array_item_create ();
void v_array_item_free (void *itemptr);

/* array_objfact.c */
int v_array_objfact_fab ();
int v_array_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_array_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);


