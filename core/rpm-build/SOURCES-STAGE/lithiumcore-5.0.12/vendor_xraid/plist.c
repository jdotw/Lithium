#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/socket.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/avail.h"

#include "main.h"
#include "plist.h"

/*
 * Xraid plist retrieval
 */

v_plist_req* v_plist_get (i_resource *self, char *action, char *plistout, int (*cbfunc) (i_resource *self, char *plist, void *passdata), void *passdata)
{
  v_plist_req *req;

  /* Create req struct */
  req = v_plist_req_create ();
  req->cbfunc = cbfunc;
  req->passdata = passdata;
  req->action = strdup (action);
  if (plistout) req->plistout = strdup (plistout);
  gettimeofday (&req->req_tv, NULL);

  /* Create socket */
  req->sock = i_socket_create_tcp (self, self->hierarchy->dev->ip_str, 80, v_plist_get_conncb, req);
  if (!req->sock)
  { 
    i_printf (1, "v_plist_get failed to create TCP socket to %s", self->hierarchy->dev->ip_str); 
    v_plist_req_free (req); 
    return NULL;
  }

  return req;
}

void v_plist_get_cancel (i_resource *self, v_plist_req *req)
{
  v_plist_req_free (req);
}

int v_plist_get_conncb (i_resource *self, i_socket *sock, void *passdata)
{
  v_plist_req *req = passdata;
  if (sock)
  {
    /* Send request */
    i_socket_write (self, sock, req->action, strlen(req->action), NULL, NULL);
    if (req->plistout)
    { i_socket_write (self, sock, req->plistout, strlen(req->plistout), NULL, NULL); }

    /* Add callback */
    i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, req->sock, v_plist_get_sockreadcb, req);
  }
  else
  {
    /* Error connecting, socket will have been freed */
    req->sock = NULL;

    /* Fire (failed) callback */    
    req->cbfunc (self, req, req->passdata);

    /* Free req */
    v_plist_req_free (req);

    /* Record failure */
    l_avail_record_fail (v_xraid_availobj());
  }
  return 0;
}

int v_plist_get_sockreadcb (i_resource *self, i_socket *sock, void *passdata)
{
  int num;
  char buf[1024];
  v_plist_req *req = passdata;
  int bytesrecvd = 0;

  /* Read all we can */
  while ((num=read (sock->sockfd, &buf, 1023)) > 0)
  {
    buf[num] = '\0';
    if (req->plistbuf)
    {
      char *x;
      asprintf (&x, "%s%s", req->plistbuf, buf);
      free (req->plistbuf);
      req->plistbuf = x;
    }
    else
    {
      req->plistbuf = strdup (buf);
    }

    bytesrecvd += num;
  }
  if (bytesrecvd < 1)
  {
    /* Failed to receive data, clear buffer */
    if (req->plistbuf)
    {
      free (req->plistbuf);
      req->plistbuf = NULL;
    }

    /* Fire (failed) callback */    
    req->cbfunc (self, req, req->passdata);

    /* Free req */
    v_plist_req_free (req);

    /* Record failure */
    l_avail_record_fail (v_xraid_availobj());

    return -1;
  }

  /* Check for end of plist */
  if (strstr (req->plistbuf, "</plist>"))
  {
    /* End of plist, process it */
    gettimeofday (&req->resp_tv, NULL);
    
    /* Move to start of XML */
    char *startptr = strstr (req->plistbuf, "\n<?xml");
    if (startptr)
    {
      *startptr = '\0';
      startptr++;
    }

    /* Create xmlDoc */
    req->plist = xmlReadMemory (startptr, strlen(startptr)+1, "plist.xml", "UTF-8", 0);
    req->root_node = xmlDocGetRootElement (req->plist);

    /* Fire callback */
    req->cbfunc (self, req, req->passdata);

    /* Record availability */
    l_avail_record_ok (v_xraid_availobj(), &req->req_tv, &req->resp_tv);

    /* Free req */
    v_plist_req_free (req);

    return -1;  /* Remove CB */
  }

  return 0;   /* Keep CB alive */
}

char* v_plist_acp_crypt (char *password_str)
{
  int key[16] = { 91, 111, 175, 93, 157, 91, 14, 19, 81, 242, 218, 29, 231, 232, 214, 115 };
  unsigned int i = 0;
  unsigned int j = 0;
  char *crypt_str;
  if (password_str) crypt_str = strdup (password_str);
  else crypt_str = strdup ("public");
  char *b64_str;

  for (j=0; j < strlen(password_str); j++)
  {
    if (i >= 16) i = 0;
    int x = password_str[j] ^ key[i];
    int y = 85 + j;
    int z = x ^ y;
    crypt_str[j] = z;
    i++;
  }

  base64_encode_alloc (crypt_str, strlen(crypt_str), &b64_str);
  free (crypt_str);

  return b64_str;
}

/* C89 compliant way to cast 'char' to 'unsigned char'. */
static inline unsigned char
to_uchar (char ch)
{
  return ch;
}

/* Base64 encode IN array of size INLEN into OUT array of size OUTLEN.
   If OUTLEN is less than BASE64_LENGTH(INLEN), write as many bytes as
   possible.  If OUTLEN is larger than BASE64_LENGTH(INLEN), also zero
   terminate the output buffer. */
void base64_encode (const char *in, size_t inlen, char *out, size_t outlen)
{
  static const char b64str[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  while (inlen && outlen)
    {
      *out++ = b64str[(to_uchar (in[0]) >> 2) & 0x3f];
      if (!--outlen)
  break;
      *out++ = b64str[((to_uchar (in[0]) << 4)
           + (--inlen ? to_uchar (in[1]) >> 4 : 0))
          & 0x3f];
      if (!--outlen)
  break;
      *out++ =
  (inlen
   ? b64str[((to_uchar (in[1]) << 2)
       + (--inlen ? to_uchar (in[2]) >> 6 : 0))
      & 0x3f]
   : '=');
      if (!--outlen)
  break;
      *out++ = inlen ? b64str[to_uchar (in[2]) & 0x3f] : '=';
      if (!--outlen)
  break;
      if (inlen)
  inlen--;
      if (inlen)
  in += 3;
    }

  if (outlen)
    *out = '\0';
}

/* Allocate a buffer and store zero terminated base64 encoded data
   from array IN of size INLEN, returning BASE64_LENGTH(INLEN), i.e.,
   the length of the encoded data, excluding the terminating zero.  On
   return, the OUT variable will hold a pointer to newly allocated
   memory that must be deallocated by the caller.  If output string
   length would overflow, 0 is returned and OUT is set to NULL.  If
   memory allocation failed, OUT is set to NULL, and the return value
   indicates length of the requested memory block, i.e.,
   BASE64_LENGTH(inlen) + 1. */
size_t base64_encode_alloc (const char *in, size_t inlen, char **out)
{
  size_t outlen = 1 + BASE64_LENGTH (inlen);

  /* Check for overflow in outlen computation.
   *
   * If there is no overflow, outlen >= inlen.
   *
   * If the operation (inlen + 2) overflows then it yields at most +1, so
   * outlen is 0.
   *
   * If the multiplication overflows, we lose at least half of the
   * correct value, so the result is < ((inlen + 2) / 3) * 2, which is
   * less than (inlen + 2) * 0.66667, which is less than inlen as soon as
   * (inlen > 4).
   */
  if (inlen > outlen)
    {
      *out = NULL;
      return 0;
    }

  *out = malloc (outlen);
  if (!*out)
    return outlen;

  base64_encode (in, inlen, *out, outlen);

  return outlen - 1;
}
