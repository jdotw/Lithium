typedef struct v_drives_item_s
{
  struct i_object_s *obj;

  /* Xserve */
  struct i_metric_s *capacity;
  struct i_metric_s *interconnect;
  struct i_metric_s *diskid;
  struct i_metric_s *location;
  struct i_metric_s *vendor;
  struct i_metric_s *model;
  struct i_metric_s *bytes_read;
  struct i_metric_s *read_bps;
  struct i_metric_s *bytes_written;
  struct i_metric_s *write_bps;
  struct i_metric_s *read_errors;
  struct i_metric_s *read_eps;
  struct i_metric_s *write_errors;
  struct i_metric_s *write_eps;
  struct i_metric_s *smart_desc;
  struct i_metric_s *smart;
  struct i_metric_s *raid_status;
  struct i_metric_s *raid_desc;

  struct i_metric_cgraph_s *tput_cg;

} v_drives_item;

/* drives.c */
struct i_container_s* v_drives_cnt ();
v_drives_item* v_drives_get (char *desc_str);
int v_drives_enable (i_resource *self);
v_drives_item* v_drives_create (i_resource *self, char *desc_str);
int v_drives_disable (i_resource *self);

/* drives_item.c */
v_drives_item* v_drives_item_create ();
void v_drives_item_free (void *itemptr);

/* drives_cntform.c */
int v_drives_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* drives_objform.c */
int v_drives_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_drives_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* drives_smart.c */
int v_drives_smart_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

