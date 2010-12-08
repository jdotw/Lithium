typedef struct v_xsancomputer_item_s
{
  struct i_object_s *obj;

  /* Xsan only */
  struct i_metric_s *cfsversion;
  struct i_metric_s *sfsversion;
  struct i_metric_s *ip;
  struct i_metric_s *name;
  struct i_metric_s *hostname;
  struct i_metric_s *rendezname;
  struct i_metric_s *failoverpriority;
  struct i_metric_s *fsmpmrunning;
  struct i_metric_s *fsnameservers;
  struct i_metric_s *cpucount;
  struct i_metric_s *cpuspeed;
  struct i_metric_s *cpukind;
  struct i_metric_s *ram;
  struct i_metric_s *licensevalid;
  struct i_metric_s *role;

} v_xsancomputer_item;

struct i_container_s* v_xsancomputer_cnt ();
v_xsancomputer_item* v_xsancomputer_get (char *desc_str);
int v_xsancomputer_enable (i_resource *self);
v_xsancomputer_item* v_xsancomputer_create (i_resource *self, char *name_str);
int v_xsancomputer_disable (i_resource *self);

int v_xsancomputer_refresh_data (i_resource *self, struct i_metric_s *met, int opcode);
int v_xsancomputer_refresh_sockconncb (i_resource *self, struct i_socket_s *sock, void *passdata);
int v_xsancomputer_read_preempt_callback (i_resource *self, struct i_socket_s *sock, void *passdata);

v_xsancomputer_item* v_xsancomputer_item_create ();
void v_xsancomputer_item_free (void *itemptr);

