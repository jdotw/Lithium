typedef struct v_drives_item_s
{
  struct i_object_s *obj;

  /* SMART */
  struct i_metric_s *smart_state;
  struct i_metric_s *smart_status;

  /* Drive Info */
  struct i_metric_s *vendor;
  struct i_metric_s *firmware;
  struct i_metric_s *sectorcapacity;
  struct i_metric_s *capacity;
  struct i_metric_s *badblockcount;
  struct i_metric_s *remapcount;

  /* RAID Status */
  struct i_metric_s *online;
  struct i_metric_s *arraymember;
  struct i_metric_s *rebuilding;
  struct i_metric_s *brokenraidmember;
  struct i_metric_s *diskcache_enabled;

  /* Array membership */
  struct i_metric_s *arraynumber;

} v_drives_item;

/* drives.c */
struct i_container_s* v_drives_cnt ();
v_drives_item* v_drives_1 ();
v_drives_item* v_drives_2 ();
v_drives_item* v_drives_3 ();
v_drives_item* v_drives_4 ();
v_drives_item* v_drives_5 ();
v_drives_item* v_drives_6 ();
v_drives_item* v_drives_7 ();
v_drives_item* v_drives_8 ();
v_drives_item* v_drives_9 ();
v_drives_item* v_drives_10 ();
v_drives_item* v_drives_11 ();
v_drives_item* v_drives_12 ();
v_drives_item* v_drives_13 ();
v_drives_item* v_drives_14 ();
v_drives_item* v_drives_x (int x);
int v_drives_enable (i_resource *self);
int v_drives_disable (i_resource *self);

/* drives_item.c */
v_drives_item* v_drives_item_create ();
void v_drives_item_free (void *itemptr);

/* drives_cntform.c */
int v_drives_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* drive_objform.c */
int v_drives_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

