typedef struct v_logdrv_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *wwn;

  struct i_metric_s *opstate;
  struct i_metric_s *condition;
  struct i_metric_s *operation;
  struct i_metric_s *synchronized;

  struct i_metric_s *raidlevel;
  struct i_metric_s *arrayid;
  struct i_metric_s *axelcount;
  struct i_metric_s *usedpd;
  struct i_metric_s *sectorsize;

  struct i_metric_s *errors;
  struct i_metric_s *eps;
  struct i_metric_s *nonrw_errors;
  struct i_metric_s *read_errors;
  struct i_metric_s *write_errors;

} v_logdrv_item;

/* logdrv.c */
struct i_container_s* v_logdrv_cnt ();
int v_logdrv_enable (i_resource *self);
int v_logdrv_disable (i_resource *self);

/* logdrv_item.c */
v_logdrv_item* v_logdrv_item_create ();
void v_logdrv_item_free (void *itemptr);

/* logdrv_objfact.c */
int v_logdrv_objfact_fab ();
int v_logdrv_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_logdrv_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

