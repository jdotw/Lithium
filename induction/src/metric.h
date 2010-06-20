/* Cement - Metric Entity */

typedef struct i_metric_s
{
  /*
   * CEMENT Entity Template Variables
   */

  /* Entity Identifiers */
  unsigned short ent_type;
  char *name_str;
  char *desc_str;
  struct i_object_s *obj;               /* Parent object */
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
  struct i_list_s *trg_list;                    /* List of triggers assigned to this metric */
  struct i_hashtable_s *trg_ht;                 /* Hashtable of child entities */
  int (*trg_list_sortfunc) ();                  /* External function for sorting the trg list */

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
  struct timeval refresh_tstamp;                /* Time-stamp of last refresh */ 
  struct i_list_s *refreshcb_list;              /* List of refresh callbacks */

  /* SNMP Agent */
  void *agent_rowptr;                           /* SNMP Row */
  int agent_rowindex;                           /* Our Index */
  int agent_nextchildindex;                     /* Next index to give to a child */

  /*
   * Metric-Specific Variables
   */

  /* Metric type */
  unsigned short met_type;              /* Type of metric */

  /* Value list */
  struct i_list_s *val_list;            /* List of recent values */
  unsigned long val_list_maxsize;       /* Maximum Length of recent value list */

  /* Value boundaries */
  struct i_metric_value_s *min_val;     /* Minimum possible value */
  struct i_metric_value_s *max_val;     /* Maximum possible value */

  /* Value period */
  unsigned short val_period;            /* Instantaneous / Day / Month / Year */
  
  /* Units */
  unsigned long alloc_unit;             /* The number of actual units referred to by the metric value */
  struct i_metric_s *alloc_unit_met;    /* A metric that defines the allocation units */
  char *unit_str;                       /* String representing the units for this metric */
  unsigned int kbase;                   /* k-base (1000 or 1024) */
  unsigned long div_by;                 /* Value to divide raw metric by */
  float multiply_by;                    /* Multiplier (float value for use with ModB) */

  /* External Display string func */
  char* (*valstr_func) ();              /* External string to generate a display string for the metric */

  /* Enumerated Strings */
  struct i_list_s *enumstr_list;

  /* Recording Configuration */
  short int record_method;              /* SQL/RRD recording */
  short int record_defaultflag;         /* Default recording flag */
  short int record_enabled;             /* 0 = Not record, 1 = Recorded */
  short int record_priority;            /* 0=Immediate 1=Queue 2=Drop */

  /* Time stamps */
  struct timeval lrefresh_tv;           /* Time the last refresh was performed */
  struct timeval lnormal_tv;            /* Time when the value was last in a normal state */
  struct timeval lvalchange_tv;         /* Time when the last value change for the metric occurred */
  struct timeval lstatechange_tv;       /* Time when the last state change for the metric occurred */

  /* RRD Validity variables */
  time_t rrd_month_vsec;                /* Earliest time a new month rrd file needs to be created */
  time_t rrd_year_vsec;                 /* Earliest time a new year rrd file needs to be created */

  /* RRD value queues */
  struct timeval rrd_5min_tstamp;       /* Last time a 5-min update was done */ 
  struct timeval rrd_6hr_tstamp;        /* Last time a 6-hour update was done */
  char *rrd_update_args;                /* Pending RRDtool Update String */
  unsigned int rrd_update_count;        /* Count of pending updates */ 

  /* Summary */
  short int summary_flag;               /* 1 = Always show in metric summary */
  float aggregate_delta;                /* The aggregate delta for recent (1hr) values */
  
  /* Incident */
  struct i_incident_s *op_inc;          /* Operational state incident */

} i_metric;

