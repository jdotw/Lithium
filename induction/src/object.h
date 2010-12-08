/* Cement - Object Entity */

typedef struct i_object_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_container_s *cnt;                    /* Parent container */
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

  /* Child Entity List/HT */
  struct i_list_s *met_list;                    /* List of metrics registered to this object */
  struct i_hashtable_s *met_ht;                 /* Hashtable of child entities */
  int (*met_list_sortfunc) ();                  /* External function for sorting the met list */

  /* Resource Data */
  struct i_resource_address_s *addr;            /* N/A */
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
   * Object-Specific Variables
   */

  /* Item variables */
  void *itemptr;                                /* Opaque 'item' pointer (e.g an i_interface struct) */

  /* Index */
  unsigned long index;                          /* An index number to help with sorting */

  /* Custom Graph List */
  struct i_list_s *cgraph_list;                 /* A list of custom/combined graphs */
  
  /* Optional Local-Object Trigger Set List */
  struct i_list_s *tset_list;                   /* A list of trigger sets for this container */
  struct i_hashtable_s *tset_ht;                /* A hash table of trigger sets for this cnt */
      
} i_object;

/* Refresh methods */
#define OBJECT_RMETHOD_CONTAINER 0      /* Object will automatically refreshed by the object */
#define OBJECT_RMETHOD_NONE 1           /* Object will not be automatically refreshed */
#define OBJECT_RMETHOD_STANDALONE 2     /* Object will automatically refresh itself */
#define OBJECT_RMETHOD_MANUAL 64        /* OR'd to value if a manual refresh is possible */

/* Refresh states */
#define OBJECT_RSTATE_IDLE 0            /* No refresh in progress */
#define OBJECT_RSTATE_BUSY 1            /* Refresh in progress */
#define OBJECT_RSTATE_MANUAL 64         /* OR'd to value if a manual refresh is occurring */

/* object.c */
i_object* i_object_create (char *name_str, char *desc_str);
void i_object_free (void *objptr);
int i_object_register (i_resource *self, struct i_container_s *cnt, i_object *obj);
int i_object_deregister (i_resource *self, i_object *obj);
