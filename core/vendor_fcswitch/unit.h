typedef struct v_unit_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  int switch_domain;

  /* Switch Unit */
  struct i_metric_s *type;
  struct i_metric_s *port_count;
  struct i_metric_s *state;
  struct i_metric_s *status;
  struct i_metric_s *product;
  struct i_metric_s *serial;
  struct i_metric_s *uptime;
  struct i_metric_s *url;
  struct i_metric_s *proxy_master;
  struct i_metric_s *principal;
  struct i_metric_s *sensor_count;
  struct i_metric_s *switchname;
  struct i_metric_s *unitinfo;

} v_unit_item;

/* unit.c */
struct i_container_s* v_unit_cnt ();
int v_unit_enable (i_resource *self);
int v_unit_disable (i_resource *self);

/* unit_item.c */
v_unit_item* v_unit_item_create ();
void v_unit_item_free (void *itemptr);

/* unit_objfact.c */
int v_unit_objfact_fab ();
int v_unit_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_unit_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* unit_cntform.c */
int v_unit_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* unit_objform.c */
int v_unit_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_unit_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* unit_refcb.c */
int v_unit_switchname_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

