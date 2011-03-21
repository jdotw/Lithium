/* case_xml.c */
xmlNodePtr l_case_xml (struct l_case_s *cas);

/* case_xml_get.c */
int xml_case_get (i_resource *self, struct i_xml_request_s *req);
int l_case_xml_get_casecb (i_resource *self, struct l_case_s *cas, void *passdata);

/* case_xml_list.c */

typedef struct l_case_xml_list_reqdata
{
  struct i_xml_request_s *req;
  struct i_list_s *caselist;
} l_case_xml_list_reqdata;

int xml_case_list ();
int l_case_xml_list_casecb (i_resource *self, struct i_list_s *list, void *passdata);
int l_case_xml_list_entitycb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_xml_update.c */
int xml_case_update ();
int l_case_xml_update_casecb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_xml_reopen.c */
int l_case_xml_reopen_casecb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_logentry_xml.c */
xmlNodePtr l_case_logentry_xml (struct l_case_logentry_s *log);

/* case_logentry_xml_list.c */
int xml_case_logentry_list ();
int l_case_logentry_xml_list_logcb (i_resource *self, struct i_list_s *list, void *passdata);

/* case_logentry_xml_insert.c */
int xml_case_logentry_insert ();
