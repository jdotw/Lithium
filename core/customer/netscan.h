/*
 * Network Scanning -- nmap based scanning
 */

#include <libxml/parser.h>

typedef struct l_netscan_proc_s
{
  /* Network */
  char *network_str;
  char *mask_str;
  
  /* Process */
  int pid;
  int readfd;
  int writefd;

  /* Socket */
  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;

  /* Output */
  char *output_str; 

  /* Callbacks */
  struct i_callback_s *cb;            /* Callback to be called after execution */

} l_netscan_proc;

/* netscan.c */
int l_netscan_parse (i_resource *self, char *network_str, char *mask_str, char *xml_str);
char* l_netscan_attr ();

/* netscan_proc.c */
l_netscan_proc* l_netscan_proc_create ();
void l_netscan_proc_free (void *procptr);
l_netscan_proc* l_netscan_scan (i_resource *self, char *network_str, char *mask_str, int (*cbfunc) (), void *passdata);
int l_netscan_scan_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_netscan_scan_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);

