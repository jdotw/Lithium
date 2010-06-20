/*
 * Actions - User-defined scripts to be run in the event
 *           of an incident occuring.
 */

#include <libxml/parser.h>

typedef struct l_service_s
{
  /* Service Info */
  unsigned long id;           /* Auto-assigned by SQL */
  char *desc_str;             /* User-defined */

  /* Behaviour */
  int enabled;                /* 0=Disabled, 1=Enabled */

  /* Script */
  char *script_file;          /* Script filename */

  /* Config */
  struct i_list_s *configvar_list;

  /* Object */
  struct i_object_s *obj;

  /* Default metrics */
  struct i_metric_s *status_met;
  struct i_metric_s *resptime_met;
  struct i_metric_s *transtime_met;
  struct i_metric_s *version_met;
  struct i_metric_s *message_met;

} l_service;

typedef struct l_service_script_s 
{
  char *name_str;                   /* Filename */
  char *desc_str;                   /* Script-reported description */
  char *info_str;                   /* Script-reported info / long description */
  char *version_str;                /* Script-reported version */
  char *protocol_str;               /* Script-reported protocol */
  char *transport_str;              /* Script-reported transport (udp/tcp) */
  char *port_str;                   /* Script-reported port */
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
  char *temp_config_file;

  /* Action */
  unsigned long serviceid;

  /* Callbacks */
  struct i_callback_s *cb;            /* Callback to be called after execution */
  struct i_callback_s *configvar_cb;  /* Callback from the configvar load operation */

} l_service_proc;

/* service.c */
struct i_container_s* l_service_cnt ();
struct i_list_s* l_service_list ();
int l_service_enable (i_resource *self);
int l_service_enable_loadcb (i_resource *self, struct i_list_s *list, void *passdata);
l_service* l_service_create ();
void l_service_free (void *serviceptr);

/* service_proc.c */
l_service_proc* l_service_proc_create ();
void l_service_proc_free (void *procptr);
l_service_proc* l_service_exec (i_resource *self, char *script_file, char *command_str, l_service *service, int (*cbfunc) (), void *passdata);
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
struct i_callback_s* l_service_sql_load_list (i_resource *self, char *id_str, char *site_name, char *dev_name, int (*cbfunc) (), void *passdata);
int l_service_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_configvar_insert (i_resource *self, long service_id, l_service_configvar *var);
int l_service_sql_configvar_delete (i_resource *self, long service_id);
struct i_callback_s* l_service_sql_configvar_load_list (i_resource *self, long service_id, int (*cbfunc) (), void *passdata);
int l_service_sql_configvar_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_log_insert (i_resource *self, long service_id, char *comment_str);
struct i_callback_s* l_service_sql_log_load (i_resource *self, int service_id, int (*cbfunc) (), void *passdata);
int l_service_sql_log_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata);
int l_service_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* service_configvar.c */
l_service_configvar* l_service_configvar_create ();
void l_service_configvar_free (void *varptr);

/* service_object.c */
int l_service_object_add (i_resource *self, l_service *service);
int l_service_object_add_scriptcb (i_resource *self, l_service_proc *proc, void *passdata);
int l_service_object_update (i_resource *self, l_service *service);
int l_service_object_remove (i_resource *self, unsigned long id);

/* service_metric.c */
struct i_metric_s* l_service_metric_fromxml ();

/* service_triggerset.c */
struct i_triggerset_s* l_service_triggerset_fromxml ();

/* service_trigger.c */
int l_service_trigger_fromxml ();

/* service_refresh.c */
int l_service_refresh (i_resource *self, struct i_object_s *obj, int opcode);
int l_service_refresh_procb (i_resource *self, l_service_proc *proc, void *passdata);
int l_service_refresh_invalidate_all (i_resource *self, struct i_object_s *obj);

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

/* service_xml_history.c */
int l_service_xml_history_list_sqlcb (i_resource *self, struct i_list_s *list, void *passdata);

