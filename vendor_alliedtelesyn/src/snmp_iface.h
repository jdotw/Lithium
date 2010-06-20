/* snmp_iface.c */
struct i_container_s* v_snmp_iface_cnt ();
struct l_snmp_objfact_s* v_snmp_iface_objfact ();
int v_snmp_iface_enable (i_resource *self);
int v_snmp_iface_disable (i_resource *self);

/* snmp_iface_objfact.c */
int v_snmp_iface_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int v_snmp_iface_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_snmp_iface_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_iface_cntform.c */
int v_snmp_iface_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_iface_objform.c */
int v_snmp_iface_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int v_snmp_iface_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

