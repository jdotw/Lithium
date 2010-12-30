typedef struct v_port_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  int disabled_metrics;

  /* Status */
  struct i_metric_s *phystate;
  struct i_metric_s *opstate;
  struct i_metric_s *adminstate;
  struct i_metric_s *linkstate;

  /* Port info */
  struct i_metric_s *txtype;
  struct i_metric_s *wwn;
  struct i_metric_s *speed_enum;
  struct i_metric_s *speed;         // Set by a refcb off speed_enum
  struct i_metric_s *type;

  /* Stats */
  struct i_metric_s *rx_byte_count;    // Words with alloc_unit of 4
  struct i_metric_s *tx_byte_count;     // Words with alloc_unit of 4
  struct i_metric_s *rx_bps;
  struct i_metric_s *tx_bps;
  struct i_metric_s *rx_utilpc;
  struct i_metric_s *tx_utilpc;

  /* Errors */
  struct i_metric_s *too_many_rdy_count;
  struct i_metric_s *too_many_rdy_rate;
  struct i_metric_s *no_tx_credit_count;
  struct i_metric_s *no_tx_credit_rate;
  struct i_metric_s *rx_inenc_err_count;
  struct i_metric_s *rx_inenc_err_rate;
  struct i_metric_s *rx_crc_count;
  struct i_metric_s *rx_crc_rate;
  struct i_metric_s *rx_trunc_count;
  struct i_metric_s *rx_trunc_rate;
  struct i_metric_s *rx_too_long_count;
  struct i_metric_s *rx_too_long_rate;
  struct i_metric_s *rx_bad_eof_count;
  struct i_metric_s *rx_bad_eof_rate;
  struct i_metric_s *rx_outenc_err_count;
  struct i_metric_s *rx_outenc_err_rate;
  struct i_metric_s *rx_bad_os_count;
  struct i_metric_s *rx_bad_os_rate;
  struct i_metric_s *rx_lip_count;
  struct i_metric_s *rx_lip_rate;
  struct i_metric_s *tx_lip_count;
  struct i_metric_s *tx_lip_rate;

  /* Aggregated Errors (Calculated) */
  struct i_metric_s *error_count;
  struct i_metric_s *error_rate;

} v_port_item;

/* port.c */
struct i_container_s* v_port_cnt ();
int v_port_enable (i_resource *self);

/* port_item.c */
v_port_item* v_port_item_create ();
void v_port_item_free (void *itemptr);

/* port_objfact.c */
int v_port_objfact_fab ();
int v_port_objfact_fab_port ();
int v_port_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_port_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* port_refcb.c */
int v_port_speed_enum_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int v_port_adminstate_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
