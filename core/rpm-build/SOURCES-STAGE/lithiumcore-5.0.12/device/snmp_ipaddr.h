typedef struct l_snmp_ipaddr_s
{
  struct i_object_s *obj;
  struct i_metric_s *addr;
  struct i_metric_s *ifindex;
  struct i_metric_s *netmask;

  unsigned short netmask_set;         /* 1 = netmask registered */
  unsigned short iface_set;           /* 1 = iface registered */
  struct i_callback_s *netmask_refcb; /* Netmask refresh callback */

  struct i_interface_s *iface;       /* Matching iface */
} l_snmp_ipaddr;

/* snmp_ipaddr.c */

struct i_container_s* l_snmp_ipaddr_cnt ();
int l_snmp_ipaddr_enable (i_resource *self);
int l_snmp_ipaddr_disable (i_resource *self);
l_snmp_ipaddr* l_snmp_ipaddr_create ();
void l_snmp_ipaddr_free (void *ipaddrptr);

/* snmp_ipaddr_objfact.c */

int l_snmp_ipaddr_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_ipaddr_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_ipaddr_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_ipaddr_ifacematch.c */

int l_snmp_ipaddr_ifacematch (i_resource *self, struct i_container_s *addr_cnt, void *passdata);

/* snmp_ipaddr_cntform.c */

int l_snmp_ipaddr_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_ipaddr_objform.c */

int l_snmp_ipaddr_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* snmp_ipaddr_netmask.c */

int l_snmp_ipaddr_netmask_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

