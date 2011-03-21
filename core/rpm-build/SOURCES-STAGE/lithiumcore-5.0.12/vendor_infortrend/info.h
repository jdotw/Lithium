typedef struct v_info_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *controller_name;
  struct i_metric_s *logo_vendor;
  struct i_metric_s *logo_model;

  struct i_metric_s *cpu;
  struct i_metric_s *cachesize;
  struct i_metric_s *memtype;

  struct i_metric_s *fwrev_major;
  struct i_metric_s *fwrev_minor;
  struct i_metric_s *fwrev_eng;
  
  struct i_metric_s *brrev_major;
  struct i_metric_s *brrev_minor;
  struct i_metric_s *brrev_eng;
  
  struct i_metric_s *serial;
  
} v_info_item;

/* info.c */
struct i_container_s* v_info_cnt ();
int v_info_enable (i_resource *self);
int v_info_disable (i_resource *self);

/* info_item.c */
v_info_item* v_info_item_create ();
void v_info_item_free (void *itemptr);

/* info_objfact.c */
int v_info_objfact_fab ();
int v_info_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_info_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* info_cntform.c */
int v_info_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* info_objform.c */
int v_info_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_info_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

