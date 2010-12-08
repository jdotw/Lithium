typedef struct v_fans_item_s
{
  struct i_object_s *obj;

  /* Xserve - PPC */
  struct i_metric_s *rpm;

  /* Xserve - Intel */
  struct i_metric_s *in_rpm;
  struct i_metric_s *out_rpm;

} v_fans_item;

/* fans.c */
struct i_container_s* v_fans_cnt ();
v_fans_item* v_fans_get (char *desc_str);
int v_fans_enable (i_resource *self);
v_fans_item* v_fans_create (i_resource *self, char *desc_str);
int v_fans_disable (i_resource *self);

/* fans_item.c */
v_fans_item* v_fans_item_create ();
void v_fans_item_free (void *itemptr);

/* fans_cntform.c */
int v_fans_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* fans_objform.c */
int v_fans_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_fans_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);


