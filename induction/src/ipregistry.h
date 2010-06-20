/* ipregistry.c */
long i_ipregistry_register (i_resource *self, struct i_object_s *ip);
long i_ipregistry_setnetmask (i_resource *self, struct i_object_s *ip, struct i_metric_s *netmask);
long i_ipregistry_setiface (i_resource *self, struct i_object_s *ip, struct i_object_s *iface);
long i_ipregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, struct i_object_s *ip);

