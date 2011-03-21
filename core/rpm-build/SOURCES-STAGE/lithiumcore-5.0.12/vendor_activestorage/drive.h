typedef struct v_drive_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *availsize;
  struct i_metric_s *status;
  struct i_metric_s *usage;
  struct i_metric_s *targettype;
  struct i_metric_s *totalsize;
  struct i_metric_s *speed;
  struct i_metric_s *firmware;
  struct i_metric_s *serial;
  struct i_metric_s *standard;

} v_drive_item;

/* drive.c */
struct i_container_s* v_drive_cnt ();
int v_drive_enable (i_resource *self);

/* drive_item.c */
v_drive_item* v_drive_item_create ();
void v_drive_item_free (void *itemptr);

/* drive_objfact.c */
int v_drive_objfact_fab ();
int v_drive_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_drive_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

