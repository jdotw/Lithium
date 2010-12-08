#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <errno.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/socket.h>

#include "snmp.h"
#include "icmp.h"

#define MAX_PACKET_SIZE 16384

static int static_seq = 0;

/* ECHO Sending */

l_icmp_echo_callback* l_icmp_echo_send (i_resource *self, char *host_str, size_t datasize, time_t timeout_sec, int (*callback_func) (i_resource *self, int result, l_icmp_echo_callback *callback, void *passdata), void *passdata)
{
  int num;
  int echo_datasize;
  struct icmp *echo;
  struct protoent *proto;
  struct hostent *host;
  struct sockaddr_in host_addr;
  l_icmp_echo_callback *callback;

  /* Check datasize */
  echo_datasize = ICMP_MINLEN + datasize;
  if (echo_datasize > MAX_PACKET_SIZE)
  { i_printf (1, "l_icmp_echo_send failed, packet size %i is greater than MAX_PACKET_SIZE (%i)", 
    echo_datasize, MAX_PACKET_SIZE); return NULL; }

  /* Get protoent for icmp */
  proto = getprotobyname ("icmp");
  if (!proto)
  { i_printf (1, "l_icmp_echo_send failed to get protocol entry for 'icmp'"); return NULL; }

  /* Find host */
  host = gethostbyname (host_str);
  if (!host)
  { i_printf (1, "l_icmp_echo_send failed to get host entry for %s", host_str); return NULL; }

  /* Copy host addr from hostent to host_addr */
  memcpy (&host_addr.sin_addr, host->h_addr_list[0], sizeof(host_addr.sin_addr));
  host_addr.sin_port = 0;
  host_addr.sin_family = AF_INET;

  /* Create callback */
  callback = l_icmp_echo_callback_create ();
  if (!callback)
  { i_printf (1, "l_icmp_echo_send failed to create callback struct"); return NULL; }
  callback->echo_datasize = echo_datasize;
  callback->callback_func = callback_func;
  callback->passdata = passdata;

  /* Create socket struct */
  callback->socket = i_socket_create ();
  if (!callback->socket)
  { i_printf (1, "l_icmp_echo_send failed to create socket struct"); return NULL; }

  /* Create socket */
  callback->socket->sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
  if (callback->socket->sockfd == -1)
  { i_printf (1, "l_icmp_echo_send failed to create sockfd (%s)", strerror(errno)); l_icmp_echo_callback_free (callback); return NULL; }

  /* Check seq */
  if (static_seq > 65535) static_seq = 0;

  /* Fill out ICMP struct */
  echo = (struct icmp *) malloc (echo_datasize);
  memset (echo, 0, echo_datasize);
  echo->icmp_type = ICMP_ECHO;
  echo->icmp_code = 0;
  echo->icmp_id = getpid() & 0xFFFF;
  echo->icmp_seq = htons(static_seq);
  callback->echo_id = echo->icmp_id;
  echo->icmp_cksum = 0;
  echo->icmp_cksum = l_icmp_echo_checksum ((u_short *)echo, echo_datasize);
  static_seq++;

  /* Set request time */
  gettimeofday (&callback->req_tv, NULL);

  /* Send ping */
  num = sendto (callback->socket->sockfd, echo, echo_datasize, 0, (struct sockaddr*) &host_addr, sizeof(host_addr));
  free (echo);
  if (num == -1)
  { i_printf (2, "l_icmp_echo_send failed to call sent_to (%s)", strerror (errno)); l_icmp_echo_callback_free (callback); return NULL; }

  /* Add socket callback */
  callback->socketcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ, callback->socket, l_icmp_echo_send_socketcb, callback);
  if (!callback->socketcb)
  { i_printf (1, "l_icmp_echo_send failed to add socket callback"); l_icmp_echo_callback_free (callback); return NULL; }

  /* Add timeout timer */
  callback->timeout_timer = i_timer_add (self, timeout_sec, 0, l_icmp_echo_send_timeout, callback);
  if (!callback->timeout_timer)
  { i_printf (1, "l_icmp_echo_send failed to add timeout timer"); l_icmp_echo_callback_free (callback); return NULL; }

  return callback;
}

