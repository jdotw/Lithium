/* snmp_iface.c */
struct i_container_s* l_snmp_iface_cnt ();
struct l_snmp_objfact_s* l_snmp_iface_objfact ();
int l_snmp_iface_enable (i_resource *self);
int l_snmp_iface_disable (i_resource *self);

/* snmp_iface_objfunc.c */
int l_snmp_iface_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_iface_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_iface_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_iface_cntform.c */
int l_snmp_iface_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* snmp_iface_objform.c */
int l_snmp_iface_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_snmp_iface_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_snmp_iface_objform_histvalcb (i_resource *self, struct i_metric_s *met, struct i_list_s *val_list, void *passdata);

/* snmp_iface_refcb.c */
int l_snmp_iface_adminstate_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int l_snmp_iface_cnt_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);

