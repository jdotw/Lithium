/*
 * Cement - Common struct/declarations
 */

/* Refresh methods */
#define REFMETHOD_PARENT 0          /* Entity will automatically refreshed by the object */
#define REFMETHOD_NONE 1            /* Entity will not be automatically refreshed */
#define REFMETHOD_TIMER 2           /* Entity will be refreshed by its own timer / func*/
#define REFMETHOD_EXTERNAL 3        /* Entity is refeshed by its own func triggered externally */
#define REFMETHOD_ALL 3             /* All methods */

/* Refresh states */
#define REFSTATE_IDLE 0            /* No refresh in progress */
#define REFSTATE_BUSY 1            /* Refresh in progress */
#define REFSTATE_ALL 1             /* All states */

/* Refresh opcodes */
#define REFOP_REFRESH 1             /* Request a refresh from the refresh_func */
#define REFOP_COLLISION 2           /* Handle a collision */
#define REFOP_TERMINATE 4           /* Terminate any current refresh operation */
#define REFOP_CLEANDATA 8           /* Cleanup any refresh data */
#define REFOP_ALL 15                /* All ops */

/* Refresh flags */
#define REFFLAG_AUTO 64             /* Automatic refresh (default) */
#define REFFLAG_MANUAL 128          /* Manual refresh */
#define REFFLAG_ALL 192             /* All flags */

/* Refresh results */
#define REFRESULT_UNKNOWN 0         /* Unknown refresh state */
#define REFRESULT_OK 1              /* All entities refreshed */
#define REFRESULT_PARTIAL_FAIL 2    /* A partial failure occurred */
#define REFRESULT_TOTAL_FAIL 3      /* A total failure occurred */
#define REFRESULT_TIMEOUT 4         /* A timeout occurred */

/* Refresh defaults */
#define REFDEFAULT_MAXCOLLS 3       /* 3 collissions may occur until the refresh is terminate */
#define REFDEFAULT_REFINTSEC 300    /* 300 second (5minute) default refresh interval */
#define REFDEFAULT_MAXBACKOFF 600   /* 600 second (10minute) maximum backed-off refresh interval */

/* Entity Operational states */

/* 'Blue' */
#define ENTSTATE_TESTING -2	        /* 'Blue' In Testing */
#define ENTSTATE_OUTOFSERVICE -3	/* 'Grey/None' Out of service */

/* 'Grey' */
#define ENTSTATE_UNKNOWN -1	        /* The default opstate as of Core 4.8.7 */

/* 'Green' */
#define ENTSTATE_GREEN 0
#define ENTSTATE_NORMAL 0           /* Applicable to all entities */

/* 'Yellow' */
#define ENTSTATE_YELLOW 1
#define ENTSTATE_WARNING 1          /* Applicable to metrics/triggers */
#define ENTSTATE_ATRISK 1           /* Applicable to device/containers/objects */

/* 'Orange */
#define ENTSTATE_ORANGE 2
#define ENTSTATE_IMPAIRED 2         /* Applicable to device/containers/objects */

/* 'Red' */
#define ENTSTATE_RED 3
#define ENTSTATE_FATAL 3            /* Applicable to metrics/triggers */
#define ENTSTATE_FAILED 3           /* Applicable to device/containers/objects */
#define ENTSTATE_CRITICAL 3         /* Applicable to metrics/triggers */

/* OR'd Flags */
//#define ENTSTATE_NONCURRENT 64      /* Applicable to all entities */
//#define ENTSTATE_DISABLED 128       /* Entity is disabled */

/* Entity Admin States */

#define ENTADMIN_ENABLED 0          /* Default state */
#define ENTADMIN_DISABLED 1         /* Disabled */
#define ENTADMIN_ALL 1              /* All bits to 1 for all possible entity states */

/* Filesystem flags */
#define FSFLAG_UNLINK 1             /* Unlink the entities file system root (and all files) when deregistered */

/* Entity Child Hashtable Size */
#define ENT_CHILDHT_SIZE 10         /* Size of child entity hashtable */

