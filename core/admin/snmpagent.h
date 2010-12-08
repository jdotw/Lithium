#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

netsnmp_tdata* l_snmpagent_device_table ();

int l_snmpagent_is_enabled ();
int l_snmpagent_enable (i_resource *self);

int l_snmpagent_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int l_snmpagent_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int l_snmpagent_fdset_snmptimeout_callback (i_resource *self, struct i_timer_s *timer, void *passdata);

int l_snmpagent_xmlsync_delegate (i_resource *self, int operation, struct i_entity_s *entity);
