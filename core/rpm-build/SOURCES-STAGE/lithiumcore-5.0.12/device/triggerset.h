/* triggerset_form_apprule.c */
int form_triggerset_apprule (i_resource *self, struct i_form_reqdata_s *reqdata);
int l_triggerset_form_apprule_rulecb (i_resource *self, i_list *list, void *passdata);

/* triggerset_xml_disable_by_trigger.c */
int l_triggerset_xml_disable_by_trigger_process (i_resource *self, struct i_trigger_s *trg, struct i_entity_descriptor_s *entdesc);

/* triggerset_xml_reset_rules.c */
int l_triggerset_xml_reset_rules_process (i_resource *self, struct i_entity_s *ent);

/* triggerset_form_apprule_edit.c */
int l_triggerset_form_apprule_edit_rulecb (i_resource *self, struct i_triggerset_apprule_s *rule, void *passdata);

/* triggerset_form_valrule.c */
int form_triggerset_valrule (i_resource *self, struct i_form_reqdata_s *reqdata);
int l_triggerset_form_valrule_rulecb (i_resource *self, i_list *list, void *passdata);

/* triggerset_form_valrule_edit.c */
int l_triggerset_form_valrule_edit_rulecb (i_resource *self, struct i_triggerset_valrule_s *rule, void *passdata);

/* l_triggerset_xml_apprule_list.c */
int l_triggerset_xml_apprule_list_rulecb (i_resource *self, struct i_list_s *list, void *passdata);
