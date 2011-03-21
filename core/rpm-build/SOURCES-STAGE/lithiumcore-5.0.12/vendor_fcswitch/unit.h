typedef struct v_unit_item_s
{
  struct i_object_s *obj;
  char *oid_suffix;

  /* Switch Unit */
  struct i_metric_s *type;
  struct i_metric_s *port_count;
  struct i_metric_s *product;
  struct i_metric_s *serial;
  struct i_metric_s *uptime;
  struct i_metric_s *url;
  struct i_metric_s *proxy_master;
  struct i_metric_s *principal;

  /* Related Containers */
  struct i_container_s *port_cnt;
  struct i_container_s *sensor_cnt;
  struct i_container_s *revision_cnt;

} v_unit_item;

/* unit.c */
struct i_container_s* v_unit_cnt ();
int v_unit_enable (i_resource *self);
char* v_unit_oid_glue (char *unit_oid_suffix, char *oid);
v_unit_item* v_unit_item_create ();
void v_unit_item_free (void *itemptr);

/* unit_objfact.c */
int v_unit_objfact_fab ();
int v_unit_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_unit_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

