/* entity_xml.h */
int i_entity_xmlsync_enable (i_resource *self, i_entity *entity, time_t refresh_interval, int (*delegate) (i_resource *self, int operation, i_entity *entity));
int i_entity_xmlsync_disable (i_resource *self, struct i_entity_s *entity);

#define XMLSYNC_SYNC 0        /* Normal refresh */
#define XMLSYNC_NEW 1         /* New entity added */
#define XMLSYNC_OBSOLETE 2    /* Obsolete Entity */
#define XMLSYNC_NOCHANGE 3    /* No change in the entity */
