/* RAID Status Item */

typedef struct l_xsnmp_raidstatus_item_s
{
  /* id variables */
  struct i_object_s *obj;

  /* raid variables */
  struct i_metric_s *general_status;
  struct i_metric_s *general_status_message;
  struct i_metric_s *battery_status;
  struct i_metric_s *battery_status_message;
  struct i_metric_s *hw_version;
  struct i_metric_s *fw_version;
  struct i_metric_s *write_cache;

} l_xsnmp_raidstatus_item;

/* raid.c */
struct i_container_s* l_xsnmp_raidstatus_cnt ();
int l_xsnmp_raidstatus_enable (i_resource *self);

