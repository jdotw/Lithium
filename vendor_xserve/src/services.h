typedef struct v_services_item_s
{
  struct i_object_s *obj;

  /* Standard */
  struct i_metric_s *state;
  struct i_metric_s *start_time;
  struct i_metric_s *throughput;
  struct i_metric_s *volume;
  struct i_metric_s *rate;

} v_services_item;

/* services.c */
struct i_container_s* v_services_cnt ();
v_services_item* v_services_get (char *name_str);
int v_services_enable (i_resource *self);
v_services_item* v_services_create (i_resource *self, char *name, char *desc, char *volume_desc, char *rate_desc, int tput_flag);
int v_services_disable (i_resource *self);

/* services_item.c */
v_services_item* v_services_item_create ();
void v_services_item_free (void *itemptr);

/* services_cntform.c */
int v_services_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* services_objform.c */
int v_services_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_services_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

