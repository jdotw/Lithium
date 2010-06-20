typedef struct v_voltage_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *voltage;

} v_voltage_item;

/* voltage.c */
struct i_container_s* v_voltage_cnt ();
int v_voltage_enable (i_resource *self, int enclosure, char *oidindex_str);
int v_voltage_disable (i_resource *self);

/* voltage_item.c */
v_voltage_item* v_voltage_item_create ();
void v_voltage_item_free (void *itemptr);

/* voltage_objfact.c */
int v_voltage_objfact_fab ();
int v_voltage_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_voltage_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