typedef struct i_metric_value_s
{
  /* Real-time Value */
  long integer;                         /* Integer value (signed 32bit) */
  unsigned long count;                  /* Counter value (wrapping unsigned 32bit) */
  unsigned long gauge;                  /* Gauge value (non-wrapping unsigned 32bit) */
  unsigned long long count64;           /* Counter value (wrapping unsigned 64 bit) */
  unsigned long long gauge64;           /* Gauge value (wrapping unsigned 64 bit) */
  double flt;                           /* Floating point value */
  char *str;                            /* String value */
  void *oid;                            /* OID value */
  int oid_len;                          /* Length of OID */
  struct i_ip_s *ip;                    /* IP Address */
  struct timeval tv;                    /* Time value */ 
  char *data;                           /* Opaque data */
  int datasize;                         /* Opaque datasize */

  /* Metric-related real-time values */
  struct timeval tstamp;                /* Timestamp of value */
  
} i_metric_value;   /* Must be compatible with i_value */

/* Metric types KEEP IN SYNC WITH value.h */
#define METRIC_INTEGER 1                /* Signed 32bit integer value */
#define METRIC_COUNT 2                  /* Unsigned 32bit integer wrapping value */
#define METRIC_GAUGE 3                  /* Unsigned 32bit integer non-wrapping value */
#define METRIC_FLOAT 4                  /* Floating point value */
#define METRIC_STRING 5                 /* NULL terminated string value */
#define METRIC_OID 6                    /* OID value */
#define METRIC_IP 7                     /* IP Address */
#define METRIC_INTERVAL 8               /* A time interval in seconds */
#define METRIC_HEXSTRING 9              /* A hex string in XX:XX:XX:XX format */
#define METRIC_COUNT_HEX64 10           /* Unsigned 64bit integer wrapping value derived from hex string */
#define METRIC_GAUGE_HEX64 11           /* Unsigned 64bit integer non-wrapping value derived from hex string */
#define METRIC_COUNT64 12               /* Unsigned 64bit integer wrapping value */
#define METRIC_DATA 100                 /* Opaque data value */

/* Value Period flags */
#define VALPERIOD_INSTANT 0             /* 'Instantaneous' point-in-time value */
#define VALPERIOD_DAY 1                 /* Value refers a whole day */
#define VALPERIOD_MONTH 2               /* Value refers a whole month */
#define VALPERIOD_YEAR 3                /* Value refers a whole year */

/* Recording flags */
#define RECMETHOD_NONE 0         /* Dont record the metric */
#define RECMETHOD_RRD 1          /* Record metric in an RRD */
#define RECMETHOD_SQL 2          /* Record metric in an SQL database */
#define RECMETHOD_ALL 3          /* All record modes */

/* RRD Definitions */
#define METRIC_RRD_FILE "archive.rrd"
#define METRIC_RRD_DATASOURCE "default"

/* Graph periods */
#define GRAPHPERIOD_LAST48HR 1
#define GRAPHPERIOD_DAY 2
#define GRAPHPERIOD_WEEK 3
#define GRAPHPERIOD_MONTH 4
#define GRAPHPERIOD_YEAR 5
#define GRAPHPERIOD_CUSTOM 6

/* Graph sizes */
#define GRAPHSIZE_SMALL 1
#define GRAPHSIZE_MEDIUM 2
#define GRAPHSIZE_LARGE 3

/* Metric Defaults */
#define METRIC_DEF_MAXVAL 4 /* Default val_list_maxsize */

/* metric.c */
i_metric* i_metric_create (char *name_str, char *desc_str, unsigned short met_type);
void i_metric_free (void *metptr);
int i_metric_register (i_resource *self, struct i_object_s *obj, i_metric *met);
int i_metric_deregister (i_resource *self, i_metric *met);
char* i_metric_typestr (unsigned short type);

/* metric_value.c */
i_metric_value* i_metric_value_create ();
void i_metric_value_free (void *valptr);
i_metric_value* i_metric_value_duplicate (i_metric_value *val);
int i_metric_value_enqueue (i_resource *self, i_metric *met, i_metric_value *val);

/* metric_period.c */

int i_metric_period_validate (i_metric *met, time_t ref_sec, time_t now_sec);

