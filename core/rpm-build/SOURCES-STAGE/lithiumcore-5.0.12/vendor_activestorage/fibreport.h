typedef struct v_fibreport_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *loopid;
  struct i_metric_s *mode;
  struct i_metric_s *rate;
  struct i_metric_s *wwn;

} v_fibreport_item;

/* fibreport.c */
struct i_container_s* v_fibreport_cnt ();
int v_fibreport_enable (i_resource *self);

/* fibreport_item.c */
v_fibreport_item* v_fibreport_item_create ();
void v_fibreport_item_free (void *itemptr);

/* fibreport_objfact.c */
int v_fibreport_objfact_fab ();
int v_fibreport_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_fibreport_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

