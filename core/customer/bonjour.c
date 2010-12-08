#include "config.h"
#ifdef HAVE_DNS_SD
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/socket.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <dns_sd.h>

#include "bonjour.h"

extern i_resource *global_self;

/* 
 * Bonjour Device Discovery
 */

static i_list *static_browser_list = NULL;

i_list* l_bonjour_browser_list ()
{ return static_browser_list; }

int l_bonjour_enable (i_resource *self)
{
  /* Create browser list */
  static_browser_list = i_list_create ();
  
  /* Enable bonjour device discovery */
  l_bonjour_browse (self, "_servermgr._tcp", "Mac OS X Server");
  l_bonjour_browse (self, "_airport._tcp", "Airport Base Station");
  l_bonjour_browse (self, "_xserveraid._tcp", "Xserve RAID");
  l_bonjour_browse (self, "_printer._tcp", "Printer");
  //l_bonjour_browse (self, "_workstation._tcp", "Workstation");
  /* Bonjour browsing for workstations has been disabled due to a
   * hang that is caused when resolving a workstation when there is 
   * large amount of bonjour traffic -- MLC Nov 2007 
   */

  return 0;
}



/* Browse */

int l_bonjour_browse (i_resource *self, char *service, char *desc)
{
  /* Create browser */
  l_bonjour_browser *browser = l_bonjour_browser_create ();
  browser->service = strdup (service);
  browser->desc = strdup (desc);

  /* Begin browse */
  DNSServiceErrorType error = DNSServiceBrowse (&browser->sdref, 0, 0, browser->service, NULL, l_bonjour_browsecb, browser);
  if (error == kDNSServiceErr_NoError)
  {
    /* Get socket, add socket callback */
    browser->sock->sockfd = DNSServiceRefSockFD (browser->sdref);
    i_socket_callback_add (self, SOCKET_CALLBACK_READ, browser->sock, l_bonjour_browse_sockcb, browser);
    i_list_enqueue (static_browser_list, browser);
  }
  else
  { 
    i_printf (1, "l_bonjour_browse failed to start browsing for %s (%i)", service, error);
    l_bonjour_browser_free (browser);
    return -1;
  }

  return 0;
}

/* Socket Callbacks */

int l_bonjour_browse_sockcb (i_resource *self, i_socket *sock, void *passdata)
{
  l_bonjour_browser *browser = (l_bonjour_browser *) passdata;
  DNSServiceProcessResult (browser->sdref);
  return 0;
}

int l_bonjour_resolve_sockcb (i_resource *self, i_socket *sock, void *passdata)
{
  l_bonjour_resolver *res = (l_bonjour_resolver *)passdata;
  DNSServiceProcessResult (res->sdref);
  return 0;
}

/* Bonjour Callbacks */

void l_bonjour_browsecb (DNSServiceRef sdref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType error, const char *name, const char *regtype, const char *replydomain, void *context)
{
  l_bonjour_browser *browser = context;

  /* Check for existing service */
  l_bonjour_service *service;
  for (i_list_move_head(browser->service_list); (service=i_list_restore(browser->service_list))!=NULL; i_list_move_next(browser->service_list))
  {
    if (strcmp(name, service->name)==0) break;
  }

  /* Check if the service exists */
  if (!service)
  {
    /* New service */
    service = l_bonjour_service_create ();
    if (name) 
    {
      char *bracket = strstr (name, "[");
      if (bracket) *bracket = '\0';
      service->name = strdup (name);
    }
    if (regtype) service->regtype = strdup (regtype);
    if (replydomain) service->replydomain = strdup (replydomain);
    
    i_list_enqueue (browser->service_list, service);
  }

  /* Set current flag */
  if (flags == 0)
  {
    service->current = 0; 
  }
  else 
  { 
    service->current = 1; 
  }

  /* Resolve it */
  if (!service->resolver)
  {
    /* Create resolver */
    service->resolver = l_bonjour_resolver_create ();
    service->resolver->name = strdup (name);
    service->resolver->regtype = strdup (regtype);
    service->resolver->domain = strdup (replydomain);

    /* Resolve */
    DNSServiceErrorType reserror = DNSServiceResolve (&service->resolver->sdref, 0, iface, name, regtype, replydomain, (DNSServiceResolveReply) l_bonjour_resolvecb, service);
    if (reserror == kDNSServiceErr_NoError)
    {
      service->resolver->sock->sockfd = DNSServiceRefSockFD (service->resolver->sdref);
      service->resolver->sockcb = i_socket_callback_add (global_self, SOCKET_CALLBACK_READ, service->resolver->sock, l_bonjour_resolve_sockcb, service->resolver);
    }
    else
    {
      i_printf (1, "l_bonjour_browsecb error occurred resolving service %s %s %i", name, regtype, reserror); 
    }
  }
}

