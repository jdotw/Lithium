/* infstat.c */

int l_infstat_enabled ();
int l_infstat_enable (i_resource *self);
int l_infstat_disable (i_resource *self);

/* infstat_cat.c */

typedef struct l_infstat_cat_s
{
  char *name_str;
  char *desc_str;

  i_list *issue_list;
} l_infstat_cat;

i_list* l_infstat_cat_list ();
int l_infstat_cat_load_inprogress ();
l_infstat_cat* l_infstat_cat_create ();
void l_infstat_cat_free (void *catptr);
int l_infstat_cat_load (i_resource *self);
int l_infstat_cat_load_callback (i_resource *self, int result, void *passdata);
int l_infstat_cat_add (i_resource *self, l_infstat_cat *cat);
int l_infstat_cat_update (i_resource *self, l_infstat_cat *cat);
int l_infstat_cat_remove (i_resource *self, l_infstat_cat *cat);
l_infstat_cat* l_infstat_cat_get (i_resource *self, char *name_str);
int l_infstat_cat_listsort_severity (void *curptr, void *nextptr);
int l_infstat_cat_listsort_name (void *cur, void *next);
int l_infstat_cat_listsort_desc (void *cur, void *next);
char* l_infstat_cat_headline_colorstr (l_infstat_cat *cat);
char* l_infstat_cat_headline_str (l_infstat_cat *cat);

/* infstat_cat_sql.c */

int l_infstat_cat_sql_init (i_resource *self);
int l_infstat_cat_sql_load (i_resource *self, i_list *list, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata);
int l_infstat_cat_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_infstat_cat_sql_load_issuecb (i_resource *self, int result, void *passdata);
int l_infstat_cat_sql_add (i_resource *self, l_infstat_cat *cat);
int l_infstat_cat_sql_update (i_resource *self, l_infstat_cat *cat);
int l_infstat_cat_sql_delete (i_resource *self, l_infstat_cat *cat);
int l_infstat_cat_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* infstat_cat_formsection.c */

int l_infstat_cat_formsection (i_resource *self, i_form_reqdata *reqdata, l_infstat_cat *cat);

/* infstat_issue.c */

typedef struct l_infstat_issue_s
{
  /* Basic Info */
  long id;                /* ID number */
  char *headline_str;     /* One-liner of the issue */
  char *desc_str;         /* Long description */
  int severity;           /* 0=trivial 1=impaired 2=offline */
  int affects;            /* 0=some 1=many 2=all */
  int state;              /* 0=close 1=open */
  char *closenote_str;    /* Closing note */

  /* Time Stamps */
  time_t start_sec;       /* Start of the issue */
  time_t etr_sec;         /* Estimated time to restore */
  time_t end_sec;         /* End of the issue */

  /* Advanced info */
  i_list *affres_list;          /* Affected resource */
  i_list *symptom_list;         /* Symptoms */
  i_list *event_list;           /* Events related to this issue */
  i_list *log_list;             /* List of activitylog-style entries */
  i_list *adminusername_list;   /* Admin user list */
  i_list *techusername_list;    /* Technical user list */

  /* Related category */
  l_infstat_cat *cat;
} l_infstat_issue; 

#define INFSTAT_SEV_NORMAL 0
#define INFSTAT_SEV_TRIVIAL 1
#define INFSTAT_SEV_IMPAIRED 2
#define INFSTAT_SEV_OFFLINE 3

#define INFSTAT_AFFECTS_NONE 0
#define INFSTAT_AFFECTS_SOME 1
#define INFSTAT_AFFECTS_MANY 2
#define INFSTAT_AFFECTS_ALL 3

#define INFSTAT_STATE_CLOSED 0
#define INFSTAT_STATE_OPEN 1

unsigned long l_infstat_issue_count_normal ();
int l_infstat_issue_count_normal_dec ();
int l_infstat_issue_count_normal_inc ();
unsigned long l_infstat_issue_count_trivial ();
int l_infstat_issue_count_trivial_dec ();
int l_infstat_issue_count_trivial_inc ();
unsigned long l_infstat_issue_count_impaired ();
int l_infstat_issue_count_impaired_dec ();
int l_infstat_issue_count_impaired_inc ();
unsigned long l_infstat_issue_count_offline ();
int l_infstat_issue_count_offline_dec ();
int l_infstat_issue_count_offline_inc ();
l_infstat_issue* l_infstat_issue_create ();
void l_infstat_issue_free (void *issueptr);
int l_infstat_issue_add (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue);
int l_infstat_issue_update (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue);
int l_infstat_issue_close (i_resource *self, l_infstat_cat *cat, l_infstat_issue *issue);
l_infstat_issue* l_infstat_issue_get (i_resource *self, l_infstat_cat *cat, long id);
int l_infstat_issue_listsort_severity (void *curptr, void *nextptr);
char* l_infstat_issue_severity_str (int severity);
char* l_infstat_issue_affects_str (int affects);
long l_infstat_issue_nextid ();
int l_infstat_issue_nextid_set (long id);
int l_infstat_issue_nextid_increment ();

/* infstat_issue_sql.c */

int l_infstat_issue_sql_init (i_resource *self);
int l_infstat_issue_sql_load (i_resource *self, l_infstat_cat *cat, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata);
int l_infstat_issue_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_infstat_issue_sql_add (i_resource *self, l_infstat_issue *issue);
int l_infstat_issue_sql_update (i_resource *self, l_infstat_issue *issue);
int l_infstat_issue_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);

/* infstat_form_adminlinks */

int l_infstat_form_adminlinks (i_resource *self, i_form_reqdata *reqdata);

/* infstat_form_main.c */

int form_infstat_main (i_resource *self, i_form_reqdata *reqdata);
