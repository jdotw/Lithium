/* Cement - Device Entity */

typedef struct i_device_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_site_s *site;                        /* Parent Site */
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
  struct i_list_s *cnt_list;                    /* Containers */
  struct i_hashtable_s *cnt_ht;                 /* Hashtable of child entities */
  int (*cnt_list_sortfunc) ();                  /* External function for sorting the cnt list */

  /* Resource Data */
  struct i_resource_address_s *resaddr;            /* Address of device resource */
  struct i_resource_spawn_cbdata_s *spawn_op;   /* Spawn data for device resouce */
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
   * Device-Specific Variables
   */

  /* Device config */
  char *ip_str;
  char *lom_ip_str;
  int snmpversion;
  char *snmpcomm_str;
  char *snmpauthpass_str;
  char *snmpprivpass_str;
  int snmpauthmethod;
  int snmpprivenc;
  char *username_str;
  char *password_str;
  char *lom_username_str;
  char *lom_password_str;
  char *vendor_str;
  char *modb_xml_str;
  char *profile_str;
  long refresh_interval;
  int protocol;
  int icmp;
  int lithiumsnmp;
  int swrun;
  int nagios;
  int lom;
  int xsan;
  int minimum_action_severity;                  /* The minimum incident severeity for which an action should be fired */

  /* Licensed */
  int licensed;                                 /* 0=Not-Licensed / 1=Licensed */
  int limited;                                  /* 0=No / 1=Yes (No Xserve, No XSan) */

  /* Refresh */
  unsigned long refresh_count;                            /* Number of device-wide refrshes */
  int interval_warning_given;                   /* A warning has been given to increase the refresh interval */

  /* Device marking */
  int mark;                                     /* 0=Normal -2=In Testing (monitor, dont report) -3=Out of Service (do not monitor) */

} i_device;

/* Defaults */
#define DEVICE_DEFAULT_REFRESH_INTERVAL 300

i_device* i_device_create (char *name_str, char *desc_str, char *ip_str, char *lom_ip_str, char *snmpcomm_str, char *username_str, char *password_str, char *lom_username, char *lom_password, char *vendor_str, char *profile_str, long refresh_interval);
void i_device_free (void *devptr);
i_device* i_device_duplicate (i_device *dev);
char* i_device_data (i_device *dev, int *datasizeptr);
i_device* i_device_struct (char *data, int datasize);

int i_device_register (i_resource *self, struct i_site_s *site, i_device *dev);
int i_device_deregister (i_resource *self, i_device *dev);

i_device* i_device_form_to_struct (struct i_form_s *form);

char* i_device_struct_to_data (i_device *dev, int *datasizeptr);
i_device* i_device_data_to_struct (char *data, int datasize);

char* i_device_deviceid_from_form (struct i_form_s *form);

/* device_defaultsubsystems.c */

int i_device_defaultsubsystems_enable (i_resource *self);
int i_device_defaultsubsystems_disable (i_resource *self);
  
