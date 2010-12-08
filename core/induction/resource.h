#include <sys/time.h>
#include <time.h>
#include <sys/select.h>

/* Resource Types and enumerator */

#define RES_CORE 0
#define RES_EXEC 1
#define RES_CLIENT_HANDLER_CORE 2
#define RES_CLIENT_HANDLER_CHILD 3
#define RES_ADMIN 4
#define RES_CUSTOMER 5
#define RES_SITE 6
#define RES_DEVICE 7
#define RES_SMS 11

#define RES_LCON 1024

typedef struct
{
  int type;
  char *type_str;
  char *module_name;
  int user_spawn;                   /* 0 = cant be spawned / added by user .. 1 = can be spawned / added by user */
} i_resource_info;

/* Resource states */

#define RES_STATE_UNKNOWN 0
#define RES_STATE_RUN 1
#define RES_STATE_SLEEP 2
#define RES_STATE_WAITING_REGISTER 3

/* Timers / Thresholds */

#define RES_FAST_RESTART_SECONDS 5		    /* If a resource restarts within x seconds of it being alive its determined a fast restart */
#define RES_FAST_RESTART_THRESHOLD 20		/* If a resource has x fast restarts put it to sleep */
#define RES_FAST_RESTART_SLEEP_SECONDS 30	/* Sleep forx seconds when a fast restart condition is detected */
#define RES_WAITING_REGISTER_TIMEOUT_SEC 5  /* A resource has x seconds to register before it is restarted */

/* Error codes */

#define RES_SANITY_CHECK_FAILED 342

/* Structs */

typedef struct i_resource_address_s
{
  char *node;				/* Name of the residing node */
  char *plexus;				/* Name of the residing plexus */
  int type;				    /* A predetermined Type */
  int ident_int;			/* A core/resource definable identifier int */
  char *ident_str;			/* A core/resource definable identifier string */
} i_resource_address;
#define RES_ADDR (i_resource_address *)

typedef struct i_resource_s
{
  char *node;	        			/* Name of the residing node */
  char *plexus;		        		/* Name of the residing plexus */
  int type;				            /* A predetermined Type */
  int ident_int;			        /* A core/resource definable identifier int */
  char *ident_str;			        /* A core/resource definable identifier string */

  struct i_entity_s *ent;           /* Local entity */

  int state;				        /* Sleep / Run etc */
  int status;                       /* Normal / Alert / Critical */
  long status_seq;                  /* Seqeuence number of the current status (avoids status race conditions) */
  int log_level;                    /* Logging output level, 0=normal-errors, 1=normal+errors, 2=debug, -1=silent */

  struct i_socket_s *core_socket;		        /* The channel used to communicate with the resource/core */
  struct i_construct_s *construct;			    /* The construct associated with this resource */
  struct timeval spawn_time;		/* When was the module spawned */
  struct timeval life_sign;		    /* When did the module last show signs of life */
  struct timeval wake_time;		    /* Time till the resource can wakeup from a sleep */

  struct i_form_s *config_form;              /* The resources configuration */

  char *module_name;			    /* The name of the module the resource it to run */

  int restart_count;			    /* Number of restarts that have been performed on this resource */
  int fast_restart_count;			/* Number of times a restart has happened quickly since the last long period of activity */

  int no_remove;                    /* 0 = can be user removed, 1 = can't be user removed. Zero by default, over rides user_spawn */
  int heartbeatfail_count;			    /* Number of times the heartbeat has failed */

  struct i_resource_address_s *owner;        /* The owner of this resource */

  char *root;                       /* The file system root for this resource */
  char *customer_id;                /* The customer this resource belongs to */

  struct i_authentication_s *auth;           /* The resources authentication if required */

  struct i_hierarchy_s *hierarchy;

  struct i_timer_s *heartbeat_timer;         /* Timer for sending/checking heartbeats */
  int heartbeat_recvd;              /* 1=Heatbeat received */

  struct i_hashtable_s *hosted;     /* Hosted resources */

  int perflog;                      /* Enable performance logging */

} i_resource;

typedef struct i_resource_spawn_cbdata_s
{
  int (*callback_func) (i_resource *self, i_resource_address *addr, void *passdata);
  void *passdata;

  long msgid;
  struct i_msgproc_callback_s *msg_callback;
  struct i_socket_s *socket;
  struct i_resource_s *self;
  int cancelled;
} i_resource_spawn_cbdata;

typedef struct i_resource_destroy_cbdata_s
{
  int (*callback_func) (i_resource *self, int result, void *passdata);
  void *passdata;

  long msgid;
  struct i_msgproc_callback_s *msg_callback;
  struct i_socket_s *socket;
} i_resource_destroy_cbdata;

typedef struct i_resource_register_cbdata_s
{
  int (*callback_func) (i_resource *self, int result, void *passdata);
  void *passdata;

  struct i_socket_s *socket;
  struct i_socket_data_s *write_op;
  struct i_socket_data_s *read_op;
} i_resource_register_cbdata;

typedef struct i_resource_local_incoming_cbdata_s
{
  int (*callback_func) (i_resource *self, i_resource *res, void *passdata);
  void *passdata;

  i_resource *res;
  struct i_hashtable_key_s *key;
  struct i_hashtable_s *res_table;
  struct i_socket_s *socket;
  struct i_socket_data_s *write_op;
  struct i_socket_data_s *read_op;
  struct i_timer_s *timeout_timer;
} i_resource_incoming_cbdata;

/* General resource functions - resource.c */

void i_resource_free (void *resptr);
i_resource* i_resource_self ();
char* i_resource_struct_to_data (i_resource *res, int *datasizeptr);
i_resource* i_resource_data_to_struct (char *data, unsigned int datasize);
i_resource *i_resource_hosted_get (i_resource *self, i_resource_address *addr);
long i_resource_send_ready (i_resource *self);
char* i_resource_state_str (i_resource *res);
char* i_resource_typestr (int type);

