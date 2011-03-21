typedef struct v_ldrive_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *size_blocks;
  struct i_metric_s *blocksize_index;
  struct i_metric_s *blocksize;
  struct i_metric_s *size;
  struct i_metric_s *mode;
  struct i_metric_s *status;
  struct i_metric_s *state;
  struct i_metric_s *drive_count;
  struct i_metric_s *online_count;
  struct i_metric_s *spare_count;
  struct i_metric_s *failed_count;

} v_ldrive_item;

/* ldrive.c */
struct i_container_s* v_ldrive_cnt ();
int v_ldrive_enable (i_resource *self);
int v_ldrive_disable (i_resource *self);

/* ldrive_item.c */
v_ldrive_item* v_ldrive_item_create ();
void v_ldrive_item_free (void *itemptr);

/* ldrive_objfact.c */
int v_ldrive_objfact_fab ();
int v_ldrive_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_ldrive_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* ldrive_cntform.c */
int v_ldrive_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* ldrive_objform.c */
int v_ldrive_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_ldrive_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

