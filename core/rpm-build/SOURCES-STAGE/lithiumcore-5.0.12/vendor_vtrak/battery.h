typedef struct v_battery_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *opstate;

  struct i_metric_s *manufactured;
  struct i_metric_s *chemistry;
  struct i_metric_s *celltype;

  struct i_metric_s *temp;
  struct i_metric_s *temp_charge_max;
  struct i_metric_s *temp_discharge_max;

  struct i_metric_s *cyclecount;
  struct i_metric_s *remaining;
  struct i_metric_s *holdtime;

  struct i_metric_s *voltage;
  struct i_metric_s *current;

} v_battery_item;

/* battery.c */
struct i_container_s* v_battery_cnt ();
int v_battery_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_battery_disable (i_resource *self);

/* battery_item.c */
v_battery_item* v_battery_item_create ();
void v_battery_item_free (void *itemptr);

/* battery_objfact.c */
int v_battery_objfact_fab ();
int v_battery_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_battery_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
