#include <netinet/in.h>
#include <arpa/inet.h>

/* dhcp.c */

#define DHCPCONF_FILE "dhcp.conf"

int l_dhcp_enabled ();
int l_dhcp_enable (i_resource *self);
int l_dhcp_disable (i_resource *self);

/* dhcp_daemon.c */

int l_dhcp_daemon_start (i_resource *self);
int l_dhcp_daemon_stop (i_resource *self);
int l_dhcp_daemon_restart (i_resource *self);
int l_dhcp_daemon_update (i_resource *self);
int l_dhcp_daemon_pid (i_resource *self, pid_t *pidptr);
int l_dhcp_daemon_writeconf (i_resource *self);

/* dhcp_subnet.c */

typedef struct l_dhcp_subnet_s
{
  long id;
  
  char *network_str;
  char *mask_str;
  char *router_str;
  char *domain_str;
  char *options_str;

  struct in_addr network_addr;
  struct in_addr mask_addr;
  struct in_addr router_addr;

  struct i_list_s *range_list;
  struct i_site_s *site;
  
} l_dhcp_subnet;

l_dhcp_subnet* l_dhcp_subnet_create ();
void l_dhcp_subnet_free (void *subptr);
i_list* l_dhcp_subnet_list ();
int l_dhcp_subnet_list_free ();
long l_dhcp_subnet_id_assign ();
int l_dhcp_subnet_id_setcurrent (long cur_id);
l_dhcp_subnet* l_dhcp_subnet_get (i_resource *self, long id);
l_dhcp_subnet* l_dhcp_subnet_add (i_resource *self, char *site_name, char *network_str, char *mask_str, char *router_str, char *domain_str, char *options_str);
int l_dhcp_subnet_update (i_resource *self, l_dhcp_subnet *subnet);
int l_dhcp_subnet_remove (i_resource *self, l_dhcp_subnet *subnet);
int l_dhcp_subnet_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_dhcp_subnet_loadall (i_resource *self);

/* dhcp_range.c */

typedef struct l_dhcp_range_s
{
  long id;

  char *start_str;
  char *end_str;

  struct l_dhcp_subnet_s *subnet;
} l_dhcp_range;

l_dhcp_range* l_dhcp_range_create ();
void l_dhcp_range_free (void *rangeptr);
long l_dhcp_range_id_assign ();
int l_dhcp_range_id_setcurrent (long cur_id);
l_dhcp_range* l_dhcp_range_get (i_resource *self, l_dhcp_subnet *subnet, long id);
l_dhcp_range* l_dhcp_range_add (i_resource *self, l_dhcp_subnet *subnet, char *start_str, char *ent_str);
int l_dhcp_range_update (i_resource *self, l_dhcp_range *range);
int l_dhcp_range_remove (i_resource *self, l_dhcp_range *range);
int l_dhcp_range_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata);
int l_dhcp_range_loadall (i_resource *self, l_dhcp_subnet *subnet);