/* Resource Spawning related functions - res_spawn.c */

i_resource_spawn_cbdata* i_resource_spawn_cbdata_create ();
void i_resource_spawn_cbdata_free (void *cbdataptr);
i_resource_spawn_cbdata* i_resource_spawn (i_resource *self, int type, int ident_int, char *ident_str, char *module_name, char *root, char *customer_id, struct i_entity_s *ent, int (*callback_func) (i_resource *self, i_resource_address *addr, void *passdata), void *passdata);
int i_resource_spawn_msg_callback ();
int i_resource_spawn_failed (i_resource *self, i_resource_spawn_cbdata *cbdata);
int i_resource_spawn_cancel (i_resource_spawn_cbdata *cbdata);

/* Resource Destruction related functions - res_destroy.c */

i_resource_destroy_cbdata* i_resource_destroy_cbdata_create ();
void i_resource_destroy_cbdata_free (void *cbdataptr);
i_resource_destroy_cbdata* i_resource_destroy (i_resource *self, i_resource_address *res_addr, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata);
int i_resource_destroy_msg_callback ();
int i_resource_destroy_failed (i_resource *self, i_resource_destroy_cbdata *cbdata);

/* Resource Registration related functions - res_register.c */

i_resource_register_cbdata* i_resource_register_cbdata_create ();
void i_resource_register_cbdata_free (void *cbdataptr);
int i_resource_register (i_resource *self, char *core_socket_file, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata);
int i_resource_register_write_callback (i_resource *self, struct i_socket_s *sock, int result, void *passdata);
int i_resource_register_read_callback (i_resource *self, struct i_socket_s *sock, struct i_socket_data_s *data, int result, void *passdata);
int i_resource_register_failed (i_resource *self, i_resource_register_cbdata *cbdata);

/* Resource Address related functions - res_address.c */

void i_resource_address_free (void *addrptr);
void i_resource_free_address (void *addrptr);
i_resource_address* i_resource_address_create ();
char* i_resource_address_struct_to_string (i_resource_address *addr);
i_resource_address* i_resource_address_string_to_struct (char *str);
int i_resource_address_compare (i_resource_address *first, i_resource_address *second);
i_resource_address* i_resource_address_duplicate (i_resource_address *original);
char* i_resource_address_entity_string (i_resource_address *addr);

/* Resource State Checking functions - res_check.c */

void i_resource_local_sleep_check_single (i_resource *self, struct i_hashtable_s *res_table, void *data);
void i_resource_local_waiting_check_single (i_resource *self, struct i_hashtable_s *res_table, i_resource *res);
void i_resource_local_check_single (i_resource *self, struct i_hashtable_s *res_table, i_resource *res);
int i_resource_local_check_all (i_resource *self, struct i_timer_s *timer, void *tableptr);

/* Resource info functions - res_info.c */

i_resource_info* i_resource_info_create ();
void i_resource_info_free (i_resource_info *info);
i_resource_info* i_resource_info_duplicate (i_resource_info *info_in);
i_resource_info* i_resource_info_local (int type);
i_resource_info* i_resource_info_module (i_resource *self, char *module_name);
char* i_resource_info_type_string (i_resource *res);

/*  Local resource manipulation functions - res_local.c */

#define ERR_RES_DUPLICATE 128

i_resource* i_resource_local_get (struct i_hashtable_s *res_table, i_resource_address *res_addr);
i_resource* i_resource_local_create (i_resource *self, i_resource_address *owner_addr, struct i_hashtable_s *res_table, int type, int ident_int, char *ident_str, char *module_name, char *root, char *customer_id, struct i_entity_s *ent);
int i_resource_local_terminate (struct i_hashtable_s *res_table, i_resource_address *res_addr);
int i_resource_local_remove (i_resource *self, struct i_hashtable_s *res_table, i_resource_address *res_addr);
int i_resource_local_sleep (i_resource *self, struct i_hashtable_s *res_table, i_resource_address *res_addr, int seconds);
int i_resource_local_wake (i_resource *self, struct i_hashtable_s *res_table, i_resource_address *res_addr);
int i_resource_local_restart (i_resource *self, struct i_hashtable_s *res_table, i_resource *res);
int i_resource_local_spawn (i_resource *self, i_resource *res);
int i_resource_local_destroy (i_resource *self, struct i_hashtable_s *res_table, i_resource_address *res_addr);

/* Local resource incoming attachment - res_local_incoming.c */

i_resource_incoming_cbdata* i_resource_incoming_cbdata_create ();
void i_resource_incoming_cbdata_free (void *cbdataptr);
int i_resource_local_incoming_list_add (i_resource *self, i_resource *res);
int i_resource_local_incoming_list_remove (i_resource *res);
i_resource_incoming_cbdata* i_resource_local_incoming (i_resource *self, struct i_hashtable_s *res_table, int listener, int (*callback_func) (i_resource *self, i_resource *res, void *passdata), void *passdata);
int i_resource_local_incoming_read_callback (i_resource *self, struct i_socket_s *sock, struct i_socket_data_s *data, int result, void *passdata);
int i_resource_local_incoming_write_callback (i_resource *self, struct i_socket_s *sock, int result, void *passdata);
int i_resource_local_incoming_timeout_callback (i_resource *self, struct i_timer_s *timer, void *passdata);
int i_resource_local_incoming_failed (i_resource *self, i_resource_incoming_cbdata *cbdata);

/* Resource form for forcing termination - res_form_quit.c */

int i_res_form_quit_timer_callback (i_resource *self, struct i_timer_s *timer, void *passdata);
