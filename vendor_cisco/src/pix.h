/* pix_iflist.c */

int v_pix_iflist_refresh_enable (i_resource *self);
int v_pix_iflist_refresh_disable (i_resource *self);
int v_pix_iflist_refresh_timer_callback (i_resource *self, i_timer *timer, void *passdata);
int v_pix_iflist_refresh_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);

/* pix_iflist_probe.c */

int v_pix_iflist_probe (i_resource *self);
int v_pix_iflist_probe_snmp_callback (i_resource *self, l_snmp_session *session, int reqid, struct snmp_pdu *pdu, void *passdata);

