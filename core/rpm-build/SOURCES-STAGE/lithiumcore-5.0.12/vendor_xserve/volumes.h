typedef struct v_volumes_item_s
{
  struct i_object_s *obj;

  /* Standard */
  struct i_metric_s *bytes_free;
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_used;
  struct i_metric_s *used_pc;

} v_volumes_item;

/* volumes.c */
struct i_container_s* v_volumes_cnt ();
v_volumes_item* v_volumes_get (char *desc_str);
int v_volumes_enable (i_resource *self);
v_volumes_item* v_volumes_create (i_resource *self, char *name_str);
int v_volumes_disable (i_resource *self);

/* volumes.c */
v_volumes_item* v_volumes_item_create ();
void v_volumes_item_free (void *itemptr);

/* volumes_cntform.c */
int v_volumes_cntform (i_resource *self,  struct i_entity_s *ent, i_form_reqdata *reqdata);

/* volumes_objform.c */
int v_volumes_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_volumes_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

