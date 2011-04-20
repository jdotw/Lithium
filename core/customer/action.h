/*
 * Actions - User-defined scripts to be run in the event
 *           of an incident occuring.
 */

#include <libxml/parser.h>

typedef struct l_action_s
{
  /* Action Info */
  unsigned long id;           /* Auto-assigned by SQL */
  char *desc_str;             /* User-defined */

  /* Behaviour */
  int enabled;                /* 0=Disabled, 1=Enabled */
  int activation;             /* 0=Manual, 1=Automatic after delay */
  time_t delay;               /* Seconds before action is automatically fired */
  int rerun;                  /* Re-run the script */
  int rerun_max;              /* Maximum re-runs */
  int rerun_delay;            /* Delay between re-running */
  int casecancels;            /* 1=Binding to a case cancels action, 2=Binding to a case cancels rerun */
  int log_output;             /* Log the output from the script */

  /* Filter */
  int time_filter;            /* 0=All day, 1=Time filtered */
  int day_mask;               /* Day Mask */
  int start_hour;             /* Start Hour */
  int end_hour;               /* End Hour */
  int min_status;             /* Minimum status level */
  struct i_list_s *entity_list;  /* Entities to execute this action for */

  /* Script */
  char *script_file;          /* Script filename */

  /* Config */
  struct i_list_s *configvar_list;

  /* Run-time variables */
  int runstate;               /* 0=Dormant waiting manual activation 1=Active and about to run/re-run */
  int entity_count;
  int run_count;              /* Total execution count */
  int rerun_count;            /* Automatic re-run count */
  int execute_flag;
  struct i_timer_s *delay_timer;
  struct i_timer_s *rerun_timer;
  struct i_incident_s *inc;

} l_action;

typedef struct l_action_script_s 
{
  char *name_str;             /* Filename */
  char *desc_str;             /* Script-reported description */
  char *info_str;             /* Script-reported info / long-description */
  char *version_str;          /* Script-reported version */
  struct i_list_s *configvar_list;  /* Config variable list */
  int status;                /* Status */
} l_action_script;

typedef struct l_action_configvar_s 
{
  long id;                    /* SQL ID */
  char *name_str;             /* Unique ID */
  char *desc_str;             /* Human readable description */
  char *value_str;            /* Current value */
  int required;               /* 0=Optional 1=Required */
} l_action_configvar;

typedef struct l_action_log_s
{
  unsigned long id;
  i_entity_descriptor *ent;
  time_t timestamp;
  char *comment_str;
} l_action_log;

typedef struct l_action_proc_s
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
  int log_output;               /* 1=Log output of script */

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
  int highest_opstate;        
  int lowest_opstate;
  char *prev_trg_desc;
  int prev_opstate;
  time_t last_transition_sec;
  time_t last_occurrence_sec;
  int occurrences;
  char *entity_url;
  char *metric_url;
  int incident_count;
  char *incident_url;
  char *temp_config_file;

  /* Action */
  unsigned long actionid;
  struct i_entity_descriptor_s *entdesc;

  /* Callbacks */
  struct i_callback_s *cb;            /* Callback to be called after execution */
  struct i_callback_s *configvar_cb;  /* Callback from the configvar load operation */

} l_action_proc;

#define ACTION_DAY_MON 1
#define ACTION_DAY_TUE 2
#define ACTION_DAY_WED 4
#define ACTION_DAY_THU 8
#define ACTION_DAY_FRI 16
#define ACTION_DAY_SAT 32
#define ACTION_DAY_SUN 64

#define CASE_CANCELS_ALL 1
#define CASE_CANCELS_RERUN 2

/* action.c */
int l_action_enable (i_resource *self);
l_action* l_action_create ();
void l_action_free (void *actionptr);

/* action_id.c */
long l_action_id_assign ();
void l_action_id_setcurrent (long currentid);

/* action_proc.c */
l_action_proc* l_action_proc_create ();
void l_action_proc_free (void *procptr);
l_action_proc* l_action_exec (i_resource *self, char *script_file, char *command_str, l_action *action, struct i_incident_s *inc, int (*cbfunc) (), void *passdata);
int l_action_exec_configvar_cb (i_resource *self, struct i_list_s *list, void *passdata);
int l_action_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_action_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_action_exec_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);

/* action_log.c */
l_action_log* l_action_log_create ();
void l_action_log_free (void *logptr);

/* action_script.c */
l_action_script* l_action_script_create ();
void l_action_script_free (void *scriptptr);

/* action_sql.c */
int l_action_sql_insert (i_resource *self, l_action *action);
int l_action_sql_update (i_resource *self, l_action *action);
int l_action_sql_delete (i_resource *self, int action_id);
struct i_callback_s* l_action_sql_load_list (i_resource *self, char *id_str, int (*cbfunc) (), void *passdata);
int l_action_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_action_sql_entity_insert (i_resource *self, long action_id, i_entity_descriptor *entdesc);
int l_action_sql_entity_delete (i_resource *self, long action_id);
struct i_callback_s* l_action_sql_entity_load_list (i_resource *self, int action_id, int (*cbfunc) (), void *passdata);
int l_action_sql_entity_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_action_sql_configvar_insert (i_resource *self, long action_id, l_action_configvar *var);
int l_action_sql_configvar_delete (i_resource *self, long action_id);
struct i_callback_s* l_action_sql_configvar_load_list (i_resource *self, long action_id, int (*cbfunc) (), void *passdata);
int l_action_sql_configvar_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_action_sql_log_insert (i_resource *self, long action_id, struct i_entity_descriptor_s *ent, char *comment_str);
struct i_callback_s* l_action_sql_log_load (i_resource *self, int action_id, int (*cbfunc) (), void *passdata);
int l_action_sql_log_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_action_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* action_sql_candidates.c */
struct i_callback_s* l_action_sql_load_candidates (i_resource *self, struct i_incident_s *inc, int (*cbfunc) (), void *passdata);
int l_action_sql_load_candidates_actioncb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_action_sql_load_candidates_idcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);

/* action_configvar.c */
l_action_configvar* l_action_configvar_create ();
void l_action_configvar_free (void *varptr);

/* action_incident.c */
int l_action_incident_report (i_resource *self, struct i_incident_s *inc);
int l_action_incident_candidatecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_action_incident_delaytimercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_action_incident_run_action (i_resource *self, struct l_action_s *action);
int l_action_incident_reruntimercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_action_incident_transition (i_resource *self, struct i_incident_s *inc);
int l_action_incident_clear (i_resource *self, struct i_incident_s *inc);
int l_action_incident_silence (i_resource *self, struct i_incident_s *inc);
int l_action_incident_boundtocase (i_resource *self, struct i_incident_s *inc);

/* action_xml.c */
xmlNodePtr l_action_xml (l_action *action);
xmlNodePtr l_action_configvar_xml (l_action_configvar *var);
l_action_configvar* l_action_configvar_fromxml ();
xmlNodePtr l_action_script_xml (l_action_script *script);
xmlNodePtr l_action_log_xml (l_action_log *log);

/* action_xml_list.c */
int l_action_xml_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* action_xml_script_list.c */
int l_action_xml_script_list_scriptcb (i_resource *self, l_action_proc *proc, void *passdata);

/* action_xml_update.c */
int l_action_xml_update_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* action_xml_configvar_list */
int l_action_xml_configvar_list_sqlcb (i_resource *self, struct i_list_s *variable_list, void *passdata);

/* action_xml_entity.c */
int l_action_xml_entity_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

/* action_xml_history.c */
int l_action_xml_history_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

