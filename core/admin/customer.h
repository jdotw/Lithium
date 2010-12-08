/* customer.c */

#define CUSTOMER_MODULE "customer.so"

i_list* l_customer_list ();
i_list* l_customer_list_create ();
int l_customer_list_free ();
i_customer* l_customer_get (i_resource *self, char *name_str);
struct i_customer_s* l_customer_add (i_resource *self, char *name_str, char *desc_str, char *baseurl_str);
int l_customer_update (i_resource *self, i_customer *cust);
int l_customer_remove (i_resource *self, i_customer *cust);
int l_customer_sqlcb ();
int l_customer_loadall (i_resource *self);
int l_customer_loadall_retry (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_customer_initsql (i_resource *self);
int l_customer_initsql_retry (i_resource *self, struct i_timer_s *timer, void *passdata);

/* customer_form_list.c */

int form_customer_list (i_resource *self, struct i_form_reqdata_s *reqdata);

/* customer_sort.c */

int l_customer_sort_name (void *curptr, void *nextptr);

/* customer_resource.c */

int l_customer_res_restart (i_resource *self, i_customer *cust);
int l_customer_res_restart_spawncb (i_resource *self, i_resource_address *addr, void *passdata);
int l_customer_res_destroy (i_resource *self, i_customer *cust);

