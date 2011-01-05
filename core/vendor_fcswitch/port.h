typedef struct v_port_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  int disabled_metrics;

  /* Port info */
  struct i_metric_s *speed;
  struct i_metric_s *speed_kbyte;
  struct i_metric_s *state;
  struct i_metric_s *hwstate;
  struct i_metric_s *status;
  struct i_metric_s *wwn;
  struct i_metric_s *alias;
  struct i_metric_s *txtype;
  struct i_metric_s *modtype;

  /* Throughput Stats */
  struct i_metric_s *octets_out;
  struct i_metric_s *octets_in;
  struct i_metric_s *bps_out;
  struct i_metric_s *bps_in;
  struct i_metric_s *utilpc_in;
  struct i_metric_s *utilpc_out;

  /* Error Stats */
  struct i_metric_s *rx_link_reset_count;
  struct i_metric_s *rx_link_reset_rate;
  struct i_metric_s *tx_link_reset_count;
  struct i_metric_s *tx_link_reset_rate;
  struct i_metric_s *rx_ols_count;
  struct i_metric_s *rx_ols_rate;
  struct i_metric_s *tx_ols_count;
  struct i_metric_s *tx_ols_rate;
  struct i_metric_s *link_failure_count;
  struct i_metric_s *link_failure_rate;
  struct i_metric_s *invalid_crc_count;
  struct i_metric_s *invalid_crc_rate;
  struct i_metric_s *invalid_word_count;
  struct i_metric_s *invalid_word_rate;
  struct i_metric_s *sequence_error_count;
  struct i_metric_s *sequence_error_rate;
  struct i_metric_s *lost_signal_count;
  struct i_metric_s *lost_signal_rate;
  struct i_metric_s *lost_sync_count;
  struct i_metric_s *lost_sync_rate;
  struct i_metric_s *invalid_os_count;
  struct i_metric_s *invalid_os_rate;
  struct i_metric_s *frame_too_long_count;
  struct i_metric_s *frame_too_long_rate;
  struct i_metric_s *frame_trunc_count;
  struct i_metric_s *frame_trunc_rate;
  struct i_metric_s *addr_error_count;
  struct i_metric_s *addr_error_rate;
  struct i_metric_s *delim_error_count;
  struct i_metric_s *delim_error_rate;
  struct i_metric_s *encoding_error_count;
  struct i_metric_s *encoding_error_rate;

  /* Generic Error Stat */
  struct i_metric_s *errors;
  struct i_metric_s *eps;

  struct i_metric_cgraph_s *tput_cg;

} v_port_item;

/* port.c */
struct i_container_s* v_port_enable (i_resource *self, struct v_unit_item_s *unit);
v_port_item* v_port_item_create ();
void v_port_item_free (void *itemptr);

/* port_objfact.c */
int v_port_objfact_fab ();
int v_port_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_port_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* port_refcb.c */
int v_port_speed_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int v_port_adminstate_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int v_port_status_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

