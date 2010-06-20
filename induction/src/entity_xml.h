/* entity_xml.h */
xmlNodePtr i_entity_xml (struct i_entity_s *ent, unsigned short flags, time_t sync_version);
xmlNodePtr i_entity_descriptor_xml (struct i_entity_descriptor_s *entdesc);
struct i_entity_descriptor_s* i_entity_descriptor_fromxml ();

