typedef struct v_xsanspdetail_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;

  /* Config */
  struct i_metric_s *nodediskdevice;
  struct i_metric_s *noderotationindex;
  struct i_metric_s *sectors;
  struct i_metric_s *sectorsize;

} v_xsanspdetail_item;

struct i_container_s* v_xsanspdetail_get_cnt (char *vol_name, char *sp_name);
v_xsanspdetail_item* v_xsanspdetail_get_node (struct i_container_s *cnt, char *node_name);
int v_xsanspdetail_enable (i_resource *self);
struct i_container_s* v_xsanspdetail_create_cnt (i_resource *self, char *vol_name, char *sp_name);
v_xsanspdetail_item* v_xsanspdetail_create_node (i_resource *self, struct i_container_s *cnt, char *disk_name);
int v_xsanspdetail_disable (i_resource *self);

v_xsanspdetail_item* v_xsanspdetail_item_create ();
void v_xsanspdetail_item_free (void *itemptr);

