/* 
 * The event struct is used by the event manager as a
 * a container for related/associated incidents. I.e
 * incidents that are deemed to stem from the same
 * root cause
 */

typedef struct i_event_s
{
  long id;                              /* Assigned by SQL */

  int state;                            /* State of event (Open, Closed, etc) */
  int highest_status;                   /* The highest incident status within this event */
  
  struct i_list_s *inc_list;           /* List of incident contained in this event */
} i_event;

#define EVENT_STATE_UNKNOWN 0
#define EVENT_STATE_HOLDDOWN 1
#define EVENT_STATE_LIVE 2
#define EVENT_STATE_ARCHIVED 3

/*
 * The incident struct is used by any resource to report
 * an incident to the eventmgr and is then used by the
 * eventmgr to record the incidents.
 */

typedef struct i_event_inc_s
{
  /* Incident info */
  long id;                              /* Assigned by SQL */
  int type;                             /* Incident Type */

  /* Object info */
  int object_type;                      /* The type of the object */
  char* object_data;                    /* Object that this incident pertains to */
  int object_datasize;                  /* Size of object data */
  struct i_threshold_s *thold;          /* Threshold which has been exceeded */

  /* Hierarchy */
  struct i_hierarchy_s *hierarchy;      /* Hierarchy of reporting resource */

  /* 
   * Variables below are used by the eventmgr
   */
  int current_status;                   /* The current status of the incident */
  struct i_event_inc_occ_s *initial_occ;
  struct i_list_s *occ_list;
  
  struct i_event_s *event;              /* Parent Event */

} i_event_inc;

/*
 * The occurrence struct is used by the eventmgr
 * to record occurrences of incidents. For example, an occurrence
 * is recorded when an incident first occurrs, and then again for
 * each change in the incidents status
 */

typedef struct i_event_inc_occ_s
{
  long id;                                /* Assigned by SQL */
  
  int status_level;                       /* Status level at this occurence */
  char *hline_str;                      /* Headline of incident */
  char *desc_str;                       /* Long description */
  struct timeval timestamp;               /* Timestamp of this occurence */
  
  struct i_event_inc_s *inc;              /* Parent incident */
} i_event_inc_occ;

/* Event Related - event.c */

i_event* i_event_create ();
void i_event_free (void *eventptr);

char *i_event_state_string (i_event *event);

/* Event Incident Related - event_incident.c */

i_event_inc* i_event_inc_create ();
void i_event_inc_free (void *incidentptr);

/* Event Incident Occurence Related - event_incident.c */

i_event_inc_occ* i_event_inc_occ_create ();
void i_event_inc_occ_free (void *occptr);

