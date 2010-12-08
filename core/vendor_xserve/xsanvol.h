typedef struct v_xsanvol_item_s
{
  struct i_object_s *obj;
  int index;

  /* Config */
  struct i_metric_s *allocation_strategy;
  struct i_metric_s *buffer_cache_size;
  struct i_metric_s *debug;
  struct i_metric_s *enforce_acls;
  struct i_metric_s *file_locks;
  struct i_metric_s *force_stripe_alignment;
  struct i_metric_s *fs_block_size;
  struct i_metric_s *global_super_user;
  struct i_metric_s *inode_cache_size;
  struct i_metric_s *inode_expand_inc;
  struct i_metric_s *inode_expand_max;
  struct i_metric_s *inode_expand_min;
  struct i_metric_s *journal_size;
  struct i_metric_s *max_connections;
  struct i_metric_s *max_logsize;
  struct i_metric_s *quotas;
  struct i_metric_s *threadpool_size;
  struct i_metric_s *unix_fab_window;
  struct i_metric_s *windows_security;

  /* State */
  struct i_metric_s *bandwidth_management;
  struct i_metric_s *creation_time;
  struct i_metric_s *global_superuser;
  struct i_metric_s *license_valid;
  struct i_metric_s *msgbuf_size;
  struct i_metric_s *connected_clients;
  struct i_metric_s *connused_pc;
  struct i_metric_s *devices;
  struct i_metric_s *stripe_groups;
  struct i_metric_s *port;
  struct i_metric_s *priority;
  struct i_metric_s *pid;
  struct i_metric_s *qos;
  struct i_metric_s *bytes_free;
  struct i_metric_s *state;
  struct i_metric_s *bytes_total;
  struct i_metric_s *bytes_used;
  struct i_metric_s *used_pc;

  /* Host (i.e Active MDC) */
  struct i_metric_s *host;
  struct i_metric_s *host_ip;
  int host_flag;
  struct i_metric_s *host_state;    /* 0=Unknown 1=NotMDC 2=ActiveMDC */
  
  /* Storage Pool Container */
  struct i_container_s *sp_cnt;
  struct l_snmp_objfact_s *sp_objfact;

  /* Affinities Container */
  struct i_container_s *affinity_cnt;
  struct l_snmp_objfact_s *affinity_objfact;

  /* LUNs Container */
  struct i_container_s *lun_cnt;
  struct l_snmp_objfact_s *lun_objfact;

  /* Raw volume name */
  char *raw_volume_name;
  
} v_xsanvol_item;

struct i_container_s* v_xsanvol_cnt ();
v_xsanvol_item* v_xsanvol_get (char *desc_str);
int v_xsanvol_enable (i_resource *self);
v_xsanvol_item* v_xsanvol_create (i_resource *self, char *name_str);
int v_xsanvol_disable (i_resource *self);

int v_xsanvol_refresh_data (i_resource *self, struct i_metric_s *met, int opcode);
int v_xsanvol_refresh_sockconncb (i_resource *self, struct i_socket_s *sock, void *passdata);
int v_xsanvol_read_preempt_callback (i_resource *self, struct i_socket_s *sock, void *passdata);

v_xsanvol_item* v_xsanvol_item_create ();
void v_xsanvol_item_free (void *itemptr);

int v_xsanvol_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int v_xsanvol_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_xsanvol_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
