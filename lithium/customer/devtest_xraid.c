#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>

#include "main.h"
#include "devtest.h"

/*
 * Xraid plist retrieval
 */

l_devtest_xraid_req* l_devtest_xraid_req_create ()
{
  l_devtest_xraid_req *req;
  req = (l_devtest_xraid_req *) malloc (sizeof(l_devtest_xraid_req));
  memset (req, 0, sizeof(l_devtest_xraid_req));
  return req;
}

void l_devtest_xraid_req_free (void *reqptr)
{
  l_devtest_xraid_req *req = reqptr;
  if (!req) return;
  if (req->action) free (req->action);
  if (req->plistout) free (req->plistout);
  if (req->plistbuf) free (req->plistbuf);
  if (req->sock) i_socket_free (req->sock);
  if (req->sockcb) i_socket_callback_remove (req->sockcb);
  if (req->timeout_timer) i_timer_remove (req->timeout_timer);
  free (req);
}

int l_devtest_xraid (i_resource *self, i_device *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata)
{
  l_devtest_xraid_req *req;

  /* Check for vital config */
  if (!dev->ip_str) return -1;
  if (!dev->password_str) return -1;

  /* Create req struct */
  req = l_devtest_xraid_req_create ();
  req->cbfunc = cbfunc;
  req->passdata = passdata;
  char *acp_password = l_devtest_xraid_acp_crypt (dev->password_str);
  asprintf (&req->action, "POST /cgi-bin/acp-get HTTP/1.1\r\n");
  asprintf (&req->plistout, "Content-Length: 781\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<array>\n\t<string>syNm</string>\n\t<string>syDN</string>\n\t<string>syPN</string>\n\t<string>syCt</string>\n\t<string>syDs</string>\n\t<string>syLo</string>\n\t<string>syVs</string>\n\t<string>syPR</string>\n\t<string>syGP</string>\n\t<string>syUT</string>\n\t<string>syFl</string>\n\t<string>laMA</string>\n\t<string>laCV</string>\n\t<string>laIP</string>\n\t<string>laSM</string>\n\t<string>laRA</string>\n\t<string>waC1</string>\n\t<string>waC2</string>\n\t<string>waC3</string>\n\t<string>laDC</string>\n\t<string>laIS</string>\n\t<string>smtp</string>\n\t<string>ntpC</string>\n\t<string>snAF</string>\n\t<string>svMd</string>\n\t<string>effF</string>\n\t<string>emNo</string>\n</array>\n</plist>\n", acp_password);

  /* Create socket */
  req->sock = i_socket_create_tcp (self, dev->ip_str, 80, l_devtest_xraid_conncb, req);
  if (!req->sock)
  { 
    i_printf (0, "l_devtest_xraid failed to create TCP socket to %s", dev->ip_str); 
    l_devtest_xraid_req_free (req); 
    return -1;
  }

  /* Start Timeout Timer */
  req->timeout_timer = i_timer_add (self, 30, 0, l_devtest_xraid_timeout, req);

  return 0;
}

int l_devtest_xraid_conncb (i_resource *self, i_socket *sock, void *passdata)
{
  l_devtest_xraid_req *req = passdata;

  if (sock)
  {
    /* Send request */
    i_socket_write (self, sock, req->action, strlen(req->action), NULL, NULL);
    i_socket_write (self, sock, req->plistout, strlen(req->plistout), NULL, NULL);

    /* Add callback */
    req->sockcb = i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, req->sock, l_devtest_xraid_sockreadcb, req);
  }
  else
  {
    /* Error connecting, socket will have been freed */
    req->sock = NULL;

    /* Fire (failed) callback */    
    req->cbfunc (self, 0, req->passdata);

    /* Free req */
    l_devtest_xraid_req_free (req);
  }
  return 0;
}

int l_devtest_xraid_sockreadcb (i_resource *self, i_socket *sock, void *passdata)
{
  int num;
  char buf[1024];
  l_devtest_xraid_req *req = passdata;
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
    req->cbfunc (self, 0, req->passdata);

    /* Free req */
    req->sockcb = NULL;
    l_devtest_xraid_req_free (req);

    return -1;
  }

  /* Check for end of plist */
  if (strstr (req->plistbuf, "</plist>"))
  {
    /* Fire callback */
    req->cbfunc (self, 1, req->passdata);

    /* Free req */
    req->sockcb = NULL;
    l_devtest_xraid_req_free (req);

    return -1;  /* Remove CB */
  }

  return 0;   /* Keep CB alive */
}

int l_devtest_xraid_timeout (i_resource *self, i_timer *timer, void *passdata)
{
  l_devtest_xraid_req *req = passdata;
  
  req->timeout_timer = NULL;
  req->cbfunc (self, 0, req->passdata);
  l_devtest_xraid_req_free (req);

  return -1;    /* Kill Timer */
}

char* l_devtest_xraid_acp_crypt (char *password_str)
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
