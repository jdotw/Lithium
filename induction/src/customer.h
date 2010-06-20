/* Cement - Customer Entity */

typedef struct i_customer_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_entity_s *parent;                    /* Parent entity */
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
  time_t fsroot_month_vsec;             /* Earliest time mkdir needs to be called to create the month path */
  time_t fsroot_year_vsec;              /* Earliest time mkdir needs to be called to create the year path */

  /* Site Entity List/HT */
  struct i_list_s *site_list;                   /* List of site entities */
  struct i_hashtable_s *site_ht;                /* Hashtable of site entities */
  int (*site_list_sortfunc) ();                 /* External function for sorting the site list */

  /* Resource Data */
  struct i_resource_address_s *resaddr;         /* The resource address of the entity */
  struct i_resource_spawn_cbdata_s *spawn_op;   /* Data concerning the spawn of the entity's resource */
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
  time_t refresh_int_sec;                       /* Refresh interval (seconds) */
  unsigned short refresh_maxcolls;              /* Maximum refresh collission count */

  /* Refresh Data */
  struct i_timer_s *refresh_timer;              /* Refresh timer */
  int (*refresh_func) ();                       /* Refresh function */
  void* refresh_data;                           /* Opaque handle for use by refresh function */
  unsigned short refresh_state;                 /* Refresh state */
  unsigned short refresh_colls;                 /* Refresh collission count */
  unsigned short refresh_result;                /* Result of last refresh op */
  struct timeval refresh_tstamp;                /* Time-stamp of last refresh */
  struct i_list_s *refreshcb_list;              /* List of refresh callbacks */

  /* SNMP Agent */
  void *agent_rowptr;                           /* SNMP Row */
  int agent_rowindex;                           /* Our Index */
  int agent_nextchildindex;                     /* Next index to give to a child */

  /*
   * Customer-specific values 
   */

  int licensed;                                 /* 0=Not-Licensed / 1=Licensed */
  char *baseurl_str;                            /* Base HTTP URL for customer */
  
} i_customer;

/* customer.c */

i_customer* i_customer_create (char *name_str, char *desc_str, char *baseurl_str);
void i_customer_free (void *custptr);
i_customer* i_customer_duplicate (i_customer *cust);
char* i_customer_data (i_customer *cust, int *datasizeptr);
i_customer* i_customer_struct (char *data, int datasize);

int i_customer_register (i_resource *self, struct i_entity_s *parent, i_customer *cust);
int i_customer_deregister (i_resource *self, i_customer *cust);

/* FIX Possibly defunct */
struct i_resource_address_s* i_customer_get_resaddr_by_id (i_resource *self, char *id_str);
