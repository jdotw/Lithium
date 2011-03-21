typedef struct v_data_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *sysinfo;
  struct i_metric_s *powerstate;
  struct i_metric_s *ambient_top;
  struct i_metric_s *ambient_bottom;
  struct i_metric_s *status_top;
  struct i_metric_s *status_bottom;
  struct i_metric_s *drives_top;
  struct i_metric_s *drives_bottom;
  struct i_metric_s *arrays_top;
  struct i_metric_s *arrays_bottom;
  struct i_metric_s *hostifaces_top;
  struct i_metric_s *hostifaces_bottom;
  struct i_metric_s *power_left;
  struct i_metric_s *power_right;
  struct i_metric_s *blower_top;
  struct i_metric_s *blower_bottom;
  struct i_metric_s *battery_left;
  struct i_metric_s *battery_right;

} v_data_item;

struct i_container_s* v_data_cnt ();
struct v_data_item_s* v_data_static_item ();
int v_data_enable (i_resource *self);
int v_data_disable (i_resource *self);

v_data_item* v_data_item_create ();
void v_data_item_free (void *itemptr);

/* data_sysinfo.c */
int v_data_sysinfo_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_sysinfo_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

/* data_powerstate.c */
int v_data_powerstate_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_powerstate_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

/* data_ambient.c */
int v_data_ambient_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_ambient_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

/* data_status.c */
int v_data_status_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_status_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);
int v_data_status_process_ups ();
int v_data_status_process_smart ();
int v_data_status_process_raidcont ();
int v_data_status_process_monitor ();
int v_data_status_process_enclosure ();

/* data_drives.c */
int v_data_drives_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_drives_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);
int v_data_drives_process_slot ();

/* data_arrays.c */
int v_data_arrays_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_arrays_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);
int v_data_arrays_process_arrays ();
int v_data_arrays_process_slices ();
int v_data_arrays_process_progress ();
int v_data_arrays_process_members ();
int v_data_arrays_process_slots ();

/* data_hostifaces.c */
int v_data_hostifaces_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_hostifaces_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);
int v_data_hostifaces_process_hostifaces ();
int v_data_hostifaces_process_luns ();

/* data_power.c */
int v_data_power_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_power_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

/* data_blower.c */
int v_data_blower_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_blower_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

/* data_battery.c */
int v_data_battery_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_data_battery_plistcb (i_resource *self, struct v_plist_req_s *req, void *passdata);

