typedef struct i_incident_s
{
  /* Incident Variables */
  long id;                                  /* Assigned by SQL */
  unsigned short state;                     /* Incident state */
  unsigned short type;                      /* Incident type */
  struct i_entity_descriptor_s *ent;        /* Current Reporting entity */
  struct i_entity_descriptor_s *prev_ent;   /* Previous Reporting entity */
  struct i_entity_descriptor_s *orig_ent;   /* Original Reporting entity */
  struct i_entity_descriptor_s *met;        /* Owner metric */
  struct timeval start_tv;                  /* Start of Incident */
  struct timeval end_tv;                    /* End of Incident */
  char *raised_valstr;                      /* The metric value that raised the incident */
  char *cleared_valstr;                     /* The metric value that cleared the incident */
  int trg_type;                             /* Trigger type (condition) */
  char *trg_xval_str;                       /* Trigger's X Value */
  char *trg_yval_str;                       /* Trigger's Y Value */
  int highest_opstate;                      /* Highest operational state seen */
  int lowest_opstate;                       /* Lowest operational state seen */
  struct timeval last_transition_tv;        /* Last transition time stamp */
  int occurrences;                          /* Occurrence count */
  struct timeval last_occurrence_tv;        /* Last occurrence */

  /* Case binding */
  long caseid;                              /* Case incident is bound to */

  /* Reporting variables */
  long msgid;                               /* Message ID of MSG_INC_REPORT message */
  struct i_msgproc_callback_s *msgcb;       /* Message processor callback for MSG_INC_REPORT response */

  /* Tracking variables */
  void *listp;
  void *cleared_listp;

  /* Action variables */
  struct i_list_s *action_list;             /* Candidate actions */
  struct i_callback_s *action_cb;           /* Callback for loading candidates */

  /* Local Pointers */
  struct i_trigger_s *local_trg;            /* Local Trigger Pointer */

  /* Recent Cleared Cache */
  struct i_timer_s *destruct_timer;         /* Removes the incident from the recent cleared cache */

} i_incident;

/* Incident States */
#define INCSTATE_INACTIVE 0
#define INCSTATE_ACTIVE 1

/* Incident Types */
#define INCTYPE_ADMINSTATE 1
#define INCTYPE_OPSTATE 2
#define INCTYPE_ALL 3

/* incident.c */

i_incident* i_incident_create ();
void i_incident_free (void *incptr);
char* i_incident_data (i_incident *inc, int *datasizeptr);
i_incident* i_incident_struct (char *data, int datasize);
char* i_incident_url (i_incident *inc);
char* i_incident_typestr (unsigned short type);
char* i_incident_statestr (unsigned short state);

/* incident_report.c */

i_incident* i_incident_report (i_resource *self, unsigned short type, i_entity *ent);
int i_incident_report_msgcb (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);

/* incident_transition.c */

int i_incident_transition (i_resource *self, struct i_incident_s *inc, struct i_entity_s *ent);

/* incident_clear.c */
int i_incident_clear (i_resource *self, i_entity *ent, i_incident *inc);

/* incident_clearall.c */
int i_incident_clearall (i_resource *self, i_resource_address *custresaddr, i_resource_address *devresaddr);
