typedef struct i_threshold_s
{
  char *name;
  char *object_desc;
  char *value_desc;

  float alert_value;
  float alert_default;
  float critical_value;
  float critical_default;
  float tolerance_percent;
  float tolerance_default;

  float last_recorded;

  struct i_threshold_section_s *section;
} i_threshold;

typedef struct i_threshold_section_s
{
  char *name;
  char *desc;
  struct i_list_s *thresholds;
} i_threshold_section;

/* threshold.c */

int i_threshold_enable (i_resource *self);
int i_threshold_disable (i_resource *self);
i_threshold* i_threshold_create ();
void i_threshold_free (void *tholdptr);
i_threshold* i_threshold_register (i_resource *self, i_threshold_section *section, char *name, char *object_desc, char *value_desc, float alert_default, float critical_default, float tolerance_default);
int i_threshold_deregister (i_threshold_section *section, char *name);
i_threshold* i_threshold_get (i_resource *self, i_threshold_section *section, char *name);

/* threshold_section.c */

int i_threshold_section_enable (i_resource *self);
int i_threshold_section_disable (i_resource *self);
struct i_list_s *i_threshold_section_list ();
i_threshold_section* i_threshold_section_create ();
void i_threshold_section_free (void *sectionptr);
i_threshold_section* i_threshold_section_register (i_resource *self, char *name, char *desc);
int i_threshold_section_deregister (char *name);
i_threshold_section* i_threshold_section_get (i_resource *self, char *name);

/* threshold_record.c */

int i_threshold_record (i_resource *self, i_threshold *thold, float value);

/* threshold_sql.c */

int i_threshold_sql_enable (i_resource *self);
int i_threshold_sql_disable (i_resource *self);
i_threshold* i_threshold_sql_get (i_resource *self, i_threshold_section *section, char *name);
int i_threshold_sql_put (i_resource *self, i_threshold *thold);
int i_threshold_sql_update (i_resource *self, i_threshold *thold);
int i_threshold_sql_del (i_resource *self, i_threshold *thold);

