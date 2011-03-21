/* vsvr.c */

typedef struct v_vsvr_s
{
  /* Config */
  struct in_addr ip_addr;
  char *name_str;
  i_list *port_list;

  /* Status */
  long admin_state;
  
  /* Stats */
  long sdatype;
  unsigned long rx_pkts;
  unsigned long tx_pkts;
  unsigned long rx_bytes;
  unsigned long tx_bytes;
  unsigned long total_conn;
  long sym_priority;
  long sym_state;
  long sym_keepfails;
  long sym_activates;
  long sym_deactivates;

  /* Currency */
  int admin_state_current;

  /* Reqids */
  int admin_state_reqid;
} v_vsvr;

int v_vsvr_enable (i_resource *self);
int v_vsvr_disable (i_resource *self);
v_vsvr* v_vsvr_create ();
void v_vsvr_free (void *vsvrptr);

/* vsvr_port.c */

typedef struct v_vsvr_port_s
{
  /* Config */
  unsigned long port;
  struct v_vsvr_s *vsvr;

  /* Status */
  long admin_state;

  /* Status */
  long cur_conn;
  unsigned long total_conn;
  long peak_conn;

  /* Currency */
  int admin_state_current;

  /* Reqids */
  int admin_state_reqid;

  /* Refresh info */
  int refresh_inprogress;
  int refresh_progress;
} v_vsvr_port;

v_vsvr_port* v_vsvr_port_create ();
void v_vsvr_port_free (void *portptr);

/* vsvr_list.c */

i_list* v_vsvr_list ();
int v_vsvr_list_populate (i_resource *self);
int v_vsvr_list_populate_serverwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata);
int v_vsvr_list_populate_portwalk_callback (i_resource *self, l_snmp_session *session, struct snmp_pdu *pdu, void *passdata);
int v_vsvr_list_populate_terminate (i_resource *self);
int v_vsvr_list_populate_finished (i_resource *self);
int v_vsvr_list_populate_failed (i_resource *self);

/* vsvr_refresh.c */

int v_vsvr_refresh_enable (i_resource *self);
int v_vsvr_refresh_disable (i_resource *self);
int v_vsvr_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata);
int v_vsvr_refresh_next (i_resource *self);
int v_vsvr_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);
int v_vsvr_refresh_portsnmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);
int v_vsvr_refresh_terminate (i_resource *self);
int v_vsvr_refresh_finished (i_resource *self);
int v_vsvr_refresh_failed (i_resource *self, char *str);

/* vsvr_formsection.c */

int v_vsvr_formsection (i_resource *self, i_form *form, void *passdata, int passdatasize, i_authentication *auth);
