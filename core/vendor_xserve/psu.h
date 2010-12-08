typedef struct v_psu_item_s
{
  struct i_object_s *obj;

  /* Xserve - Intel */
  struct i_metric_s *exhaust_temp;
  struct i_metric_s *fan_in_rpm;
  struct i_metric_s *fan_out_rpm;
  struct i_metric_s *v_5v_standby;
  struct i_metric_s *v_12v;
  struct i_metric_s *power;

} v_psu_item;

/* psu.c */
struct i_container_s* v_psu_cnt ();
v_psu_item* v_psu_get (char *desc_str);
int v_psu_enable (i_resource *self);
v_psu_item* v_psu_create (i_resource *self, char *name_str);
int v_psu_disable (i_resource *self);

/* psu_item.c */
v_psu_item* v_psu_item_create ();
void v_psu_item_free (void *itemptr);

/* psu_cntform.c */
int v_psu_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* psu_objform.c */
int v_psu_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_psu_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);


