typedef struct v_hostifaces_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;

  /* Config */
  struct i_metric_s *linkstate;
  struct i_metric_s *hardloopid;
  struct i_metric_s *topology;
  struct i_metric_s *actual_topology;
  struct i_metric_s *speed;
  struct i_metric_s *hardloopid_enabled;
  struct i_metric_s *type;
  struct i_metric_s *max_speed;
  struct i_metric_s *actual_speed;
  struct i_metric_s *wwn;

  /* LUN */
  struct i_metric_s *lun0_array;
  struct i_metric_s *lun0_slice;
  struct i_metric_s *lun1_array;
  struct i_metric_s *lun1_slice;
  struct i_metric_s *lun2_array;
  struct i_metric_s *lun2_slice;
  struct i_metric_s *lun3_array;
  struct i_metric_s *lun3_slice;
  struct i_metric_s *lun4_array;
  struct i_metric_s *lun4_slice;
  struct i_metric_s *lun5_array;
  struct i_metric_s *lun5_slice;
  struct i_metric_s *lun6_array;
  struct i_metric_s *lun6_slice;
  struct i_metric_s *lun7_array;
  struct i_metric_s *lun7_slice;

  /* Errors */
  struct i_metric_s *linkfailure_count;
  struct i_metric_s *syncloss_count;
  struct i_metric_s *signalloss_count;
  struct i_metric_s *protocolerror_count;
  struct i_metric_s *invalidword_count;
  struct i_metric_s *invalidcrc_count;

} v_hostifaces_item;

/* hostifaces.c */
v_hostifaces_item* v_hostifaces_get_item (int controller, int id);
int v_hostifaces_enable (i_resource *self);
v_hostifaces_item* v_hostifaces_create (i_resource *self, int controller);
int v_hostifaces_disable (i_resource *self);

/* hostifaces_item.c */
v_hostifaces_item* v_hostifaces_item_create ();
void v_hostifaces_item_free (void *itemptr);

/* hostifaces_cntform.c */
int v_hostifaces_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* hostifaces_objform.c */
int v_hostifaces_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

