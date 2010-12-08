/* i_entity.h - part of CEMent 
 *
 * The i_entity type is a generic struct
 * for defining one or more entities in cement
 */

typedef struct i_entity_s
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
  char *desc_esc_str;                           /* Escaped Description */

  /* Priority */
  unsigned long prio;                           /* Entity sort priority */

  /* Entity State */
  short adminstate;                    		    /* Administrative State of Entity */
  struct i_incident_s *admin_inc;               /* Administrative State Incident */
  short opstate;                       		    /* Operational State of Entity */
  unsigned short registered;                    /* 0=Not Registered 1=Registered */
  unsigned short hidden;                        /* Whether to show via xml or web */
  unsigned short authorative;                   /* Whether or not the local resource is authorative for this entity */
  time_t version;                               /* Version of entity (timestamp) */

  /* Filesystem Path */
  char *fsroot_str;                             /* The filesystem root dir for this entity */
  unsigned short fs_flags;                      /* Flags related to the file system */
  unsigned short fsroot_valid;                  /* 0=Path not created 1=Path created */
  time_t fsroot_month_vsec;                     /* Earliest time mkdir needs to be called to create the month path */
  time_t fsroot_year_vsec;                      /* Earliest time mkdir needs to be called to create the year path */

  /* Child Entity List/HT */
  struct i_list_s *child_list;                  /* List of child entities */
  struct i_hashtable_s *child_ht;               /* Hashtable of child entities */
  int (*child_list_sortfunc) ();                /* External function for sorting the child list */

  /* Resource Data */
  struct i_resource_address_s *resaddr;         /* The resource address of the entity */
  struct i_resource_spawn_cbdata_s *spawn_op;   /* Data concerning the spawn of the entity's resource */
  struct i_form_s *config_form;                 /* FIX DEFUNCT */
  
  /* Forms / Navigation / XML */
  int (*navtree_func) ();                       /* Function to generate navigation tree */
  unsigned int navtree_expand;                  /* Navtree Expansion Flag */
  int (*mainform_func) ();                      /* A general 'main' form for this entity */
  int (*histform_func) ();                      /* A general 'historic' form for this entity */
  int (*xml_func) ();                           /* Entity-specific XML data function */

  /* XML Sync */
  struct i_timer_s *xmlsync_timer;              /* XML Sync refresh timer */
  int (*xmlsync_delegate) ();                   /* XML Sync Delegate */
  struct i_xml_request_s *xmlsync_req;		    /* XML Sync Request */
  long xmlsync_tag;                             /* XML Sync Refresh Tag */
  unsigned int xmlsync_collisions;              /* XML Sync Collision Count */
  time_t xmlsync_version;                /* XML Sync synced version of entity */

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
  unsigned short refresh_forcedterm;            /* Forced termination or normal termination */
  struct timeval refresh_tstamp;                /* Time-stamp of last refresh */ 
  struct i_list_s *refreshcb_list;              /* List of refresh callbacks */

  /* SNMP Agent */
  void *agent_rowptr;                           /* SNMP Row */
  int agent_rowindex;                           /* Our Index */
  int agent_nextchildindex;                     /* Next index to give to a child */

} i_entity;

typedef struct i_entity_refresh_data_s
{
  int opcode;
  struct timeval start_tv;
  struct timeval curstart_tv;
  struct i_list_s *child_list;
  struct i_entity_s *cur_child;
  unsigned long failcount;
} i_entity_refresh_data;

typedef struct i_entity_address_s 
{
  unsigned short type;
  char *cust_name;
  char *site_name;
  char *dev_name;
  char *cnt_name;
  char *obj_name;
  char *met_name;
  char *trg_name;
} i_entity_address;

typedef struct i_entity_descriptor_s
{
  /* Entity type */
  unsigned short type;

  /* Hierarchy of name_str */
  char *cust_name;
  char *site_name;
  char *dev_name;
  char *cnt_name;
  char *obj_name;
  char *met_name;
  char *trg_name;

  /* Hierarchy of desc_str */
  char *cust_desc;
  char *site_desc;
  char *dev_desc;
  char *cnt_desc;
  char *obj_desc;
  char *met_desc;
  char *trg_desc;

  /* Hierarchy of adminstate */
  short cust_adminstate;
  short site_adminstate;
  short dev_adminstate;
  short cnt_adminstate;
  short obj_adminstate;
  short met_adminstate;
  short trg_adminstate;

  /* Hierarchy of opstate */
  short cust_opstate;
  short site_opstate;
  short dev_opstate;
  short cnt_opstate;
  short obj_opstate;
  short met_opstate;
  short trg_opstate;

  /* Hierarchy of resource addresses */
  i_resource_address *cust_resaddr;
  i_resource_address *dev_resaddr;

  /* Entity's own values */
  char *name_str;
  char *desc_str;
  short adminstate;
  short opstate;
  i_resource_address *resaddr;

  /* Misc values */
  char *site_suburb;

} i_entity_descriptor;

/* Entity types */

#define ENT_CUSTOMER 1
#define ENT_SITE 2
#define ENT_DEVICE 3
#define ENT_CONTAINER 4
#define ENT_OBJECT 5
#define ENT_METRIC 6
#define ENT_TRIGGER 7

#define ENTITY (i_entity *)

/* entity.c */
i_entity* i_entity_create (char *name_str, char *desc_str, unsigned short ent_type, size_t datasize);
void i_entity_free (void *entptr);
char* i_entity_data (i_entity *ent, int *datasizeptr);
i_entity* i_entity_struct (char *data, int datasize);
i_entity* i_entity_duplicate (i_entity *ent);
char* i_entity_typestr (unsigned short type);
char* i_entity_opstatestr (short state);
char* i_entity_adminstatestr (short state);

