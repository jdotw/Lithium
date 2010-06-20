typedef struct v_xsansp_item_s
{
  struct i_object_s *obj;

  /* Config */
  struct i_metric_s *affinity;
  struct i_metric_s *exclusive;
  struct i_metric_s *journal;
  struct i_metric_s *metadata;
  struct i_metric_s *multipathmethod;
  struct i_metric_s *read;
  struct i_metric_s *write;
  struct i_metric_s *status;
  struct i_metric_s *stripebreadth;

  /* State */
  struct i_metric_s *enabled;
  struct i_metric_s *mirrorindex;
  struct i_metric_s *nativekeyvalue;
  struct i_metric_s *devices;
  struct i_metric_s *realtime;
  struct i_metric_s *bytes_free;
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_used;
  struct i_metric_s *used_pc;

  /* Node Container */
  struct i_container_s *node_cnt;

} v_xsansp_item;

v_xsansp_item* v_xsansp_get (struct i_container_s *sp_cnt, char *desc_str);
v_xsansp_item* v_xsansp_create (i_resource *self, struct v_xsanvol_item_s *vol, char *name_str);

v_xsansp_item* v_xsansp_item_create ();
void v_xsansp_item_free (void *itemptr);

