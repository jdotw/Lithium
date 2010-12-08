/* Case System */

typedef struct l_case_s
{
  long id;
  unsigned short state;
  struct timeval start;
  struct timeval end;
  char *hline_str;
  char *owner_str;
  char *requester_str;

  struct i_list_s *ent_list;

  struct i_list_s *log_list;
} l_case;

typedef struct l_case_logentry_s
{
  long id;
  long caseid;
  unsigned short case_state;
  unsigned short type;
  struct timeval tstamp;
  time_t timespent_sec;
  char *author_str;
  char *entry_str;
} l_case_logentry;

#define CASE_STATE_CLOSED 0
#define CASE_STATE_OPEN 1

#define CASE_STATE_CLOSE_NUMSTR "0"
#define CASE_STATE_OPEN_NUMSTR "1"

/* case.c */
l_case* l_case_create ();
void l_case_free (void *caseptr);
int l_case_enable (i_resource *self);

/* case_id.c */
long l_case_id_assign ();
void l_case_id_setcurrent (long currentid);

/* case_open.c */
int l_case_open (i_resource *self, l_case *cas, i_list *ent_list);

/* case_reopen.c */
int l_case_reopen (i_resource *self, l_case *cas, char *owner_str);
int l_case_reopen_entitycb (i_resource *self, i_list *list, void *passdata);

/* case_close.c */
int l_case_close (i_resource *self, long caseid);
int l_case_close_casecb (i_resource *self, i_list *list, void *passdata);

/* case_str.c */
char* l_case_statestr (unsigned short state);

/* case_sql.c */
int l_case_sql_insert (i_resource *self, l_case *cas);
int l_case_sql_update (i_resource *self, l_case *cas);
int l_case_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
struct i_callback_s* l_case_sql_list (i_resource *self, char *id_str, char *state_str, char *startupper_str, char *startlower_str, char *endupper_str, char *endlower_str, char *hline_str, char *owner_str, char *requester_str, int (*cbfunc) (), void *passdata);
struct i_callback_s* l_case_sql_list_idlist (i_resource *self, struct i_list_s *idlist, int (*cbfunc) (), void *passdata);
int l_case_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
struct i_callback_s* l_case_sql_get (i_resource *self, char *caseid_str, int (*cbfunc) (), void *passdata);
int l_case_sql_get_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_case_sql_get_entitycb (i_resource *self, struct i_list_s *list, void *passdata);
int l_case_sql_get_logcb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_form_main.c */
int form_case_main ();
int l_case_form_main_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_case_form_main_submit_casecb (i_resource *self, i_list *list, void *passdata);

/* case_form_view.c */
int form_case_view ();
int l_case_form_view_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_case_form_view_logcb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_form_edit.c */
int form_case_edit ();
int l_case_form_edit_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int form_case_edit_submit ();
int l_case_form_edit_submit_casecb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_form_logupdate.c */
int form_case_logupdate ();
int l_case_logupdate_casecb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_form_close.c */
int form_case_close ();
int l_case_form_close_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int form_case_close_submit ();
int l_case_form_close_submit_casecb ();

/* case_logentry.c */
l_case_logentry* l_case_logentry_create ();
void l_case_logentry_free (void *logptr);

/* case_logentry_sql.c */
int l_case_logentry_sql_insert (i_resource *self, long caseid, l_case_logentry *log);
int l_case_logentry_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
struct i_callback_s* l_case_logentry_sql_list (i_resource *self, long caseid, int (*cbfunc) (), void *passdata);
int l_case_logentry_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_case_logentry_sql_update_casestate (i_resource *self, long caseid, unsigned short state);

/* case_entity.c */
int l_case_entity_add (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);
int l_case_entity_remove (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);

/* case_entity_sql.c */
int l_case_entity_sql_insert (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);
int l_case_entity_sql_delete (i_resource *self, long caseid, struct i_entity_descriptor_s *ent);
int l_case_entity_sql_delete_device ();
int l_case_entity_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
struct i_callback_s* l_case_entity_sql_list (i_resource *self, char *caseid_str, char *casestate_str, char *type_str, char *site_str, char *dev_str, char *cnt_str, char *obj_str, char *met_str, char *trg_str, char *tstampupper_str, char *tstamplower_str, int (*cbfunc) (), void *passdata);
int l_case_entity_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_case_entity_sql_update_casestate (i_resource *self, long caseid, unsigned short state);

/* case_entity_xml_list.c */
int xml_case_entity_list ();
int l_case_entity_xml_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);
