/* xsanregistry.c */
long i_xsanregistry_register (i_resource *self, int type, struct i_object_s *vol);
long i_xsanregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, struct i_object_s *vol);

#define XSAN_TYPE_CONTROLLER 1
#define XSAN_TYPE_CLIENT 2