int l_icmp_echo_send_socketcb (i_resource *self, i_socket *socket, void *passdata)
{
  /* Always return -1 to remove callback 
   * unless the ICMP echo response recvd is
   * not for us
   */
  int recv_count;
  socklen_t from_len;
  int hlen;
  int result;
  char buf[MAX_PACKET_SIZE];
  struct ip *iphdr;
  struct icmp *echo;
  struct sockaddr_in from_addr;
  struct in_addr from_inaddr;
  l_icmp_echo_callback *callback;

  callback = passdata;
  callback->socketcb = NULL;

  /* Recv response */
  recv_count = recvfrom (socket->sockfd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr*) &from_addr, &from_len);
  if (recv_count == -1)
  { 
    i_printf (1, "l_icmp_echo_send_socketcb failed to recvfrom %i bytes from socket", callback->echo_datasize);
    if (callback->callback_func)
    { callback->callback_func (self, ECHORES_FAIL, callback, callback->passdata); }
    l_icmp_echo_callback_free (callback);
    return -1; 
  }
  from_inaddr.s_addr = from_addr.sin_addr.s_addr;

  /* Check response size*/
  iphdr = (struct ip *) buf;
  hlen = iphdr->ip_hl << 2;
  if (recv_count < hlen + ICMP_MINLEN)
  {
    i_printf (1, "l_icmp_echo_send_socketcb recvd packet too short from %s (%i bytes)",
      inet_ntoa(from_inaddr), recv_count); 
    if (callback->callback_func)
    { callback->callback_func (self, ECHORES_FAIL, callback, callback->passdata); }
    l_icmp_echo_callback_free (callback);
    return -1; 
  }

  /* Check ICMP ID */
  echo = (struct icmp *) (buf + hlen);
  if (echo->icmp_id != callback->echo_id)
  { 
    /* Not our echo, count be dest unreach etc */
    i_printf (2, "l_icmp_echo_send_socketcb recv echo not intended us from %s", 
      inet_ntoa(from_inaddr));
    return 0;
  }

  /* Check for ICMP error response */
  switch (echo->icmp_type)
  {
    case ICMP_ECHOREPLY:
      result = ECHORES_OK;
      break;
    case ICMP_UNREACH:
      result = echo->icmp_code;
      break;
    case ICMP_SOURCEQUENCH:
      result = ECHORES_SOURCEQUENCH;
      break;
    case ICMP_REDIRECT:
      result = ECHORES_REDIRECT;
      break;
    case ICMP_TIMXCEED:
      result = ECHORES_TIMXCEED;
      break;
    case ICMP_PARAMPROB:
      result = ECHORES_PARAMPROB;
      break;
    default:
      result = ECHORES_FAIL;
  }

  /* Set response time */
  gettimeofday (&callback->resp_tv, NULL);
  
  /* Run Callback */
  if (callback->callback_func)
  { callback->callback_func (self, result, callback, callback->passdata); }
  l_icmp_echo_callback_free (callback);

  return -1;
}

int l_icmp_echo_send_timeout (i_resource *self, i_timer *timer, void *passdata)
{
  /* Always return -1 to remove timer */
  l_icmp_echo_callback *callback = passdata;

  callback->timeout_timer = NULL;
  
  if (callback->callback_func)
  { callback->callback_func (self, ECHORES_TIMEOUT, callback, callback->passdata); }
  
  l_icmp_echo_callback_free (callback);

  return -1;
}

/* Checksum calculation */

int l_icmp_echo_checksum (u_short *echoptr, int echo_datasize)
{
  register long sum = 0;
  u_short answer = 0;

  while (echo_datasize > 1)
  {
    sum += *echoptr++;
    echo_datasize -= 2;
  }

  if (echo_datasize == 1)
  {
    *(u_char *) &answer = *(u_char *) echoptr;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  answer = ~sum;

  return answer;
}

/* Callback Struct Manipulation */

l_icmp_echo_callback* l_icmp_echo_callback_create ()
{
  l_icmp_echo_callback *cb;

  cb = (l_icmp_echo_callback *) malloc (sizeof(l_icmp_echo_callback));
  if (!cb)
  { i_printf (1, "l_icmp_echo_callback_create failed to malloc l_icmp_echo_callback struct"); return NULL; }
  memset (cb, 0, sizeof(l_icmp_echo_callback));

  return cb;
}

void l_icmp_echo_callback_free (void *cbptr)
{
  l_icmp_echo_callback *cb = cbptr;

  if (!cb) return;

  if (cb->socket)
  { i_socket_free (cb->socket); }
  if (cb->socketcb)
  { i_socket_callback_remove (cb->socketcb); }
  if (cb->timeout_timer)
  { i_timer_remove (cb->timeout_timer); }

  free (cb);
}

