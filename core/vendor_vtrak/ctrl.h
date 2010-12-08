typedef struct v_ctrl_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *alias;
  struct i_metric_s *wwn;
  struct i_metric_s *part;
  struct i_metric_s *serial;
  struct i_metric_s *revision;
  struct i_metric_s *software;
  struct i_metric_s *firmware;
  struct i_metric_s *role;
  struct i_metric_s *readiness;

  struct i_metric_s *arraycount;

  struct i_metric_s *phydrvcount;
  struct i_metric_s *phydrvonline;
  struct i_metric_s *phydrvoffline;
  struct i_metric_s *phydrvfpa;
  struct i_metric_s *phydrvrebuild;
  struct i_metric_s *phydrvmissing;
  struct i_metric_s *phydrvunconfig;
  struct i_metric_s *logdrvcount;
  struct i_metric_s *logdrvonline;
  struct i_metric_s *logdrvoffline;
  struct i_metric_s *logdrvcritical;

  struct i_metric_s *globalspares;
  struct i_metric_s *dedspares;
  struct i_metric_s *rvtglobalspares;
  struct i_metric_s *rvtglobalspares_used;
  struct i_metric_s *rvtdedspares;
  struct i_metric_s *rvtdedspares_used;

  struct i_metric_s *bgacount;

  struct i_metric_s *dirtycache;
  struct i_metric_s *cacheused;

  struct i_metric_s *errors;
  struct i_metric_s *eps;
  struct i_metric_s *nonrw_errors;
  struct i_metric_s *read_errors;
  struct i_metric_s *write_errors;

} v_ctrl_item;

/* ctrl.c */
struct i_container_s* v_ctrl_cnt ();
int v_ctrl_enable (i_resource *self);
int v_ctrl_disable (i_resource *self);

/* ctrl_item.c */
v_ctrl_item* v_ctrl_item_create ();
void v_ctrl_item_free (void *itemptr);

/* ctrl_objfact.c */
int v_ctrl_objfact_fab ();
int v_ctrl_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_ctrl_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

