typedef struct v_sysinfo_item_s
{
  struct i_object_s *obj;

  /* Standard */
  struct i_metric_s *computername;
  struct i_metric_s *rendname;
  struct i_metric_s *hostname;
  struct i_metric_s *boottime;
  struct i_metric_s *version;
  struct i_metric_s *serialvalid;
  struct i_metric_s *serialuserlimit;

  /* Xserve - Common */
  struct i_metric_s *cpu_count;
  struct i_metric_s *cpu_speed;

  /* Xserve - PPC */
  struct i_metric_s *bootrom;
  struct i_metric_s *cpu_l2cache;
  struct i_metric_s *ram_size;

  /* Xserve - Intel */
  struct i_metric_s *cpu_type;

} v_sysinfo_item;

/* sysinfo.c */
struct i_container_s* v_sysinfo_cnt ();
struct v_sysinfo_item_s* v_sysinfo_static_item ();
int v_sysinfo_enable (i_resource *self);
int v_sysinfo_disable (i_resource *self);

/* sysinfo_item.c */
v_sysinfo_item* v_sysinfo_item_create ();
void v_sysinfo_item_free (void *itemptr);

/* sysinfo_cntform.c */
int v_sysinfo_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* sysinfo_objform.c */
int v_sysinfo_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);