/* metric_enumstr.c */

typedef struct i_metric_enumstr_s
{
  int val_int;
  char *str;
} i_metric_enumstr;

i_metric_enumstr* i_metric_enumstr_create ();
void i_metric_enumstr_free (void *estrptr);
int i_metric_enumstr_add (i_metric *met, int int_val, char *str);
char* i_metric_enumstr_eval (i_metric *met, int int_val);

/* metric_record.c */
int i_metric_record (i_resource *self, i_metric *met);

/* metric_record_rrd.c */
#define RRDQUEUE_5MIN 0
#define RRDQUEUE_6HR 1
int i_metric_record_rrd (i_resource *self, i_metric *met);
int i_metric_record_rrd_queued (i_resource *self, i_metric *met, unsigned short queueflag);

/* metric_record_sql.c */
typedef struct i_metric_record_sql_data_s
{
  int ent_type;
  int mday;
  int month;
  int year;
  char *valstr;
  double valflt;
  time_t tstamp_sec;
  char *tablestr;
  char *metnamestr;
  char *objnamestr;
} i_metric_record_sql_data;

i_metric_record_sql_data* i_metric_record_sql_data_create ();
void i_metric_record_sql_data_free (void *dataptr);
int i_metric_record_sql (i_resource *self, i_metric *met);
int i_metric_record_sql_updatecb ();
int i_metric_record_sql_insertcb ();

/* metric_load.c */
struct i_callback_s* i_metric_load (i_resource *self, struct i_metric_s *met, unsigned short period, time_t ref_sec, int (*cbfunc) (), void *passdata);
struct i_list_s* i_metric_load_sync (i_resource *self, struct i_metric_s *met, unsigned short period, time_t ref_sec);

/* metric_load_sql.c */
struct i_callback_s* i_metric_load_sql (i_resource *self, struct i_metric_s *met, unsigned short period, time_t ref_sec, int (*cbfunc) (), void *passdata);
int i_metric_load_sql_sqlcb ();
struct i_list_s* i_metric_load_sql_sync (i_resource *self, struct i_metric_s *met, unsigned short period, time_t ref_sec);

/* metric_util.c */
i_metric_value* i_metric_curval (i_metric *met);
struct i_callback_s* i_metric_histval (i_resource *self, struct i_metric_s *met, unsigned short period, time_t ref_sec, int (*cbfunc) (), void *passdata);
char* i_metric_valstr (i_metric *met, i_metric_value *val);
char* i_metric_valstr_raw (i_metric *met, i_metric_value *val);
int i_metric_valstr_set (i_metric *met, i_metric_value *val, char *str);
double i_metric_valflt (i_metric *met, i_metric_value *val);
int i_metric_valflt_set (i_metric *met, i_metric_value *val, double valflt);
int i_metric_valisnum (i_metric *met);
char* i_metric_lastupdatestr (i_metric *met);

/* metric_acrate.c */
typedef struct i_metric_acrate_data_s
{
  struct i_metric_s *countmet;
  struct i_callback_s *countrefcb;
  short int modflag;
  struct i_metric_s *met;
} i_metric_acrate_data;

/* Modifiers */
#define ACRATE_MOD_BYTETOBIT 1

struct i_metric_s* i_metric_acrate_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, char *unit_str, unsigned short recmethod, struct i_metric_s *countmet, short int modflag);
i_metric_acrate_data* i_metric_acrate_data_create ();
void i_metric_acrate_data_free (void *dataptr);
int i_metric_acrate_refcb (i_resource *self, i_metric *met, void *passdata);

/* metric_acrate_refresh.c */

int i_metric_acrate_refresh (i_resource *self, i_metric *met, int opcode);

/* metric_acpcent.c */

typedef struct i_metric_acpcent_data_s
{
  struct i_metric_s *gaugemet;
  struct i_metric_s *maxmet;
  struct i_callback_s *gaugerefcb;
  struct i_callback_s *maxrefcb;
  struct i_metric_s *met;
} i_metric_acpcent_data;

