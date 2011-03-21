/* site.c */

i_site* l_site_add (i_resource *self, char *name_str, char *desc_str, char *addr1_str, char *addr2_str, char *addr3_str, char *suburb_str, char *state_str, char *postcode_str, char *country_str, double longitude, double latitude);
int l_site_update (i_resource *self, i_site *site);
int l_site_remove (i_resource *self, i_site *site);
int l_site_sqlcb ();
int l_site_loadall (i_resource *self);
int l_site_initsql (i_resource *self);

/* site_sort.c */

int l_site_sortfunc_suburb (void *curptr, void *nextptr);
int l_site_sortfunc_name (void *curptr, void *nextptr);
int l_site_sortfunc_addr1 (void *curptr, void *nextptr);

/* site_form_list.c */

int form_site_list (i_resource *self, struct i_form_reqdata_s *reqdata);
