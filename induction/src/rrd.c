#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <netdb.h> 
#include <stdint.h>

#include "induction.h"
#include "rrd.h"
#include "rrdtool.h"
#include "path.h"
#include "files.h"
#include "configfile.h"
#include "timeutil.h"
#include "hierarchy.h"
#include "metric.h"

#define RRDTOOL_EXEC_STR "rrdtool -"

static int static_mars_sockfd = -1;
static struct sockaddr_in static_mars_addr;

int i_rrd_mars_sockfd ()
{
  if (static_mars_sockfd == -1)
  {
    struct hostent *he;

    if ((he=gethostbyname("localhost")) == NULL)
    { printf ("*** i_rrd_update failed to gethostbyname for localhost\n"); return -1; }

    if ((static_mars_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    { printf ("*** i_rrd_update failed to create socket\n"); return -1; }

    static_mars_addr.sin_family = AF_INET;
    static_mars_addr.sin_port = htons(51110);
    static_mars_addr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(static_mars_addr.sin_zero), 8);
  }

  return static_mars_sockfd;
}

/* Tools to interact with rrdtool */

int i_rrd_check_exists (i_resource *self, char *filename)
{
  /* Check a rrd file exist */

  int num;
  struct stat statbuf;

  if (!self || !filename) return -1;
  
  num = stat (filename, &statbuf);
  if (num != 0)
  { return -1; }

  return 0;
}

i_rrdtool_cmd *i_rrd_create (i_resource *self, char *filename, time_t start_time, time_t step_seconds, char *arg_str, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata) 
{
  char *command_str; char *start_time_str; char *step_seconds_str;

  if (start_time > 0)
  { asprintf (&start_time_str, "--start %li", start_time); }
  else
  { start_time_str = strdup(""); }

  if (step_seconds > 0)
  { asprintf (&step_seconds_str, "--step %li", step_seconds); }
  else
  { step_seconds_str = strdup (""); }

  asprintf (&command_str, "create '%s' %s %s %s\n", filename, start_time_str, step_seconds_str, arg_str);
  free (start_time_str);
  free (step_seconds_str);

  size_t data_len = strlen(command_str) + 1 + sizeof(uint32_t);
  char *data = (char *) malloc (data_len);
  char *dataptr = data;
  uint32_t priority_int = 0;
  memcpy (dataptr, &priority_int, sizeof(uint32_t));
  dataptr += sizeof(uint32_t);
  memcpy (dataptr, command_str, strlen(command_str)+1);  

  int sockfd = i_rrd_mars_sockfd();
  int num = sendto (sockfd, data, data_len, 0, (struct sockaddr *) &static_mars_addr, sizeof(struct sockaddr));
  free (data);
  if (num == -1)
  { i_printf (0, "i_rrd_update failed to send create packet to MARS (%s)", strerror(errno)); }
  if (num < (int) strlen(command_str))
  { i_printf (0, "i_rrd_update failed to send whole create packet to MARS. Sent %i bytes of %i", num, strlen(command_str)); }
  free (command_str);
  
  return NULL;
}

i_rrdtool_cmd* i_rrd_update (i_resource *self, int priority, char *filename, char *arg_str, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata)
{
  int num;
  char *command_str;

  asprintf (&command_str, "update '%s' %s\n", filename, arg_str);

  size_t data_len = strlen(command_str) + 1 + sizeof(uint32_t);
  char *data = (char *) malloc (data_len);
  char *dataptr = data;
  uint32_t priority_int = (uint32_t) priority;
  memcpy (dataptr, &priority_int, sizeof(uint32_t));
  dataptr += sizeof(uint32_t);
  memcpy (dataptr, command_str, strlen(command_str)+1);  

  int sockfd = i_rrd_mars_sockfd();
  num = sendto (sockfd, data, data_len, 0, (struct sockaddr *) &static_mars_addr, sizeof(struct sockaddr));
  free (data);
  if (num == -1)
  { i_printf (0, "i_rrd_update failed to send update packet to MARS (%s)", strerror(errno)); }
  if (num < (int) strlen(command_str))
  { i_printf (0, "i_rrd_update failed to send whole update packet to MARS. Sent %i bytes of %i", num, strlen(command_str)); }
  free (command_str);
  
  return NULL;
}

i_rrdtool_cmd* i_rrd_graph (i_resource *self, char *filename, time_t start_sec, time_t end_sec, char *title, char *y_label, unsigned int kbase, char *arg_str, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata)
{
  char *start_str;
  char *end_str;
  char *y_label_str;
  char *command_str;
  char *title_str;
  char *font_str;
  i_rrdtool_cmd *cmd;
                  
  /* Time Strings */

  if (start_sec > 0)
  { asprintf (&start_str, "--start=%li", start_sec); }
  else
  { start_str = strdup (""); }

  if (end_sec > 0)
  { asprintf (&end_str, "--end=%li", end_sec); }
  else
  { end_str = strdup (""); }

  /* Title String */

  if (title)
  { asprintf (&title_str, "--title=\"%s\"", title); }
  else
  { title_str = strdup (""); }

  /* Label String */

  if (y_label)
  { asprintf (&y_label_str, "--vertical-label=\"%s\"", y_label); }
  else
  { y_label_str = strdup (""); }

  /* Font */
  font_str = strdup ("");

  asprintf (&command_str, "graph '%s' -a PNG -E -c FONT#010101FF -c CANVAS#FFFFFF00 -c BACK#F6F6F600 -c SHADEA#F6F6F600 -c SHADEB#F6F6F600 -b %u %s %s %s %s %s %s", filename, kbase, start_str, end_str, title_str, y_label_str, font_str, arg_str);

  free (start_str);
  free (end_str);
  free (title_str);
  free (y_label_str);
  free (font_str);

  cmd = i_rrdtool_exec (self, filename, command_str, RRDFLAG_URGENT, cbfunc, passdata);
  free (command_str);
  if (!cmd)
  { i_printf (1, "i_rrd_graph failed to execute rrdtool"); return NULL; }
      
  return cmd;
}

/* PDF Graph */

i_rrdtool_cmd* i_rrd_pdfgraph (i_resource *self, char *imagepath, time_t start_sec, time_t end_sec, char *y_label, unsigned int kbase, char *args, int (*cbfunc) (), void *passdata)
{
  char *start_str;
  char *end_str;
  char *command_str;
  char *y_label_str;
  i_rrdtool_cmd *cmd;

  /* Time Strings */

  if (start_sec > 0)
  { asprintf (&start_str, "--start=%li", start_sec); }
  else
  { start_str = strdup (""); }

  if (end_sec > 0)
  { asprintf (&end_str, "--end=%li", end_sec); }
  else
  { end_str = strdup (""); }

  /* Label String */
  if (y_label)
  { asprintf (&y_label_str, "--vertical-label=\"%s\"", y_label); }
  else
  { y_label_str = strdup (""); }

  /* Create command string */
  asprintf (&command_str, "graphv '%s' -a PDF -E --only-graph --full-size-mode --font DEFAULT:8:Arial -c FONT#FFFFFFDD -c CANVAS#00000000 -c BACK#00000000 -c SHADEA#00000000 -c SHADEB#00000000 -b %u %s %s %s %s", imagepath, kbase, start_str, end_str, y_label_str, args);
  free (start_str);
  free (end_str);
  free (y_label_str);

  cmd = i_rrdtool_exec (self, imagepath, command_str, RRDFLAG_URGENT, cbfunc, passdata);
  free (command_str);
  if (!cmd)
  { i_printf (1, "i_rrd_pdfgraph failed to execute rrdtool"); return NULL; }

  return cmd;
}

/* XML Graph */

i_rrdtool_cmd* i_rrd_xmlgraph (i_resource *self, char *imagepath, time_t start_sec, time_t end_sec, char *y_label, unsigned int kbase, char *args, int (*cbfunc) (), void *passdata)
{
  char *start_str;
  char *end_str;
  char *command_str;
  char *y_label_str;
  char *font_str;
  i_rrdtool_cmd *cmd;

  /* Time Strings */

  if (start_sec > 0)
  { asprintf (&start_str, "--start=%li", start_sec); }
  else 
  { start_str = strdup (""); }
  
  if (end_sec > 0)
  { asprintf (&end_str, "--end=%li", end_sec); }
  else
  { end_str = strdup (""); }
  
  /* Label String */
  if (y_label)
  { asprintf (&y_label_str, "--vertical-label=\"%s\"", y_label); }
  else
  { y_label_str = strdup (""); }

  /* Font */
  font_str = strdup ("");
  
  /* Create command string */
  asprintf (&command_str, "graphv '%s' -E --only-graph --full-size-mode -g --font DEFAULT:8:Arial -c FONT#010101FF -c CANVAS#00000000 -c BACK#00000000 -c SHADEA#00000000 -c SHADEB#00000000 -b %u %s %s %s %s", imagepath, kbase, start_str, end_str, y_label_str, args);
  free (start_str);
  free (end_str);
  free (y_label_str);
  free (font_str);
  
  cmd = i_rrdtool_exec (self, imagepath, command_str, RRDFLAG_URGENT, cbfunc, passdata);
  free (command_str);
  if (!cmd)
  { i_printf (1, "i_rrd_xmlgraph failed to execute rrdtool"); return NULL; }

  return cmd;
}

/* Xporting */

i_rrdtool_cmd* i_rrd_xport (i_resource *self, time_t start_sec, time_t end_sec, char *rrdfullpath, int (*cbfunc) (), void *passdata)
{
  char *start_str;
  char *end_str;
  char *command_str;
  i_rrdtool_cmd *cmd;

  /* Time Strings */
  if (start_sec > 0)
  { asprintf (&start_str, "--start=%li", start_sec); }
  else
  { start_str = strdup (""); }
  if (end_sec > 0)
  { asprintf (&end_str, "--end=%li", end_sec); }
  else
  { end_str = strdup (""); }

  /* Create command string */
  asprintf (&command_str, "xport %s %s DEF:default_min='%s':default:MIN DEF:default_avg='%s':default:AVERAGE DEF:default_max='%s':default:MAX XPORT:default_min:Minimum XPORT:default_avg:Average XPORT:default_max:Maximum", start_str, end_str, rrdfullpath, rrdfullpath, rrdfullpath);
  free (start_str);
  free (end_str);

  cmd = i_rrdtool_exec (self, rrdfullpath, command_str, RRDFLAG_URGENT, cbfunc, passdata);
  free (command_str);
  if (!cmd)
  { i_printf (1, "i_rrd_xport failed to execute rrdtool"); return NULL; }

  return cmd;
}


/* Tune */

i_rrdtool_cmd* i_rrd_tune (i_resource *self, char *filename, char *arg_str, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata)
{
  char *command_str;
  i_rrdtool_cmd *cmd;

  asprintf (&command_str, "tune '%s' %s", filename, arg_str);

  cmd = i_rrdtool_exec (self, filename, command_str, 0, cbfunc, passdata);
  free (command_str);
  if (!cmd)
  { i_printf (1, "i_rrd_tune failed to execute rrdtool"); return NULL; }
        
  return cmd;
}

/* Comment escaping */

char* i_rrd_comment_escape (char *in)
{
  size_t i;
  int outlen;
  char *out;

  out = strdup (in);
  outlen = strlen (out) + 1;

  for (i=0; i < strlen (out); i++)
  {
    /* Loops through each character in out string */
    if (out[i] == ':')
    {
      /* Escape a colon */
      char *tmp;
      char *tmpptr;
      char *outptr;

      tmp = malloc (outlen + 1);        /* Alloc a temp buffer, one char larger */
      strcpy (tmp, out);                /* Copy string to tmp */
      tmp[i] = '\\';                    /* Add the backslash */
      tmpptr = tmp + i + 1;             /* Move tmpptr to just past the slash */
      outptr = out + i;                 /* Move the outptr to the colon */
      strcpy (tmpptr, outptr);          /* Copy the rest of the outstr to tmpptr */

      free (out);                       /* Obsolete */
      out = tmp;                        /* Set new out buffer */
      outlen++;                         /* Increment out buffersize */
      i++;                              /* Moves past the escaped : */
    }
  }

  return out;
}

      
