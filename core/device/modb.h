typedef struct l_modb_metric_h
{
  /* Metric template */
  char *name_str;
  char *desc_str;
  char *oid_str;
  int type;         /* 0=SNMP 1=AC_Rate 2=AC_Pcent */
  int val_type;     /* Value type */
  int record_method;
  int summary_flag;
  char *xmet_desc;
  char *ymet_desc;
  char *unit_str;
  float multiply_by;
  int kbase;

  /* Rate Metric */
  char *countermet_desc;

  /* Percent Metric */
  char *gaugemet_desc;
  char *maxmet_desc;

  /* Enums */
  i_list *enum_list;

} l_modb_metric;

/* modb.c */
int l_modb_init (i_resource *self);

/* modb_metric.c */
l_modb_metric* l_modb_metric_create ();
void l_modb_metric_free (void *metptr);

/* modb_parse.c */
int l_modb_parse_container (i_resource *self, xmlDocPtr doc, xmlNodePtr container_node);
int l_modb_parse_triggerset (i_resource *self, xmlDocPtr doc, xmlNodePtr container_node, struct i_container_s *cnt);
int l_modb_parse_metric (i_resource *self, xmlDocPtr doc, xmlNodePtr metric_node, struct i_container_s *cnt);
int l_modb_parse_metric_enum (i_resource *self, xmlDocPtr doc, xmlNodePtr enum_node, l_modb_metric *met);

/* modb_objfact.c */
int l_modb_objfact_fab ();
struct i_metric_s* l_modb_objfact_fab_met (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, l_modb_metric *met_template, char *index_oidstr);
int l_modb_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_modb_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* modb_cntform.c */
int l_modb_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_modb_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);
int l_modb_objform_hist (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);  

