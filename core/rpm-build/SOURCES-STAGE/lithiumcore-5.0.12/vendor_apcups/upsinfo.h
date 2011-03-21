typedef struct v_upsinfo_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  /* Basic Data */
  struct i_metric_s *model;
  struct i_metric_s *name;
  
  /* Advanced Data */
  struct i_metric_s *firmware;
  struct i_metric_s *manufacture_date;
  struct i_metric_s *serial;

} v_upsinfo_item;

/* upsinfo.c */
struct i_container_s* v_upsinfo_cnt ();
int v_upsinfo_enable (i_resource *self);
int v_upsinfo_disable (i_resource *self);

/* upsinfo_item.c */
v_upsinfo_item* v_upsinfo_item_create ();
void v_upsinfo_item_free (void *itemptr);

/* upsinfo_objfact.c */
int v_upsinfo_objfact_fab ();
int v_upsinfo_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_upsinfo_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* upsinfo_cntform.c */
int v_upsinfo_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* upsinfo_objform.c */
int v_upsinfo_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_upsinfo_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

