/* rsvr.c */

typedef struct v_rsvr_s
{
  /* Config */
  struct in_addr ip_addr;
  char *name_str;
  i_list *port_list;

  /* Status */
  long admin_state;
  long op_state;
  
  /* Stats */
  long max_conn;
  long weight;
  unsigned long rx_pkts;
  unsigned long tx_pkts;
  long cur_conn;
  unsigned long total_conn;
  long agedout;
  long reassign;
  long reassign_limit;
  long failedportexists;
  long failtime;
  long peak_conn;
  long reverse_conn;
  long cur_sess; 

  /* Currency */
  int admin_state_current;
  int op_state_current;

  /* Reqids */
  int admin_state_reqid;
  int op_state_reqid;
} v_rsvr;

int v_rsvr_enable (i_resource *self);
int v_rsvr_disable (i_resource *self);
v_rsvr* v_rsvr_create ();
void v_rsvr_free (void *rsvrptr);

/* rsvr_port.c */

typedef struct v_rsvr_port_st
{
  /* Config */
  unsigned long port;
  struct v_rsvr_s *rsvr;

  /* Status */
  long admin_state;
  long op_state;

  /* Status */
  long reassign;
  long failtime;
  long cur_conn;
  unsigned long total_conn;
  unsigned long rx_pkts;
  unsigned long tx_pkts;
  unsigned long rx_bytes;
  unsigned long tx_bytes;
  long peak_conn;

  /* Currency */
  int admin_state_current;
  int op_state_current;

  /* Reqids */
  int admin_state_reqid;
  int op_state_reqid;

  /* Refresh info */
  l_snmp_session *snmp_session;
  int refresh_inprogress;
  int refresh_progress;
} v_rsvr_port;

v_rsvr_port* v_rsvr_port_create ();
void v_rsvr_port_free (void *portptr);

/* rsvr_list.c */

i_list* v_rsvr_list ();
int v_rsvr_list_populate (i_resource *self);
int v_rsvr_list_populate_serverwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata);
int v_rsvr_list_populate_portwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata);
int v_rsvr_list_populate_terminate (i_resource *self);
int v_rsvr_list_populate_finished (i_resource *self);
int v_rsvr_list_populate_failed (i_resource *self);

/* rsvr_refresh.c */

int v_rsvr_refresh_enable (i_resource *self);
int v_rsvr_refresh_disable (i_resource *self);
int v_rsvr_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata);
int v_rsvr_refresh_next (i_resource *self);
int v_rsvr_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);
int v_rsvr_refresh_portsnmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);
int v_rsvr_refresh_terminate (i_resource *self);
int v_rsvr_refresh_finished (i_resource *self);
int v_rsvr_refresh_failed (i_resource *self, char *str);

/* rsvr_formsection.c */

int v_rsvr_formsection (i_resource *self, i_form *form, void *passdata, int passdatasize, i_authentication *auth);
