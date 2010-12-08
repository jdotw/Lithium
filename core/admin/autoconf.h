int l_autoconf_generate_customer (i_resource *self, i_customer *cust);
int l_autoconf_generate_deployment (i_resource *self, i_list *cust_list);
void l_autoconf_writestart (int fd);
void l_autoconf_writeend (int fd);
void l_autoconf_writecust (int fd, i_customer *cust);

