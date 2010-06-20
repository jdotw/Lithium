typedef struct v_status_item_s
{
  struct i_object_s *obj;

  /* Power */
  struct i_metric_s *powerstate;

  /* Ambient temp */
  struct i_metric_s *ambient_temp;

  /* UPS */
  struct i_metric_s *batterylow;
  struct i_metric_s *batteryinuse;
  struct i_metric_s *linepowerdown;

  /* RAID */
  struct i_metric_s *raidcont_state;

  /* Monitor */
//  struct i_metric_s *powersupply;
//  struct i_metric_s *raidset;
  struct i_metric_s *drivetemp;
  struct i_metric_s *blowertemp;
  struct i_metric_s *raidtemp;
  struct i_metric_s *blowerspeed;
  struct i_metric_s *emutemp;
  struct i_metric_s *smart;
  struct i_metric_s *blower;
  struct i_metric_s *raidcomms;
  struct i_metric_s *ambientstate;
  struct i_metric_s *battery;

  /* Enclosure */
  struct i_metric_s *serviceid;
  struct i_metric_s *buzzer;
  struct i_metric_s *xsyncstate;

} v_status_item;

/* status.c */
struct i_container_s* v_status_cnt ();
v_status_item* v_status_static_top_item ();
v_status_item* v_status_static_bottom_item ();
int v_status_enable (i_resource *self);
int v_status_disable (i_resource *self);

/* status_item.c */
v_status_item* v_status_item_create ();
void v_status_item_free (void *itemptr);

/* status_cntform.c */
int v_status_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* status_objform.c */
int v_status_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_status_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

