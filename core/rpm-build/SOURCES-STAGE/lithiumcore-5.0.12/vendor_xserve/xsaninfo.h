typedef struct v_xsaninfo_item_s
{
  struct i_object_s *obj;

  /* Info */
  struct i_metric_s *cpu_usage;
  struct i_metric_s *network_tput;
  struct i_metric_s *fibre_tput;

  /* Times */
  time_t last_cpu_t;
  time_t last_network_t;
  time_t last_fibre_t;

} v_xsaninfo_item;

struct i_container_s* v_xsaninfo_cnt ();
struct i_object_s* v_xsaninfo_obj ();
int v_xsaninfo_enable (i_resource *self);
int v_xsaninfo_enable_history (i_resource *self);

v_xsaninfo_item* v_xsaninfo_item_create ();
void v_xsaninfo_item_free (void *itemptr);