/* entity_address.c */

i_entity_address* i_entity_address_create ();
void i_entity_address_free (void *addrptr);
i_entity_address* i_entity_address_duplicate (i_entity_address *entaddr);
char* i_entity_address_string (i_entity *ent, i_entity_address *entaddr);
i_entity_address* i_entity_address_struct (char *addrstr);
i_entity_address* i_entity_address_struct_static (i_entity *ent);
char* i_entity_address_url (i_entity *ent, i_entity_address *entaddr);
#define ENT_ADDR(addr) i_entity_address_struct_static (ENTITY(addr))    /* USE ONLY ON I_ENTITY STRUCTS */

/* entity_descriptor.c */

i_entity_descriptor* i_entity_descriptor_create ();
void i_entity_descriptor_free (void *descptr);
i_entity_descriptor* i_entity_descriptor_duplicate (i_entity_descriptor *entdesc);
char* i_entity_descriptor_data (i_entity_descriptor *desc, i_entity *ent, int *datasizeptr);
i_entity_descriptor* i_entity_descriptor_struct (char *data, int datasize);
i_entity_descriptor* i_entity_descriptor_struct_static (i_entity *ent);


/* entity_register.c */

int i_entity_register (i_resource *self, i_entity *parent, i_entity *ent);
int i_entity_deregister (i_resource *self, i_entity *ent);

/* entity_child.c */

i_entity* i_entity_child_get (i_entity *parent, char *childname_str);
int i_entity_child_sortfunc (void *curptr, void *nextptr);

/* entity_local.c */

i_entity* i_entity_local_get (i_resource *self, i_entity_address *addr);
i_entity* i_entity_local_get_str (i_resource *self, char *addrstr);

/* entity_refresh.c */

int i_entity_refresh (i_resource *self, i_entity *ent, int flags, int (*cbfunc) (), void *passdata);
int i_entity_refresh_timercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int i_entity_refresh_terminate (i_entity *ent);
int i_entity_refresh_cleandata (i_entity *ent);
int i_entity_refresh_collision (i_resource *self, i_entity *ent);
int i_entity_refresh_execcblist (i_resource *self, i_entity *ent);
char* i_entity_refresh_resultstr (unsigned short result);

/* entity_refreshcb.c */

struct i_callback_s* i_entity_refreshcb_add (struct i_entity_s *ent, int (*cbfunc) (), void *passdata);
int i_entity_refreshcb_remove (struct i_callback_s *cb);
int i_entity_refreshcb_execlist (i_resource *self, i_entity *ent);

/* entity_refresh_children */

i_entity_refresh_data* i_entity_refresh_data_create ();
void i_entity_refresh_data_free (void *dataptr);
int i_entity_refresh_children (i_resource *self, i_entity *ent, int opcode);
int i_entity_refresh_children_refcb (i_resource *self, i_entity *child, void *passdata);
int i_entity_refresh_children_next (i_resource *self, i_entity *ent);
int i_entity_refresh_children_removechild (i_entity *parent, i_entity *child);
unsigned short i_entity_refresh_children_refresult (i_entity_refresh_data *data);
int i_entity_refresh_children_sortfunc (void *curptr, void *nextptr);

/* entity_refresh_config.c */

typedef struct i_entity_refresh_config_s
{
  int refresh_method;
  int refresh_int_sec;
  int refresh_maxcolls;
} i_entity_refresh_config;

i_entity_refresh_config* i_entity_refresh_config_create ();
void i_entity_refresh_config_free (void *configptr);
int i_entity_refresh_config_loadapply (i_resource *self, i_entity *ent, i_entity_refresh_config *defaults);
int i_entity_refresh_config_loadapply_loadcb (i_resource *self, i_entity *ent, i_entity_refresh_config *config, void *passdata);
int i_entity_refresh_config_apply (i_resource *self, i_entity *ent, i_entity_refresh_config *config);
int i_entity_refresh_config_load (i_resource *self, struct i_hierarchy_s *hier, i_entity *ent, int (*cbfunc) (i_resource *self, i_entity *ent, i_entity_refresh_config *config, void *passdata), void *passdata);
int i_entity_refresh_config_load_sqlcb ();
i_entity_refresh_config* i_entity_refresh_config_running (i_resource *self, i_entity *ent);

/* entity_path.c */
char* i_entity_path_extdev (i_resource *self, i_entity_descriptor *entdesc, time_t ref_sec, int flags);

/* Entity path creation flag */
#define ENTPATH_CREATE 64

/* Time-specific subdir path flags */
#define ENTPATH_ROOT 0
#define ENTPATH_YEAR_BIT 1
#define ENTPATH_MONTH_BIT 2
#define ENTPATH_PERIOD 3

/* Time-specific subdir combined path flags */
#define ENTPATH_YEAR 1
#define ENTPATH_MONTH 3

char* i_entity_path (i_resource *self, i_entity *ent, time_t ref_sec, int flags);

/* entity_util.c */

int i_entity_restype (unsigned short ent_type);
char* i_entity_statecolstr_fg (unsigned short state);
char* i_entity_statecolstr_bg (unsigned short state);

/* entity_xml.c */

#define ENTXML_TREE 1             /* Create a full tree of all child entities */
#define ENTXML_TREE_ONE_LEVEL 2   /* Current entity + children only */
#define ENTXML_TREE_AUTHORATIVE 4      /* Only include entities for which the resource is authorative */
#define ENTXML_TREE_SUMMARY 8      /* Full detail for authorative, summary for non-authorative */
#define ENTXML_SYNC 16            /* Observe sync timestamp */

