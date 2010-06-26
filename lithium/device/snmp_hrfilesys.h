/* snmp_hrfilesys.c */
int l_snmp_hrfilesys_enable (i_resource *self);
int l_snmp_hrfilesys_enabled();

/* snmp_hrfilesys_objfact.c */

int l_snmp_hrfilesys_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_snmp_hrfilesys_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_snmp_hrfilesys_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* snmp_hrfilesys_refcb.c */
int l_snmp_hrfilesys_access_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
