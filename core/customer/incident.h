/* incident.c */

#define DEFAULT_INCIDENT_TABLE_SIZE 200

unsigned long l_incident_count_failed ();
int l_incident_count_failed_inc ();
int l_incident_count_failed_dec ();
unsigned long l_incident_count_impaired ();
int l_incident_count_impaired_inc ();
int l_incident_count_impaired_dec ();
unsigned long l_incident_count_atrisk ();
int l_incident_count_atrisk_inc ();
int l_incident_count_atrisk_dec ();

i_list* l_incident_list ();
i_list* l_incident_uhlist ();
i_hashtable* l_incident_table ();
i_hashtable* l_incident_cleared_table ();
i_list* l_incident_cleared_list ();
int l_incident_enable (i_resource *self);
int l_incident_disable (i_resource *self);

/* incident_id.c */
long l_incident_id_assign ();
int l_incident_id_setcurrent (long currentid);

/* incident_handler.c */
int l_incident_handler_report (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_incident_handler_transition (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_incident_handler_clear (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
int l_incident_handler_clearall (i_resource *self, i_socket *sock, i_message *msg, void *passdata);

/* incident_selfdestruct.c */
int l_incident_selfdestruct (i_resource *self, struct i_timer_s *timer, void *passdata);

/* incident_bind */
int l_incident_bind_case (i_resource *self, struct l_case_s *cas);
int l_incident_bind_case_entity (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);
int l_incident_bind_incident (i_resource *self, struct i_incident_s *inc);
int l_incident_bind_incident_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_incident_bind_entitymatch (struct i_entity_descriptor_s *ent1, struct i_entity_descriptor_s *ent2);

/* incident_unbind */
int l_incident_unbind_case (i_resource *self, long caseid);
int l_incident_unbind_case_entity (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);

/* incident_sql.c */
int l_incident_sql_insert (i_resource *self, struct i_incident_s *inc);
int l_incident_sql_update (i_resource *self, struct i_incident_s *inc);
int l_incident_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
struct i_callback_s* l_incident_sql_list (i_resource *self, char *id_str, char *state_str, char *startupper_str, char *startlower_str, char *endupper_str, char *endlower_str, char *enttype_str, char *cust_name, char *site_name, char *dev_name, char *cnt_name, char *obj_name, char *met_name, char *trg_name, char *adminstate_str, char *opstate_str, char *caseid_str, int max_count, int (*cbfunc) (), void *passdata);
int l_incident_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);

/* incident_notify.c */
int l_incident_notify (i_resource *self, struct i_incident_s *inc);

/* incident_xml_list.c */
int l_incident_xml_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