void l_bonjour_resolvecb (DNSServiceRef sdref, DNSServiceFlags flags, uint32_t iface, DNSServiceErrorType error, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txt, void *context)
{
  l_bonjour_service *service = context;
  
  /* Set variables */
  if (service->fullname) { free (service->fullname); service->fullname = NULL; }
  if (fullname) 
  {
    char *bracket = strstr (fullname, "[");
    if (bracket) *bracket = '\0';
    service->fullname = strdup (fullname);
  }
  if (service->hosttarget) { free (service->hosttarget); service->hosttarget = NULL; }
  if (hosttarget) service->hosttarget = strdup (hosttarget);
  service->port = port;

  /* Set IP */
  if (service->ip) { free (service->ip); service->ip = NULL; }
  if (service->hosttarget)
  {
    struct hostent *hp = gethostbyname (service->hosttarget);
    if (hp && hp->h_addr_list[0] != NULL)
    { asprintf (&service->ip, "%s", inet_ntoa(*(struct in_addr*)(hp -> h_addr_list[0]))); }
  }

  /* Free Resolver */
  l_bonjour_resolver_free (service->resolver);
  service->resolver = NULL;
}

/* Struct Manipulation */

l_bonjour_resolver* l_bonjour_resolver_create ()
{
  l_bonjour_resolver *res = (l_bonjour_resolver *) malloc (sizeof(l_bonjour_resolver));
  res->name = NULL;
  res->regtype = NULL;
  res->domain = NULL;
  res->sock = i_socket_create ();
  res->sockcb = NULL;
  res->service = NULL;
  return res;
}

void l_bonjour_resolver_free (void *resptr)
{
  l_bonjour_resolver *res = resptr;
  DNSServiceRefDeallocate (res->sdref);
  if (res->sock)
  { res->sock->sockfd = 0; i_socket_free (res->sock); }
  if (res->sockcb)
  { i_socket_callback_remove (res->sockcb); }
  if (res->name) free (res->name);
  if (res->regtype) free (res->regtype);
  if (res->domain) free (res->domain);
  free (res);
}

l_bonjour_browser* l_bonjour_browser_create ()
{
  l_bonjour_browser *browser = (l_bonjour_browser *) malloc (sizeof(l_bonjour_browser));
  browser->service = NULL;
  browser->desc = NULL;
  browser->sock = i_socket_create ();
  browser->sockcb = NULL;
  browser->service_list = i_list_create ();
  return browser;
}

void l_bonjour_browser_free (void *browserptr)
{
  l_bonjour_browser *browser = browserptr;
  if (browser->service) free (browser->service);
  if (browser->desc) free (browser->desc);
  if (browser->sock)
  { browser->sock->sockfd = 0; i_socket_free (browser->sock); }
  if (browser->sockcb)
  { i_socket_callback_remove (browser->sockcb); }
  if (browser->service_list) i_list_free (browser->service_list);
  free (browser);
}

l_bonjour_service* l_bonjour_service_create ()
{
  l_bonjour_service *service = (l_bonjour_service *) malloc (sizeof(l_bonjour_service));
  memset (service, 0, sizeof(l_bonjour_service));
  return service;
}

void l_bonjour_service_free (void *serviceptr)
{
  l_bonjour_service *service = serviceptr;
  if (service->name) free (service->name);
  if (service->regtype) free (service->regtype);
  if (service->replydomain) free (service->replydomain);
  if (service->fullname) free (service->fullname);
  if (service->hosttarget) free (service->hosttarget);
  if (service->resolver) l_bonjour_resolver_free (service->resolver);
  free (service);
}

#endif
