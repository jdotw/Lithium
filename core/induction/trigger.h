/* Cement - Trigger Entity */

typedef struct i_trigger_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_metric_s *met;                       /* Parent metric */
  uuid_t uuid;                                  /* UUID */
  char *desc_esc_str;                           /* Escaped */

  /* Priority */
  unsigned long prio;                           /* Entity sort priority */

  /* Entity State */
  short adminstate;                    		/* Administrative State of Entity */
  struct i_incident_s *admin_inc;               /* Administrative State Incident */
  short opstate;                       		/* Operational State of Entity */
  unsigned short registered;                    /* 0=Not Registered 1=Registered */
  unsigned short hidden;                        /* Whether to show via xml or web */
  unsigned short authorative;                   /* Whether or not the local resource is authorative for this entity */
  time_t version;                               /* Version of entity (for sync) */

  /* Filesystem Path */
  char *fsroot_str;                             /* The filesystem root dir for this entity */
  unsigned short fs_flags;                      /* Flags related to the file system */
  unsigned short fsroot_valid;                  /* 0=Path not created 1=Path created */
  time_t fsroot_month_vsec;                     /* Earliest time mkdir needs to be called to create the month path */
  time_t fsroot_year_vsec;                      /* Earliest time mkdir needs to be called to create the year path */

  /* Child Entity List */
  struct i_list_s *child_list;                  /* N/A */
  struct i_hashtable_s *child_ht;               /* N/A */
  int (*child_list_sortfunc) ();                /* N/A */

  /* Resource Data */
  struct i_resource_address_s *resaddr;         /* N/A */
  struct i_resource_spawn_cbdata_s *spawn_op;   /* N/A */
  struct i_form_s *config_form;                 /* FIX DEFUNCT */

  /* Forms / Navigation */
  int (*navtree_func) ();                       /* Function to generate navigation tree */
  unsigned int navtree_expand;                  /* Navtree Expansion Flag */
  int (*mainform_func) ();                      /* A general 'main' form for this entity */
  int (*histform_func) ();                      /* A general 'historic' form for this entity */
  int (*xml_func) ();                           /* Entity-specific XML func */

  /* XML Sync */
  struct i_timer_s *xmlsync_timer;              /* XML Sync refresh timer */
  int (*xmlsync_delegate) ();                   /* XML Sync Delegate */
  struct i_xml_request_s *xmlsync_req;          /* XML Sync Request */
  long xmlsync_tag;                             /* XML Sync Refresh Tag */
  unsigned int xmlsync_collisions;              /* XML Sync Collision Count */
  time_t xmlsync_version;                       /* XML Sync synced version of entity */

  /* Refesh Configuration */
  short int refresh_method;                     /* Method for refreshing child entities */
  time_t refresh_int_sec;                       /* Device refresh interval */
  unsigned short refresh_maxcolls;              /* Maximum refresh collission count */

  /* Refresh Data */
  struct i_timer_s *refresh_timer;              /* Refresh timer */
  int (*refresh_func) ();                       /* Refresh function */
  void* refresh_data;                           /* Opaque handle for use by refresh function */
  unsigned short refresh_state;                 /* Refresh state */
  unsigned short refresh_colls;                 /* Refresh collission count */
  unsigned short refresh_result;                /* Result of last refresh op */
  unsigned short refresh_forcedterm;            /* Forced termination or normal termination */
  struct timeval refresh_tstamp;                /* Time-stamp of last refresh */ 
  struct i_list_s *refreshcb_list;              /* List of refresh callbacks */

  /* SNMP Agent */
  void *agent_rowptr;                           /* SNMP Row */
  int agent_rowindex;                           /* Our Index */
  int agent_nextchildindex;                     /* Next index to give to a child */

  /* 
   * Trigger-specific variables
   */

  /* Trigger configuration */
  unsigned short val_type;                      /* Type of value */
  struct i_value_s *val;                        /* Value of trigger */
  unsigned short trg_type;                      /* Type of trigger */
  unsigned short effect;                        /* Status level to be set when trigger is active */
  struct i_value_s *yval;                       /* y/secondary value */
  time_t duration_sec;                          /* Duration-based triggers */
  int active;                                   /* 0=Dormant 1=Action/Condition Met */
  struct timeval active_tstamp;                 /* Timstamp when this trigger first became active */

} i_trigger;

typedef struct i_trigger_value_loadapply_cbdata_s
{
  struct i_value_s *def_val;
  struct i_value_s *def_yval;
} i_trigger_value_loadapply_cbdata;

/* Trigger Types */
#define TRGTYPE_EQUAL 1                         /* Metric = Trigger */
#define TRGTYPE_LT 2                            /* Metric < Trigger */
#define TRGTYPE_GT 3                            /* Metric > Trigger */
#define TRGTYPE_NOTEQUAL 4                      /* Metric != Trigger */
#define TRGTYPE_RANGE 5                         /* xValue <= Trigger < yValue */

/* trigger.c */
i_trigger* i_trigger_create (char *name_str, char *desc_str, unsigned short val_type, unsigned short trg_type, unsigned short effect);
void i_trigger_free (void *trgptr);
i_trigger* i_trigger_duplicate (i_trigger *trg);
int i_trigger_register (i_resource *self, i_metric *met, i_trigger *trg);
int i_trigger_deregister (i_resource *self, i_trigger *trg);
char* i_trigger_typestr (unsigned short trg_type);

/* trigger_value.c */
struct i_callback_s* i_trigger_value_load (i_resource *self, i_trigger *trg, int (*cbfunc) (i_resource *self, i_trigger *trg, struct i_value_s *val, struct i_value_s *yval, void *passdata), void *passdata);
int i_trigger_value_load_sqlcb ();
int i_trigger_value_apply (i_resource *self, i_trigger *trg, struct i_value_s *val, struct i_value_s *yval);
struct i_callback_s* i_trigger_value_loadapply (i_resource *self, i_trigger *trg, struct i_value_s *def_val, struct i_value_s *def_yval);
int i_trigger_value_loadapply_loadcb (i_resource *self, i_trigger *trg, struct i_value_s *val, struct i_value_s *yval, void *passdata);

/* trigger_eval.c */
int i_trigger_eval (i_resource *self, i_trigger *trg, i_metric_value *val);

/* trigger_process.c */
int i_trigger_process (i_resource *self, i_trigger *trg);
int i_trigger_process_all (i_resource *self, i_metric *met);