#define ACPCENT_REFCB_GAUGE 1
#define ACPCENT_REFCB_MAX 2
#define ACPCENT_REFCB_BOTH 3

i_metric* i_metric_acpcent_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, unsigned short recmethod, i_metric *gaugemet, i_metric *maxmet, unsigned short flags);
i_metric_acpcent_data* i_metric_acpcent_data_create ();
void i_metric_acpcent_data_free (void *dataptr);
int i_metric_acpcent_refcb (i_resource *self, i_metric *eithermet, void *passdata);

/* metric_acpcent_refresh.c */

int i_metric_acpcent_refresh (i_resource *self, i_metric *met, int opcode);

/* metric_acdiff.c */

typedef struct i_metric_acdiff_data_s
{
  struct i_metric_s *xmet;
  struct i_metric_s *ymet;
  struct i_callback_s *xrefcb;
  struct i_callback_s *yrefcb;
  struct i_metric_s *met;
} i_metric_acdiff_data;

#define ACDIFF_REFCB_XMET 1
#define ACDIFF_REFCB_YMET 2
#define ACDIFF_REFCB_BOTH 3

i_metric* i_metric_acdiff_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, unsigned short met_type, unsigned short recmethod, i_metric *xmet, i_metric *ymet, unsigned short flags);
i_metric_acdiff_data* i_metric_acdiff_data_create ();
void i_metric_acdiff_data_free (void *dataptr);
int i_metric_acdiff_refcb (i_resource *self, i_metric *met, void *passdata);

/* metric_acdiff_refresh.c */

int i_metric_acdiff_refresh (i_resource *self, i_metric *met, int opcode);

/* metric_acsum.c */

typedef struct i_metric_acsum_data_s
{
  struct i_list_s *met_list;
  struct i_list_s *cb_list;
  struct i_metric_s *met;
} i_metric_acsum_data;

#define ACSUM_REFCB_XMET 1
#define ACSUM_REFCB_YMET 2
#define ACSUM_REFCB_BOTH 3
#define ACSUM_REFMETHOD_PARENT 4

i_metric* i_metric_acsum_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, unsigned short met_type, unsigned short recmethod, i_metric *xmet, i_metric *ymet, unsigned short flags);
int i_metric_acsum_addmet (i_metric *summet, i_metric *xmet, unsigned short refcb_flag);
int i_metric_acsum_removemet (i_metric *summet, i_metric *xmet);
i_metric_acsum_data* i_metric_acsum_data_create ();
void i_metric_acsum_data_free (void *dataptr);
int i_metric_acsum_refcb (i_resource *self, i_metric *derivmet, void *passdata);

/* metric_acsum_refresh.c */

int i_metric_acsum_refresh (i_resource *self, i_metric *summet, int opcode);

/* metric_acmult.c */

typedef struct i_metric_acmult_data_s
{  
  struct i_list_s *met_list;
  struct i_list_s *cb_list; 
  struct i_metric_s *met;
} i_metric_acmult_data;

#define ACMULT_REFCB_XMET 1
#define ACMULT_REFCB_YMET 2
#define ACMULT_REFCB_BOTH 3

i_metric* i_metric_acmult_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, unsigned short met_type, unsigned short recmethod, i_metric *xmet, i_metric *ymet, unsigned short flags);
int i_metric_acmult_addmet (i_metric *mult, i_metric *xmet, unsigned short refcb_flag);
i_metric_acmult_data* i_metric_acmult_data_create ();
void i_metric_acmult_data_free (void *dataptr);
int i_metric_acmult_refcb (i_resource *self, i_metric *derivmet, void *passdata);

/* metric_acmult_refresh.c */

int i_metric_acmult_refresh (i_resource *self, i_metric *multmet, int opcode);

/* metric_acvol.c */

