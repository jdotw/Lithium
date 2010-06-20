typedef struct v_arrays_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;

  /* Config */
  struct i_metric_s *stripesize;
  struct i_metric_s *raidlevel;
  struct i_metric_s *membercount;
  struct i_metric_s *raidtype;
  struct i_metric_s *raidstatus;
  struct i_metric_s *sectorcapacity;
  struct i_metric_s *capacity;

  /* Slices */
  struct i_metric_s *slice1_size;
  struct i_metric_s *slice2_size;
  struct i_metric_s *slice3_size;
  struct i_metric_s *slice4_size;
  struct i_metric_s *slice5_size;
  struct i_metric_s *slice6_size;
  struct i_metric_s *slice7_size;
  struct i_metric_s *slice8_size;

  /* Progress */
  struct i_metric_s *initialize;
  struct i_metric_s *addmember;
  struct i_metric_s *verify;
  struct i_metric_s *expand;

  /* Members */
  struct i_metric_s *member1_slot;
  struct i_metric_s *member2_slot;
  struct i_metric_s *member3_slot;
  struct i_metric_s *member4_slot;
  struct i_metric_s *member5_slot;
  struct i_metric_s *member6_slot;
  struct i_metric_s *member7_slot;
  /*
  struct i_metric_s *member8slot;
  struct i_metric_s *member9slot;
  struct i_metric_s *member10slot;
  struct i_metric_s *member11slot;
  struct i_metric_s *member12slot;
  struct i_metric_s *member13slot;
  struct i_metric_s *member14slot;
  struct i_metric_s *member15slot;
  struct i_metric_s *member16slot;
  struct i_metric_s *member17slot;
  struct i_metric_s *member18slot;
  struct i_metric_s *member19slot;
  struct i_metric_s *member20slot;
  struct i_metric_s *member21slot;
  struct i_metric_s *member22slot;
  struct i_metric_s *member23slot;
  struct i_metric_s *member24slot;
  struct i_metric_s *member25slot;
  struct i_metric_s *member26slot;
  struct i_metric_s *member27slot;
  struct i_metric_s *member28slot;
  struct i_metric_s *member29slot;
  struct i_metric_s *member30slot;
  struct i_metric_s *member31slot;
  struct i_metric_s *member32slot;
  */

  int lun_registered;
  
} v_arrays_item;

/* arrays.c */
v_arrays_item* v_arrays_get_item (int controller, int id);
int v_arrays_enable (i_resource *self);
v_arrays_item* v_arrays_create (i_resource *self, int controller);
int v_arrays_disable (i_resource *self);

/* arrays_item.c */
v_arrays_item* v_arrays_item_create ();
void v_arrays_item_free (void *itemptr);

/* arrays_cntform.c */
int v_arrays_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* arrays_objform.c */
int v_arrays_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

