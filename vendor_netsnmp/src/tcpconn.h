typedef struct l_snmp_nstcpconn_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *state;
  struct i_metric_s *local_ip;
  struct i_metric_s *local_port;
  struct i_metric_s *remote_ip;
  struct i_metric_s *remote_port;

} l_snmp_nstcpconn_item;

/* tcpconn.c */
struct i_container_s* l_snmp_nstcpconn_cnt ();
int l_snmp_nstcpconn_enable (i_resource *self);
int l_snmp_nstcpconn_disable (i_resource *self);

/* tcpconn_item.c */
l_snmp_nstcpconn_item* l_snmp_nstcpconn_item_create ();
void l_snmp_nstcpconn_item_free (void *itemptr);

/* tcpconn_objfact.c */
int l_snmp_nstcpconn_objfact_fab ();
int l_snmp_nstcpconn_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_nstcpconn_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* tcpconn_cntform.c */
int l_snmp_nstcpconn_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* tcpconn_objform.c */
int l_snmp_nstcpconn_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int l_snmp_nstcpconn_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

