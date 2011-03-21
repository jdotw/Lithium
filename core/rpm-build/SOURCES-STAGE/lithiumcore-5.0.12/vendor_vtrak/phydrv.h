typedef struct v_phydrv_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *interface;
  struct i_metric_s *alias;
  struct i_metric_s *model;
  struct i_metric_s *serial;
  struct i_metric_s *firmware;
  struct i_metric_s *blocksize;
  struct i_metric_s *drivetype;

  struct i_metric_s *opstate;
  struct i_metric_s *condition;
  struct i_metric_s *operation;

  struct i_metric_s *arrayid;
  struct i_metric_s *seqnum;
  struct i_metric_s *enclosure;
  struct i_metric_s *slot;

  struct i_metric_s *errors;
  struct i_metric_s *eps;
  struct i_metric_s *nonrw_errors;
  struct i_metric_s *read_errors;
  struct i_metric_s *write_errors;

} v_phydrv_item;

/* phydrv.c */
struct i_container_s* v_phydrv_cnt ();
int v_phydrv_enable (i_resource *self);
int v_phydrv_disable (i_resource *self);

/* phydrv_item.c */
v_phydrv_item* v_phydrv_item_create ();
void v_phydrv_item_free (void *itemptr);

/* phydrv_objfact.c */
int v_phydrv_objfact_fab ();
int v_phydrv_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_phydrv_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

