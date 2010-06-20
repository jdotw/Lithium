typedef struct v_raidinfo_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *descr;
  struct i_metric_s *alias;
  struct i_metric_s *wwn;

  struct i_metric_s *redundancy;
  struct i_metric_s *controllers;
  struct i_metric_s *interconnect;
  struct i_metric_s *enclosures;

  struct i_metric_s *vendor;
  struct i_metric_s *model;
  struct i_metric_s *serial;
  struct i_metric_s *revision;
  struct i_metric_s *manufactured;
  
} v_raidinfo_item;

/* raidinfo.c */
struct i_container_s* v_raidinfo_cnt ();
int v_raidinfo_enable (i_resource *self);
int v_raidinfo_disable (i_resource *self);

/* raidinfo_item.c */
v_raidinfo_item* v_raidinfo_item_create ();
void v_raidinfo_item_free (void *itemptr);


