typedef struct l_snmp_route_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *ifindex;
  struct i_metric_s *metric1;
  struct i_metric_s *metric2;
  struct i_metric_s *metric3;
  struct i_metric_s *metric4;
  struct i_metric_s *metric5;

  struct i_metric_s *mask;
  struct i_metric_s *nexthop;
  struct i_metric_s *type;
  struct i_metric_s *protocol;
  struct i_metric_s *age;

  struct i_interface_s *iface;
  
} l_snmp_route_item;

/* snmp_route.c */
struct i_container_s* l_snmp_route_cnt ();
int l_snmp_route_enable (i_resource *self);
int l_snmp_route_disable (i_resource *self);

/* snmp_route_item.c */
l_snmp_route_item* l_snmp_route_item_create ();
void l_snmp_route_item_free (void *itemptr); 

/* snmp_route_objfunc.c */
int l_snmp_route_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_route_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_route_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_route_cntform.c */
int l_snmp_route_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_route_objform.c */
int l_snmp_route_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_route_ifacematch.c */
int l_snmp_route_ifacematch (i_resource *self, struct i_container_s *route_cnt, void *passdata);
