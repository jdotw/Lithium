typedef struct v_xsanvisdisk_item_s
{
  struct i_object_s *obj;

  /* WWN */
  struct i_metric_s *wwn;

  /* Raw visdisk name */
  char *raw_visdisk_name;

} v_xsanvisdisk_item;

struct i_container_s* v_xsanvisdisk_cnt ();
v_xsanvisdisk_item* v_xsanvisdisk_get (char *desc_str);
int v_xsanvisdisk_enable (i_resource *self);
v_xsanvisdisk_item* v_xsanvisdisk_create (i_resource *self, char *name_str);
int v_xsanvisdisk_disable (i_resource *self);

v_xsanvisdisk_item* v_xsanvisdisk_item_create ();
void v_xsanvisdisk_item_free (void *itemptr);

