typedef struct l_bonjour_browser_s
{
  /* Info */
  char *service;
  char *desc;

  /* DNS-SD */
  DNSServiceRef sdref;
  i_socket *sock;
  i_socket_callback *sockcb;

  /* Discovered services */
  struct i_list_s *service_list;
} l_bonjour_browser;

typedef struct l_bonjour_resolver_s
{
  /* Info */
  char *name;
  char *regtype;
  char *domain;
  
  /* DNS-SD */
  DNSServiceRef sdref;
  i_socket *sock;
  i_socket_callback *sockcb;

  /* Service */
  struct l_bonjour_service_s *service;

} l_bonjour_resolver;

typedef struct l_bonjour_service_s
{
  /* Browsed */
  char *name;
  char *regtype;
  char *replydomain;

  /* Resolved */
  char *fullname;
  char *hosttarget;
  uint16_t port;

  /* FLags */
  int current;

  /* IP */
  char *ip;

  /* Resolver */
  l_bonjour_resolver *resolver;

} l_bonjour_service;

struct i_list_s* l_bonjour_browser_list ();

int l_bonjour_enable (i_resource *self);

int l_bonjour_browse (i_resource *self, char *service, char *desc);

int l_bonjour_browse_sockcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_bonjour_resolve_sockcb (i_resource *self, struct i_socket_s *sock, void *passdata);

void l_bonjour_browsecb (DNSServiceRef sdref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType error, const char *service, const char *regtype, const char *replydomain, void *context);
void l_bonjour_resolvecb (DNSServiceRef sdref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType error, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txt, void *context);

l_bonjour_browser* l_bonjour_browser_create ();
void l_bonjour_browser_free (void *browseptr);

l_bonjour_resolver* l_bonjour_resolver_create ();
void l_bonjour_resolver_free (void *resptr);

l_bonjour_service* l_bonjour_service_create ();
void l_bonjour_service_free (void *serviceptr);


