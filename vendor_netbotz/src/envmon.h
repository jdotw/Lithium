typedef struct v_envmon_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  struct i_metric_s *temp;
  struct i_metric_s *humidity;
  struct i_metric_s *dewpoint;
  struct i_metric_s *audio;
  struct i_metric_s *airflow;
  struct i_metric_s *door;
  struct i_metric_s *motion;
  struct i_metric_s *campic_small;
  struct i_metric_s *campic_large;

} v_envmon_item;

/* envmon.c */
struct i_container_s* v_envmon_cnt ();
int v_envmon_enable (i_resource *self);
int v_envmon_disable (i_resource *self);
struct i_object_s* v_envmon_masterobj ();

/* envmon_item.c */
v_envmon_item* v_envmon_item_create ();
void v_envmon_item_free (void *itemptr);

/* envmon_cntform.c */
int v_envmon_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* envmon_objform.c */
int v_envmon_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_envmon_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* envmon_campic.c */
int v_envmon_campic_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int v_envmon_campic_sockcb (i_resource *self, struct i_socket_s *sock, void *passdata);

/* envmon_campicform.c */
int v_envmon_campicform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
