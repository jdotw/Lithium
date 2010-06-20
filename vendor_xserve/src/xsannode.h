typedef struct v_xsannode_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;
  struct v_xsansp_item_s *sp_item;

  /* Config */
  struct i_metric_s *noderotationindex;
  struct i_metric_s *blockdev;
  struct i_metric_s *hostnumber;
  struct i_metric_s *hostinquiry;
  struct i_metric_s *lun;
  struct i_metric_s *maxsectors;
  struct i_metric_s *sectorsize;
  struct i_metric_s *wwn;
  struct i_metric_s *size;
  struct i_metric_s *unlabeled;

  int lun_registered;

} v_xsannode_item;

v_xsannode_item* v_xsannode_get (struct i_container_s *node_cnt, char *node_name);
v_xsannode_item* v_xsannode_get_global (char *node_name);
v_xsannode_item* v_xsannode_create (i_resource *self, struct v_xsansp_item_s *sp, char *node_name);

v_xsannode_item* v_xsannode_item_create ();
void v_xsannode_item_free (void *itemptr);

