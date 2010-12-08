/* lunregistry.c */
long i_lunregistry_register (i_resource *self, int type, struct i_object_s *obj, char *wwn_str, int lun);
long i_lunregistry_deregister (i_resource *self, i_resource_address *custaddr, i_resource_address *devaddr, struct i_object_s *obj);