typedef struct i_metric_acvol_data_s
{
  struct i_metric_s *countmet;
  struct i_metric_s *uptimemet;
  struct i_metric_s *discontmet;
  struct i_metric_value_s *lastcurcountval;
  struct i_callback_s *countrefcb;
  struct i_metric_s *met;

  unsigned long remain;
} i_metric_acvol_data;

i_metric* i_metric_acvol_create (i_resource *self, struct i_object_s *obj, char *name_str, char *desc_str, char *unit_str, unsigned long alloc_unit, unsigned short recmethod, i_metric *countmet, i_metric *uptimemet, i_metric *discontmet, short int val_period);
i_metric_acvol_data* i_metric_acvol_data_create ();
void i_metric_acvol_data_free (void *dataptr);
int i_metric_acvol_refcb (i_resource *self, i_metric *countmet, void *passdata);
int i_metric_acvol_process_history (i_resource *self, i_metric *met, struct i_list_s *val_list);

/* metric_acvol_refresh.c */

int i_metric_acvol_refresh (i_resource *self, i_metric *volmet, int opcode);

/* metric_graph.c */

#define METGRAPH_SHOWTRG 1

struct i_rrdtool_cmd_s* i_metric_graph_render (i_resource *self, i_metric *met, int graphsize, time_t ref_sec, int period, int (*cbfunc) (), void *passdata);
char* i_metric_graph_periodstr (time_t ref_sec, int period);

/* metric_cgraph.c */

/* Single Metric Graph size defaults */
#define METRIC_GRAPH_WIDTH_SMALL 610        /* Single metric default width (small)   */
#define METRIC_GRAPH_WIDTH_MEDIUM 610       /* Single metric default width (medium)  */
#define METRIC_GRAPH_WIDTH_LARGE 610        /* Single metric default width (large)   */
#define METRIC_GRAPH_HEIGHT_SMALL 35        /* Single metric default height (small)  */
#define METRIC_GRAPH_HEIGHT_MEDIUM 60       /* Single metric default height (medium) */
#define METRIC_GRAPH_HEIGHT_LARGE 500       /* Single metric default height (large)  */

/* Combined Metric Graph size defaults */
#define METRIC_CGRAPH_WIDTH_SMALL 610        /* Combined metric default width (small)   */
#define METRIC_CGRAPH_WIDTH_MEDIUM 610       /* Combined metric default width (medium)  */
#define METRIC_CGRAPH_WIDTH_LARGE 610        /* Combined metric default width (large)   */
#define METRIC_CGRAPH_HEIGHT_SMALL 50        /* Combined metric default height (small)  */
#define METRIC_CGRAPH_HEIGHT_MEDIUM 80       /* Combined metric default height (medium) */
#define METRIC_CGRAPH_HEIGHT_LARGE 500       /* Combined metric default height (large)  */

typedef struct i_metric_cgraph_s
{
  struct i_object_s *obj;
  char *name_str;
  char *title_str;
  char *render_str;
  char *yaxis_str;
  unsigned int kbase;     /* 1000 (default) or 1024 */
  int upper_limit;
  unsigned short upper_limit_set;
  int lower_limit;
  unsigned short lower_limit_set;
  unsigned short rigid;
  int width_small;
  int width_medium;
  int width_large;
  int height_small;
  int height_medium;
  int height_large;
  struct i_list_s *met_list;
} i_metric_cgraph;

i_metric_cgraph* i_metric_cgraph_create (struct i_object_s *obj, char *name_str, char *yaxis_str);
void i_metric_cgraph_free (void *cgraphptr);
i_metric_cgraph* i_metric_cgraph_get (struct i_object_s *obj, char *name_str);
struct i_rrdtool_cmd_s* i_metric_cgraph_render (i_resource *self, i_metric_cgraph *cgraph, int graphsize, time_t ref_sec, int period, int (*cbfunc) (), void *passdata);
char* i_metric_cgraph_defs (i_resource *self, i_metric *met, char *rrdfullpath);


