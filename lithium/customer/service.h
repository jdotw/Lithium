/*
 * Actions - User-defined scripts to be run in the event
 *           of an incident occuring.
 */

#include <libxml/parser.h>

typedef struct l_service_s
{
  /* Action Info */
  unsigned long id;           /* Auto-assigned by SQL */
  char *desc_str;             /* User-defined */

  /* Behaviour */
  int enabled;                /* 0=Disabled, 1=Enabled */
  int activation;             /* 0=Manual, 1=Automatic after delay */
  time_t delay;               /* Seconds before service is automatically fired */
  int rerun;                  /* Re-run the script */
  int rerun_delay;            /* Delay between re-running */

  /* Filter */
  int time_filter;            /* 0=All day, 1=Time filtered */
  int day_mask;               /* Day Mask */
  int start_hour;             /* Start Hour */
  int end_hour;               /* End Hour */
  struct i_list_s *entity_list;  /* Entities to execute this service for */

  /* Script */
  char *script_file;          /* Script filename */

  /* Config */
  struct i_list_s *configvar_list;

  /* Run-time variables */
  int entity_count;
  int run_count;
  int execute_flag;
  struct i_timer_s *delay_timer;
  struct i_timer_s *rerun_timer;
  struct i_incident_s *inc;

} l_service;

typedef struct l_service_script_s 
{
  char *name_str;             /* Filename */
  char *desc_str;             /* Script-reported description */
  struct i_list_s *configvar_list;  /* Config variable list */
  int status;                /* Status */
} l_service_script;

typedef struct l_service_configvar_s 
{
  long id;                    /* SQL ID */
  char *name_str;             /* Unique ID */
  char *desc_str;             /* Human readable description */
  char *value_str;            /* Current value */
  int required;               /* 0=Optional 1=Required */
} l_service_configvar;

typedef struct l_service_log_s
{
  unsigned long id;
  i_entity_descriptor *ent;
  time_t timestamp;
  char *comment_str;
} l_service_log;

typedef struct l_service_proc_s
{
  /* Process */
  int pid;
  int readfd;
  int writefd;

  /* Socket */
  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;

  /* Script */
  char *script_file;            /* File to execute */
  char *output_str;              /* The output from the script */

  /* Command line args */
  char *command_str;            /* Command to be executed */
  long incid;
  char *entaddr_str;
  unsigned int opstate;
  char *cust_desc;
  char *site_desc;
  char *dev_desc;
  char *cnt_desc;
  char *obj_desc;
  char *met_desc;
  char *trg_desc;
  int run_count;
  time_t start_sec;
  time_t end_sec;

  /* Action */
  unsigned long serviceid;
  struct i_entity_descriptor_s *entdesc;

  /* Callbacks */
  struct i_callback_s *cb;            /* Callback to be called after execution */
  struct i_callback_s *configvar_cb;  /* Callback from the configvar load operation */

} l_service_proc;

#define ACTION_DAY_MON 1
#define ACTION_DAY_TUE 2
#define ACTION_DAY_WED 4
#define ACTION_DAY_THU 8
#define ACTION_DAY_FRI 16
#define ACTION_DAY_SAT 32
#define ACTION_DAY_SUN 64

/* service.c */
int l_service_enable (i_resource *self);
l_service* l_service_create ();
void l_service_free (void *serviceptr);

/* service_id.c */
long l_service_id_assign ();
void l_service_id_setcurrent (long currentid);

/* service_proc.c */
l_service_proc* l_service_proc_create ();
void l_service_proc_free (void *procptr);
l_service_proc* l_service_exec (i_resource *self, char *script_file, char *command_str, l_service *service, struct i_incident_s *inc, int (*cbfunc) (), void *passdata);
int l_service_exec_configvar_cb (i_resource *self, struct i_list_s *list, void *passdata);
int l_service_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_service_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_service_exec_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);

/* service_log.c */
l_service_log* l_service_log_create ();
void l_service_log_free (void *logptr);

/* service_script.c */
l_service_script* l_service_script_create ();
void l_service_script_free (void *scriptptr);

/* service_sql.c */
int l_service_sql_insert (i_resource *self, l_service *service);
int l_service_sql_update (i_resource *self, l_service *service);
int l_service_sql_delete (i_resource *self, int service_id);
struct i_callback_s* l_service_sql_load_list (i_resource *self, char *id_str, int (*cbfunc) (), void *passdata);
int l_service_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_entity_insert (i_resource *self, long service_id, i_entity_descriptor *entdesc);
int l_service_sql_entity_delete (i_resource *self, long service_id, i_entity_descriptor *entdesc);
struct i_callback_s* l_service_sql_entity_load_list (i_resource *self, int service_id, int (*cbfunc) (), void *passdata);
int l_service_sql_entity_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_configvar_update_insert (i_resource *self, long service_id, l_service_configvar *var, int force_insert);
int l_service_sql_configvar_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_service_sql_configvar_delete (i_resource *self, long service_id);
struct i_callback_s* l_service_sql_configvar_load_list (i_resource *self, long service_id, int (*cbfunc) (), void *passdata);
int l_service_sql_configvar_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_log_insert (i_resource *self, long service_id, struct i_entity_descriptor_s *ent, char *comment_str);
struct i_callback_s* l_service_sql_log_load (i_resource *self, int service_id, int (*cbfunc) (), void *passdata);
int l_service_sql_log_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* service_sql_candidates.c */
struct i_callback_s* l_service_sql_load_candidates (i_resource *self, struct i_incident_s *inc, int (*cbfunc) (), void *passdata);
int l_service_sql_load_candidates_servicecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_load_candidates_idcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);

/* service_configvar.c */
l_service_configvar* l_service_configvar_create ();
void l_service_configvar_free (void *varptr);

/* service_incident.c */
int l_service_incident_report (i_resource *self, struct i_incident_s *inc);
int l_service_incident_candidatecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_service_incident_delaytimercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_service_incident_run_service (i_resource *self, struct l_service_s *service);
int l_service_incident_reruntimercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_service_incident_clear (i_resource *self, struct i_incident_s *inc);

/* service_xml.c */
xmlNodePtr l_service_xml (l_service *service);
xmlNodePtr l_service_configvar_xml (l_service_configvar *var);
l_service_configvar* l_service_configvar_fromxml ();
xmlNodePtr l_service_script_xml (l_service_script *script);
xmlNodePtr l_service_log_xml (l_service_log *log);

/* service_xml_list.c */
int l_service_xml_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* service_xml_script_list.c */
int l_service_xml_script_list_scriptcb (i_resource *self, l_service_proc *proc, void *passdata);

/* service_xml_update.c */
int l_service_xml_update_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* service_xml_configvar_list */
int l_service_xml_configvar_list_sqlcb (i_resource *self, struct i_list_s *variable_list, void *passdata);

/* service_xml_entity.c */
int l_service_xml_entity_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* service_xml_execute.c */
int l_service_xml_execute_cb (i_resource *self, l_service_proc *proc, void *passdata);

/* service_xml_history.c */
int l_service_xml_history_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

