/* Cement - Container Entity */

typedef struct i_container_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_device_s *dev;                       /* Parent Device */
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
  struct i_list_s *obj_list;                    /* Objects */
  struct i_hashtable_s *obj_ht;                 /* Hashtable of child entities */
  int (*obj_list_sortfunc) ();                  /* External function for sorting the obj list */

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
   * Container-Specific Variables
   */

  /* Item list */
  struct i_list_s *item_list;                   /* A list of container-specific items (e.g. interface structs) */
  unsigned short int item_list_state;           /* The state of the item list */

  /* Summary */
  int (*sumform_func) ();                       /* Function to generate mainform summary info */

  /* Trigger Set List */
  struct i_list_s *tset_list;                   /* A list of trigger sets for this container */
  struct i_hashtable_s *tset_ht;                /* A hash table of trigger sets for this cnt */

  /* Module Builder */
  struct i_list_s *met_templates;               /* Metric templates */
  void *objfactptr;
  unsigned short modb;                          /* 1=Created using Module Builder */
  char *cview1_str;                             /* Console view position 1 */
  char *cview2_str;                             /* Console view position 2 */
  char *cview3_str;                             /* Console view position 3 */
  char *cview4_str;                             /* Console view position 4 */
  struct i_list_s *wview_metrics;               /* List of web-view metrics in order */
  unsigned short wview_mode;                    /* 0=List 1=Table */

  /* Metrics SQL Table */
  unsigned short int metric_sql_checked;        /* 1=Table has been checked/created */

} i_container;

#define CNT_TSETHT_SIZE 5                       /* Triggerset Hashtable Size */

/* Item List States */
#define ITEMLIST_STATE_NONE 0                   /* List not present */
#define ITEMLIST_STATE_NORMAL 1                 /* Populated and ready */
#define ITEMLIST_STATE_POPULATE 2               /* List is being populated */

/* container.c */
i_container* i_container_create (char *name_str, char *desc_str);
void i_container_free (void *cntptr);
int i_container_register (i_resource *self, struct i_device_s *dev, i_container *cnt);
int i_container_register_sqlselectcb ();
int i_container_register_sqlcreatecb ();
int i_container_deregister (i_resource *self, i_container *cnt);
